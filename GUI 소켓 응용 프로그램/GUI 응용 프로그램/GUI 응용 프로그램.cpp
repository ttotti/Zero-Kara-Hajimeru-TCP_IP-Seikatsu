#include <Windows.h>
// ���� ���� ���ڸ� ó���ϱ� ���� va_listŸ�԰�, va_start(), va_end() �Լ��� ����ϴµ�, �̸� ���� �ʿ��� ��� �����̴�
#include <stdio.h>

// ������ ���ν���
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// ���� ��Ʈ�� ��� �Լ�
void DisplayText(const char* fmt, ...);

// �ν��Ͻ� �ڵ�
HINSTANCE hInst;
// ���� ��Ʈ��
HWND hEdit;

// ���� �Լ�
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// �ν��Ͻ� ����� ���� ������ �����Ѵ�
	hInst = hInstance;

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

	MSG msg;
	while (GetMessage(&msg, 0, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

// ������ ���ν���
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
		// ���� ��Ʈ���� ���� �������� �ڽ� ������� ������ �� DisplayText() �Լ��� ����� ���� ��Ʈ�ѿ� ���ڿ��� ����ϰ� �ִ�
		hEdit = CreateWindow("edit", NULL, WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL | ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_MULTILINE, 0, 0, 0, 0, hWnd, (HMENU)100, hInst, NULL);

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

	char cbuf[256];
	vsprintf(cbuf, fmt, arg);

	// ���� ��Ʈ�ѿ� ���ڿ��� �߰��ϴ� �ڵ��̴�
	// GetWindowTextLength() �Լ��� ���� ��Ʈ�ѿ� ��� �ִ� ���ڿ��� ���̸� ���� ��
	// EM_SETSEL / EM_REPLACESEL �޽����� �̿��� �� ���� ���ο� ���ڿ��� �߰��Ѵ�
	int nLength = GetWindowTextLength(hEdit);
	SendMessage(hEdit, EM_SETSEL, nLength, nLength);
	SendMessage(hEdit, EM_REPLACESEL, FALSE, (LPARAM)cbuf);

	va_end(arg);
}
