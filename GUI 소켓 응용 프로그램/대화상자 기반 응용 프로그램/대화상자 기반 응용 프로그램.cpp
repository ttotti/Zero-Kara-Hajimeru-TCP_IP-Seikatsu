#include <Windows.h>
#include <stdio.h>
#include "resource.h"

// ���� ���� ��Ʈ�ѿ� �Է��� �� �ִ� ������ �ִ� ������(�� ���� ����)
#define BUFSIZE 25

// ��ȭ���� ���ν���
BOOL CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);
// ���� ��Ʈ�� ��� �Լ�
void DisplayText(const char* fmt, ...);

// ���� ��Ʈ��
// �� ���� ��Ʈ�ѿ� �������� ������ �� �ֵ��� ��� ���� ������ ���� ������ �����ߴ�
HWND hEdit1, hEdit2;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgProc);
	return 0;
}

// ��ȭ���� ���ν���
BOOL CALLBACK DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// ���� ���� ��Ʈ�ѿ� �Է��� ���ڿ��� �����ϱ� ���� ���۴� ��('\0') ���ڸ� ������ ũ�⸦ +1 �ߴ�
	static char buf[BUFSIZE + 1];

	switch (uMsg)
	{
	case WM_INITDIALOG:
		// �� ���� ��Ʈ�ѿ� �������� ������ �� �ֵ��� ���� ������ �ڵ� ���� �����صд�
		// GetDlgItem() �� ȣ���ϸ� ��Ʈ���� �ڵ� ���� ���� �� �ִ�
		hEdit1 = GetDlgItem(hDlg, IDC_EDIT1);
		hEdit2 = GetDlgItem(hDlg, IDC_EDIT2);
		// ���� ���� ��Ʈ�ѿ� �Է��� �� �ִ� ������ �ִ� ������ �����Ѵ�
		// SendMessage(���� ��Ʈ��ID, EM_SETLIMITTEXT, BUFSIZE, 0) �� ȣ���ϸ� �ȴ�
		SendMessage(hEdit1, EM_SETLIMITTEXT, BUFSIZE, 0);
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			// ���� ���� ��Ʈ�ѿ� �Էµ� ���ڿ��� �� �Ʒ��� ���� ��Ʈ�ѿ� ����Ѵ�
			// GetDlgItemText(��ȭ���� �ڵ�, ��Ʈ�� ID, ���� ���� �ּ�, ���� ����) �������� ȣ���ϸ�
			// ���� ��Ʈ�ѿ� �Էµ� ���ڿ��� ���ۿ� ������ �� �ִ�
			GetDlgItemText(hDlg, IDC_EDIT1, buf, BUFSIZE + 1);
			DisplayText("%s\r\n", buf);
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

// ��� �Լ�
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