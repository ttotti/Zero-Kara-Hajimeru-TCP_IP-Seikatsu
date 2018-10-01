#pragma comment(lib,"ws2_32")
#include <WinSock2.h>
#include <stdlib.h>
#include <Windows.h>
// ���� ���� ���ڸ� ó���ϱ� ���� va_listŸ�԰�, va_start(), va_end() �Լ��� ����ϴµ�, �̸� ���� �ʿ��� ��� �����̴�
#include <stdio.h>

#define SERVERPORT 9000
#define BUFSIZE 512

// ������ ���ν���
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// ���� ��Ʈ�� ��� �Լ�
void DisplayText(const char* fmt, ...);

// ���� ��� �Լ�
void err_quit(const char* msg);
void err_display(const char* msg);

// ���� ��� ������ �Լ�
DWORD WINAPI ServerMain(LPVOID arg);
DWORD WINAPI ProcessClient(LPVOID arg);

// �ν��Ͻ� �ڵ�
HINSTANCE hInst;
// ���� ��Ʈ��
HWND hEdit;

// �Ӱ� ����
// ������ �� �̻��� ���� ��Ʈ�ѿ� ������ ����ϹǷ�, �����Ͱ� �ڼ��̴� ��Ȳ�� �����ϱ� ���� ���̴�
CRITICAL_SECTION cs;

// ���� �Լ�
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// �ν��Ͻ� ����� ���� ������ �����Ѵ�
	hInst = hInstance;

	// �Ӱ� ���� ������ �ʱ�ȭ�� ���� �۾��� ����Ѵ�
	InitializeCriticalSection(&cs);

	// ������ Ŭ���� ���, ������ ����, �޽��� ���� �κ�
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

	// ���� ��� ������ ����
	// �޽��� ������ �����ϱ� ���� TCP ������ ���� �ڵ带 ������ ������� �����Ѵ�
	CreateThread(NULL, 0, ServerMain, NULL, 0, NULL);

	// �޽��� ����
	MSG msg;
	while (GetMessage(&msg, 0, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	// �Ӱ� ������ ����
	DeleteCriticalSection(&cs);

	return msg.wParam;
}

// ������ ���ν���
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
		// ���� ��Ʈ���� ���� �������� �ڽ� ������� ������ �� DisplayText() �Լ��� ����� ���� ��Ʈ�ѿ� ���ڿ��� ����ϰ� �ִ�
		// ���� ��Ʈ�� ���� �� ES_READONLY ��Ÿ���� �־� �б⸸ �����ϰ� �Ѵ�, �̴� ���� ��Ʈ���� ��� �������� ����Ѵٴ� �ǹ̴�
		hEdit = CreateWindow("edit", NULL, WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL | ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_MULTILINE | ES_READONLY, 0, 0, 0, 0, hWnd, (HMENU)100, hInst, NULL);

		DisplayText("������ GUI ���� ���α׷��Դϴ�.\r\n");
		DisplayText("�ν��Ͻ� �ڵ� ���� %#x�Դϴ�.\r\n", hInst);
		return 0;

		// WM_SIZE �޽����� ���� �������� ũ�Ⱑ ����� ������ �߻��Ѵ�
		// �̶� lParam�� ���� 16��Ʈ�� ���� 16��Ʈ���� ���� Ŭ���̾�Ʈ ������ ���� ���̰� ��� �ִ�
		// �� ������ �̿��� ���� ��Ʈ���� ���� �������� Ŭ���̾�Ʈ ���� ��ü�� �����ϵ��� ũ�⸦ �����ϰ� �ִ�
	case WM_SIZE:
		MoveWindow(hEdit, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
		return 0;

		// WM_SETFOCUS �޽����� ���� �����찡 Ű���� ��Ŀ���� ���� �� �߻��Ѵ�
		// �̴� ���� �����찡 Ű���� �޽����� ���� �� ������ �ǹ��Ѵ�
		// SetFocus() �Լ��� ȣ���ϸ� Ű���� ��Ŀ���� �ٸ� ������� ��ȯ�� �� �ִ�
		// ���⼭�� ���� ��Ʈ���� Ű���� ��Ŀ���� �����ϰ� �����ν� Ű���� �Է��� ���� �� �ְ� �Ѵ�
	case WM_SETFOCUS:
		SetFocus(hEdit);
		return 0;

		// WM_DESTROY �޽����� ���� �����츦 ���� �� �߻��ϹǷ� ���� û�� �۾�(���� �޸� ����, ���� �ݱ� ��)�� �����ϴ� �� �����ϴ�
		// PostQuitMessage() �Լ��� ���� ���α׷� �޽��� ť�� WM_QUIT �޽����� �������ν� GetMessage() �Լ��� 0�� �����ϰ� �Ѵ�
		// ���� �޽��� ������ ������ ���� �Լ��� �����ϹǷ� ���� ���α׷��� �����Ѵ�
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

// ��� �Լ�
void DisplayText(const char * fmt, ...)
{
	// va_list Ÿ�԰� va_start(), va_end() �Լ��� ���� ���� ���ڸ� �����ϱ� ���� ǥ�� C ���̺귯�� �Լ����� �����Ѵ�
	// ���� ��Ʈ�ѿ� ����� ���ڿ��� vsprintf() �Լ��� �̿��� cbuf[] �迭�� �����Ѵ�
	va_list arg;
	va_start(arg, fmt);

	// ���� ��Ʈ�ѿ� ����� ���ڿ� ������ ���� ���۴�
	// ������ ���� ���� ũ�⸦ ������ BUFSIZE + 256 ���� �����ߴ�
	char cbuf[BUFSIZE + 256];
	vsprintf(cbuf, fmt, arg);

	// ������ �� �̻��� ���� ��Ʈ�ѿ� ������ �� �����Ƿ� ����� �ڼ����� �ʵ��� �Ӱ� �������� ��ȣ�Ѵ�
	EnterCriticalSection(&cs);

	// ���� ��Ʈ�ѿ� ���ڿ��� �߰��ϴ� �ڵ��̴�
	// GetWindowTextLength() �Լ��� ���� ��Ʈ�ѿ� ��� �ִ� ���ڿ��� ���̸� ���� ��
	// EM_SETSEL / EM_REPLACESEL �޽����� �̿��� �� ���� ���ο� ���ڿ��� �߰��Ѵ�
	int nLength = GetWindowTextLength(hEdit);
	SendMessage(hEdit, EM_SETSEL, nLength, nLength);
	SendMessage(hEdit, EM_REPLACESEL, FALSE, (LPARAM)cbuf);

	// ������ �� �̻��� ���� ��Ʈ�ѿ� ������ �� �����Ƿ� ����� �ڼ����� �ʵ��� �Ӱ� �������� ��ȣ�Ѵ�
	LeaveCriticalSection(&cs);

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

// ���� ��� ������ �Լ�
// Ư���� �����ϰų� �Ѱ� ���� printf() ��� DisplayText() �� ����ߴ�
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

		DisplayText("\r\n[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\r\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

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

	DisplayText("[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\r\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

	return 0;
}
