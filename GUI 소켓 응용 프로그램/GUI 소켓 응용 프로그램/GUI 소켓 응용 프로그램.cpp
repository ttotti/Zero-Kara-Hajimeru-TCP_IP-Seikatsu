#pragma comment(lib,"ws2_32")
#include <WinSock2.h>
#include <stdlib.h>
#include <Windows.h>
// 가변 길이 인자를 처리하기 위해 va_list타입과, va_start(), va_end() 함수를 사용하는데, 이를 위해 필요한 헤더 파일이다
#include <stdio.h>

#define SERVERPORT 9000
#define BUFSIZE 512

// 원도우 프로시저
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// 편집 컨트롤 출력 함수
void DisplayText(const char* fmt, ...);

// 오루 출력 함수
void err_quit(const char* msg);
void err_display(const char* msg);

// 소켓 통신 스레드 함수
DWORD WINAPI ServerMain(LPVOID arg);
DWORD WINAPI ProcessClient(LPVOID arg);

// 인스턴스 핸들
HINSTANCE hInst;
// 편집 컨트롤
HWND hEdit;

// 임계 영역
// 스레드 둘 이상이 편집 컨트롤에 접근해 출력하므로, 데이터가 뒤섞이는 상황을 방지하기 위한 것이다
CRITICAL_SECTION cs;

// 메인 함수
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// 인스턴스 행들을 전역 변수에 저장한다
	hInst = hInstance;

	// 임계 영역 변수의 초기화와 종료 작업을 담당한다
	InitializeCriticalSection(&cs);

	// 윈도우 클래스 등록, 윈도우 생성, 메시지 루프 부분
	WNDCLASS wndclass;
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = "MyWndClass";

	if (!RegisterClass(&wndclass))
		return 1;

	HWND hWnd = CreateWindow("MyWndclass", "WinApp", WS_OVERLAPPEDWINDOW, 0, 0, 600, 200, NULL, NULL, hInstance, NULL);
	if (hWnd == NULL)
		return 1;

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	// 소켓 통신 스레드 생성
	// 메시지 루프를 구동하기 전에 TCP 서버의 메인 코드를 별도의 스레드로 시작한다
	CreateThread(NULL, 0, ServerMain, NULL, 0, NULL);

	// 메시지 루프
	MSG msg;
	while (GetMessage(&msg, 0, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	// 임계 영역의 종료
	DeleteCriticalSection(&cs);

	return msg.wParam;
}

// 윈도우 프로시저
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
		// 편집 컨트롤을 메인 윈도우의 자식 윈도우로 생성한 후 DisplayText() 함수를 사용해 편집 컨트롤에 문자열을 출력하고 있다
		// 편집 컨트롤 생성 시 ES_READONLY 스타일을 주어 읽기만 가능하게 한다, 이는 편집 컨트롤을 출력 전용으로 사용한다는 의미다
		hEdit = CreateWindow("edit", NULL, WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL | ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_MULTILINE | ES_READONLY, 0, 0, 0, 0, hWnd, (HMENU)100, hInst, NULL);

		DisplayText("간단한 GUI 응용 프로그램입니다.\r\n");
		DisplayText("인스턴스 핸들 값은 %#x입니다.\r\n", hInst);
		return 0;

		// WM_SIZE 메시지는 메인 원도우의 크기가 변경될 때마다 발생한다
		// 이때 lParam의 하위 16비트와 상위 16비트에는 각각 클라이언트 영역의 폭과 높이가 들어 있다
		// 이 정보를 이용해 편집 컨트롤이 메인 원도우의 클라이언트 영역 전체를 차지하도록 크기를 조정하고 있다
	case WM_SIZE:
		MoveWindow(hEdit, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
		return 0;

		// WM_SETFOCUS 메시지는 메인 원도우가 키보드 포커스를 얻을 때 발생한다
		// 이는 메인 원도우가 키보드 메시지를 받을 수 있음을 의미한다
		// SetFocus() 함수를 호출하면 키보드 포커스를 다른 원도우로 전환할 수 있다
		// 여기서는 편집 컨트롤이 키보드 포커스를 소유하게 함으로써 키보드 입력을 받을 수 있게 한다
	case WM_SETFOCUS:
		SetFocus(hEdit);
		return 0;

		// WM_DESTROY 메시지는 메인 윈도우를 닫을 때 발생하므로 각종 청소 작업(동적 메모리 해제, 파일 닫기 등)을 수행하는 데 적합하다
		// PostQuitMessage() 함수는 응용 프로그램 메시지 큐에 WM_QUIT 메시지를 넣음으로써 GetMessage() 함수가 0을 리턴하게 한다
		// 따라서 메시지 루프가 끝나고 메인 함수가 리턴하므로 응용 프로그램이 종료한다
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

// 출력 함수
void DisplayText(const char * fmt, ...)
{
	// va_list 타입과 va_start(), va_end() 함수는 가변 길이 인자를 지원하기 위해 표준 C 라이브러리 함수에서 제공한다
	// 편집 컨트롤에 출력할 문자열은 vsprintf() 함수를 이용해 cbuf[] 배열에 저장한다
	va_list arg;
	va_start(arg, fmt);

	// 편집 컨트롤에 출력할 문자열 저장을 위한 버퍼다
	// 서버의 수신 버퍼 크기를 감안해 BUFSIZE + 256 으로 변경했다
	char cbuf[BUFSIZE + 256];
	vsprintf(cbuf, fmt, arg);

	// 스레드 둘 이상이 편집 컨트롤에 접근할 수 있으므로 출력이 뒤섞이지 않도록 임계 영역으로 보호한다
	EnterCriticalSection(&cs);

	// 편집 컨트롤에 문자열을 추가하는 코드이다
	// GetWindowTextLength() 함수로 편집 컨트롤에 들어 있는 문자열의 길이를 얻은 후
	// EM_SETSEL / EM_REPLACESEL 메시지를 이용해 맨 끝에 새로운 문자열을 추가한다
	int nLength = GetWindowTextLength(hEdit);
	SendMessage(hEdit, EM_SETSEL, nLength, nLength);
	SendMessage(hEdit, EM_REPLACESEL, FALSE, (LPARAM)cbuf);

	// 스레드 둘 이상이 편집 컨트롤에 접근할 수 있으므로 출력이 뒤섞이지 않도록 임계 영역으로 보호한다
	LeaveCriticalSection(&cs);

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

// 소켓 통신 스레드 함수
// 특별히 변경하거나 한거 없이 printf() 대신 DisplayText() 를 사용했다
DWORD WINAPI ServerMain(LPVOID arg)
{
	int retval;

	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET)
		err_quit("socket()");

	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR)
		err_quit("bind()");

	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR)
		err_quit("listen()");

	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;
	HANDLE hThread;

	while (1)
	{
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET)
		{
			err_display("accept()");
			break;
		}

		DisplayText("\r\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\r\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

		hThread = CreateThread(NULL, 0, ProcessClient, (LPVOID)client_sock, 0, NULL);
		if (hThread == NULL)
		{
			closesocket(client_sock);
		}
		else
		{
			CloseHandle(hThread);
		}

	}

	closesocket(listen_sock);

	WSACleanup();
	return 0;
}

DWORD WINAPI ProcessClient(LPVOID arg)
{
	SOCKET client_sock = (SOCKET)arg;
	int retval;
	SOCKADDR_IN clientaddr;
	int addrlen;
	char buf[BUFSIZE + 1];

	addrlen = sizeof(clientaddr);
	getpeername(client_sock, (SOCKADDR*)&clientaddr, &addrlen);

	while (1)
	{
		retval = recv(client_sock, buf, BUFSIZE, 0);
		if (retval == SOCKET_ERROR)
		{
			err_display("recv()");
			break;
		}
		else if (retval == 0)
			break;

		buf[retval] = '\0';
		DisplayText("[TCP/%s:%d] %s\r\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port), buf);

		retval = send(client_sock, buf, retval, 0);
		if (retval == SOCKET_ERROR)
		{
			err_display("send()");
			break;
		}
	}

	closesocket(client_sock);

	DisplayText("[TCP 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d\r\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

	return 0;
}
