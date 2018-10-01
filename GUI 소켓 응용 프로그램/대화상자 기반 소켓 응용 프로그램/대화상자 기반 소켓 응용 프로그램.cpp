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

// ��ȭ���� ���ν���
BOOL CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);

// ���� ��Ʈ�� ��� �Լ�
void DisplayText(const char* fmt, ...);

// ���� ��� �Լ�
void err_quit(const char* msg);
void err_display(const char* msg);

// ����� ���� ������ ���� �Լ�
int recvn(SOCKET s, char* buf, int len, int flags);

// ���� ��� ������ �Լ�
// TCPClient ������ ���� �Լ� �κ��� ��� �ִ� ������ �Լ���
DWORD WINAPI ClientMain(LPVOID arg);

SOCKET sock;                    // ����
char buf[BUFSIZE + 1];          // ������ �ۼ��� ����
HANDLE hReadEvent, hWriteEvent; // �̺�Ʈ // ������ ����ȭ�� ���� �̺�Ʈ�� ����Ѵ�
HWND hSendButton;               // ������ ��ư // <������> ��ư�� �������� ������ �� �ֵ��� �ڵ� ���� ������ ���� ������ �����ߴ�

// ���� ��Ʈ��
// �� ���� ��Ʈ�ѿ� �������� ������ �� �ֵ��� �ڵ� ���� ������ ���� ������ �����ߴ�
HWND hEdit1, hEdit2;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// �̺�Ʈ ����
	// �̺�Ʈ �� ���� �����Ѵ�, hReadEvent �� ��ȣ ���·�, hWriteEvent �� ���ȣ ���·� �����Ѵ�
	hReadEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
	if (hReadEvent == NULL) return 1;
	hWriteEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (hWriteEvent == NULL) return 1;

	// ���� ��� ������ ����
	// TCP Ŭ���̾�Ʈ�� ���� �ڵ带 ������ ������� �����Ѵ�
	CreateThread(NULL, 0, ClientMain, NULL, 0, NULL);

	// ��ȭ���� ����
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgProc);

	// �̺�Ʈ ����
	// ����� ��ġ�� �̺�Ʈ�� �����Ѵ�
	CloseHandle(hReadEvent);
	CloseHandle(hWriteEvent);

	// closesocket()
	closesocket(sock);

	// ���� ����
	WSACleanup();

	return 0;
}

// ��ȭ���� ���ν���
BOOL CALLBACK DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		// �� ���� ��Ʈ�ѿ� �������� ������ �� �ֵ��� ���� ������ �ڵ� ���� �����صд�
		// GetDlgItem() �� ȣ���ϸ� ��Ʈ���� �ڵ� ���� ���� �� �ִ�
		hEdit1 = GetDlgItem(hDlg, IDC_EDIT1);
		hEdit2 = GetDlgItem(hDlg, IDC_EDIT2);

		// <������> ��ư�� �������� ������ �� �ֵ��� ���� ������ �ڵ� ���� �����صд�
		hSendButton = GetDlgItem(hDlg, IDOK);

		// ���� ���� ��Ʈ�ѿ� �Է��� �� �ִ� ������ �ִ� ������ �����Ѵ�
		// SendMessage(���� ��Ʈ��ID, EM_SETLIMITTEXT, BUFSIZE, 0) �� ȣ���ϸ� �ȴ�
		SendMessage(hEdit1, EM_SETLIMITTEXT, BUFSIZE, 0);
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			// ������ ��ư ��Ȱ��ȭ
			// <������> ��ư�� �� �� ���� ���� �� ������ ��Ȱ��ȭ�Ѵ�
			EnableWindow(hSendButton, FALSE);
			// �б� �Ϸ� ��ٸ���
			// hReadEvent �� ��ȣ ���°� �Ǳ� ��ٸ���, ó������ ��ȣ ���¹Ƿ� ��� ���� �ڵ�� �����Ѵ�
			WaitForSingleObject(hReadEvent, INFINITE);

			// ���� ���� ��Ʈ�ѿ� �Էµ� ���ڿ��� �� �Ʒ��� ���� ��Ʈ�ѿ� ����Ѵ�
			// GetDlgItemText(��ȭ���� �ڵ�, ��Ʈ�� ID, ���� ���� �ּ�, ���� ����) �������� ȣ���ϸ�
			// ���� ��Ʈ�ѿ� �Էµ� ���ڿ��� ���ۿ� ������ �� �ִ�
			GetDlgItemText(hDlg, IDC_EDIT1, buf, BUFSIZE + 1);

			DisplayText("[���� ������] %s\r\n", buf);

			// ���� �Ϸ� �˸���
			// ���� ���α׷� ���ۿ� �����͸� ���������� �˸��� ���� hWriteEvent �� ��ȣ ���·� �����
			SetEvent(hWriteEvent);
			// ��Ŀ�� �Լ�
			SetFocus(hEdit1);

			// ���� ���� ��Ʈ�ѿ� �Էµ� ���ڿ��� ��� ���� ���°� �ǰ� �Ѵ�
			// SendMessage(��Ʈ��ID, EM_SETSEL, 0, -1) �������� ȣ���ϸ� �ȴ�
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

// ���� ��Ʈ�� ��� �Լ�
void DisplayText(const char* fmt, ...)
{
	va_list arg;
	va_start(arg, fmt);

	// ������ �ִ� ������ ������ ���� ũ�⸦ �����ߴ�
	char cbuf[BUFSIZE + 256];
	vsprintf(cbuf, fmt, arg);

	int nLength = GetWindowTextLength(hEdit2);
	SendMessage(hEdit2, EM_SETSEL, nLength, nLength);
	SendMessage(hEdit2, EM_REPLACESEL, FALSE, (LPARAM)cbuf);

	va_end(arg);
}

// ���� ��� �Լ��� printf() ��� DisplayText() �� ����ϵ��� �����ߴ�
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

// ����� ���� ������ ���� �Լ�
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

// TCP Ŭ���̾�Ʈ ���� �κ�
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
		printf("����\n");
		// ���� �Ϸ� ��ٸ���
		// hWriteEvent �� ��ȣ ���°� �Ǳ⸦ ��ٸ���
		WaitForSingleObject(hWriteEvent, INFINITE);

		if (strlen(buf) == 0)
		{
			// <������> ��ư�� Ȱ��ȭ�� �� hReadEvent �� ��ȣ ���·� �����
			// ������ ��ư Ȱ��ȭ
			EnableWindow(hSendButton, TRUE);
			// �б� �Ϸ� �˸���
			SetEvent(hReadEvent);
			continue;
		}

		retval = send(sock, buf, strlen(buf), 0);
		if (retval == SOCKET_ERROR)
		{
			err_display("send()");
			break;
		}
		DisplayText("[TCP Ŭ���̾�Ʈ] %d����Ʈ�� ���½��ϴ�\r\n", retval);

		retval = recvn(sock, buf, retval, 0);
		if (retval == SOCKET_ERROR)
		{
			err_display("recv()");
			break;
		}
		else if (retval == 0)
			break;

		buf[retval] = '\0';
		DisplayText("[TCP Ŭ���̾�Ʈ] %d����Ʈ�� �޾ҽ��ϴ�\r\n", retval);
		DisplayText("[���� ������] %s\r\n", buf);

		// <������> ��ư�� Ȱ��ȭ�� �� hReadEvent �� ��ȣ ���·� �����
		// ������ ��ư Ȱ��ȭ
		EnableWindow(hSendButton, TRUE);
		// �б� �Ϸ� �˸���
		SetEvent(hReadEvent);
	}

	return 0;
}