#pragma comment (lib,"ws2_32")
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")

#include <WinSock2.h>
#include <stdlib.h>
#include <Windows.h>
#include <stdio.h>
#include "resource.h"

#define SERVERIP "127.0.0.1"
#define SERVERPORT 9000
#define BUFSIZE 512

// 대화상자 프로시저
BOOL CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);

// 편집 컨트롤 출력 함수
void DisplayText(const char* fmt, ...);

// 오류 출력 함수
void err_quit(const char* msg);
void err_display(const char* msg);

// 사용자 정의 데이터 수신 함수
int recvn(SOCKET s, char* buf, int len, int flags);

// 소켓 통신 스레드 함수
// TCPClient 예제의 메인 함수 부분을 담고 있는 스레드 함수다
DWORD WINAPI ClientMain(LPVOID arg);

SOCKET sock;                    // 소켓
char buf[BUFSIZE + 1];          // 데이터 송수신 버퍼
HANDLE hReadEvent, hWriteEvent; // 이벤트 // 스레드 동기화를 위해 이벤트를 사용한다
HWND hSendButton;               // 보내기 버튼 // <보내기> 버튼에 언제든지 접근할 수 있도록 핸들 값을 저장할 전역 변수를 선언했다

// 편집 컨트롤
// 두 편집 컨트롤에 언제든지 접근할 수 있도록 핸들 값을 저장할 전역 변수를 선언했다
HWND hEdit1, hEdit2;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// 이벤트 생성
	// 이벤트 두 개를 생성한다, hReadEvent 는 신호 상태로, hWriteEvent 는 비신호 상태로 시작한다
	hReadEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
	if (hReadEvent == NULL) return 1;
	hWriteEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (hWriteEvent == NULL) return 1;

	// 소켓 통신 스레드 생성
	// TCP 클라이언트의 메인 코드를 별도의 스레드로 시작한다
	CreateThread(NULL, 0, ClientMain, NULL, 0, NULL);

	// 대화상자 생성
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgProc);

	// 이벤트 제거
	// 사용을 마치면 이벤트를 제거한다
	CloseHandle(hReadEvent);
	CloseHandle(hWriteEvent);

	// closesocket()
	closesocket(sock);

	// 윈속 종료
	WSACleanup();

	return 0;
}

// 대화상자 프로시저
BOOL CALLBACK DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		// 두 편집 컨트롤에 언제든지 접근할 수 있도록 전역 변수에 핸들 값을 저장해둔다
		// GetDlgItem() 을 호출하면 컨트롤의 핸들 값을 얻을 수 있다
		hEdit1 = GetDlgItem(hDlg, IDC_EDIT1);
		hEdit2 = GetDlgItem(hDlg, IDC_EDIT2);

		// <보내기> 버튼에 언제든지 접근할 수 있도록 전역 변수에 핸들 값을 저장해둔다
		hSendButton = GetDlgItem(hDlg, IDOK);

		// 위쪽 편집 컨트롤에 입력할 수 있는 글자의 최대 개수를 설정한다
		// SendMessage(편집 컨트롤ID, EM_SETLIMITTEXT, BUFSIZE, 0) 을 호출하면 된다
		SendMessage(hEdit1, EM_SETLIMITTEXT, BUFSIZE, 0);
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			// 보내기 버튼 비활성화
			// <보내기> 버튼을 두 번 연속 누를 수 없도록 비활성화한다
			EnableWindow(hSendButton, FALSE);
			// 읽기 완료 기다리기
			// hReadEvent 가 신호 상태가 되길 기다린다, 처음에는 신호 상태므로 즉시 다음 코드로 진행한다
			WaitForSingleObject(hReadEvent, INFINITE);

			// 위쪽 편집 컨트롤에 입력된 문자열을 얻어서 아래쪽 편집 컨트롤에 출력한다
			// GetDlgItemText(대화상자 핸들, 컨트롤 ID, 버퍼 시작 주소, 버퍼 길이) 형식으로 호출하면
			// 편집 컨트롤에 입력된 문자열열 버퍼에 저장할 수 있다
			GetDlgItemText(hDlg, IDC_EDIT1, buf, BUFSIZE + 1);

			DisplayText("[보낼 데이터] %s\r\n", buf);

			// 쓰기 완료 알리기
			// 응용 프로그램 버퍼에 데이터를 저장했음을 알리기 위해 hWriteEvent 를 신호 상태로 만든다
			SetEvent(hWriteEvent);
			// 포커스 함수
			SetFocus(hEdit1);

			// 위쪽 편집 컨트롤에 입력된 문자열이 모두 선택 상태가 되게 한다
			// SendMessage(컨트롤ID, EM_SETSEL, 0, -1) 형식으로 호출하면 된다
			SendMessage(hEdit1, EM_SETSEL, 0, -1);
			return TRUE;

		case IDCANCEL:
			EndDialog(hDlg, IDCANCEL);
			return TRUE;
		}

		return FALSE;
	}

	return FALSE;
}

// 편집 컨트롤 출력 함수
void DisplayText(const char* fmt, ...)
{
	va_list arg;
	va_start(arg, fmt);

	// 글자의 최대 개수를 감안해 버퍼 크기를 수정했다
	char cbuf[BUFSIZE + 256];
	vsprintf(cbuf, fmt, arg);

	int nLength = GetWindowTextLength(hEdit2);
	SendMessage(hEdit2, EM_SETSEL, nLength, nLength);
	SendMessage(hEdit2, EM_REPLACESEL, FALSE, (LPARAM)cbuf);

	va_end(arg);
}

// 오류 출력 함수로 printf() 대신 DisplayText() 를 사용하도록 변경했다
void err_quit(const char * msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

void err_display(const char * msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
	DisplayText("[%s] %s", msg, (char*)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

// 사용자 정의 데이터 수신 함수
int recvn(SOCKET s, char* buf, int len, int flags)
{
	int received;
	char* ptr = buf;
	int left = len;

	while (left > 0)
	{
		received = recv(s, ptr, left, flags);

		if (received == SOCKET_ERROR)
			return SOCKET_ERROR;
		else if (received == 0)
			break;

		left -= received;
		ptr += received;
	}

	return (len - left);
}

// TCP 클라이언트 시작 부분
DWORD WINAPI ClientMain(LPVOID arg)
{
	int retval;

	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
		err_quit("socket()");

	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = connect(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR)
		err_quit("connect()");

	while (1)
	{
		printf("동작\n");
		// 쓰기 완료 기다리기
		// hWriteEvent 가 신호 상태가 되기를 기다린다
		WaitForSingleObject(hWriteEvent, INFINITE);

		if (strlen(buf) == 0)
		{
			// <보내기> 버튼을 활성화한 후 hReadEvent 를 신호 상태로 만든다
			// 보내기 버튼 활성화
			EnableWindow(hSendButton, TRUE);
			// 읽기 완료 알리기
			SetEvent(hReadEvent);
			continue;
		}

		retval = send(sock, buf, strlen(buf), 0);
		if (retval == SOCKET_ERROR)
		{
			err_display("send()");
			break;
		}
		DisplayText("[TCP 클라이언트] %d바이트를 보냈습니다\r\n", retval);

		retval = recvn(sock, buf, retval, 0);
		if (retval == SOCKET_ERROR)
		{
			err_display("recv()");
			break;
		}
		else if (retval == 0)
			break;

		buf[retval] = '\0';
		DisplayText("[TCP 클라이언트] %d바이트를 받았습니다\r\n", retval);
		DisplayText("[받은 데이터] %s\r\n", buf);

		// <보내기> 버튼을 활성화한 후 hReadEvent 를 신호 상태로 만든다
		// 보내기 버튼 활성화
		EnableWindow(hSendButton, TRUE);
		// 읽기 완료 알리기
		SetEvent(hReadEvent);
	}

	return 0;
}