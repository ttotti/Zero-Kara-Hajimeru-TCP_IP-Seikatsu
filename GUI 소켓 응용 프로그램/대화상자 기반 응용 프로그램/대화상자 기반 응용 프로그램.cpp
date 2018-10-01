#include <Windows.h>
#include <stdio.h>
#include "resource.h"

// 위쪽 편집 컨트롤에 입력할 수 있는 글자의 최대 개수다(영 숫자 기준)
#define BUFSIZE 25

// 대화상자 프로시저
BOOL CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);
// 편집 컨트롤 출력 함수
void DisplayText(const char* fmt, ...);

// 편집 컨트롤
// 두 편집 컨트롤에 언제든지 접근할 수 있도록 행들 값을 저장할 전역 변수를 선언했다
HWND hEdit1, hEdit2;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgProc);
	return 0;
}

// 대화상자 프로시저
BOOL CALLBACK DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// 위쪽 편집 컨트롤에 입력한 문자열을 저장하기 위한 버퍼다 널('\0') 문자를 감안해 크기를 +1 했다
	static char buf[BUFSIZE + 1];

	switch (uMsg)
	{
	case WM_INITDIALOG:
		// 두 편집 컨트롤에 언제든지 접근할 수 있도록 전역 변수에 핸들 값을 저장해둔다
		// GetDlgItem() 을 호출하면 컨트롤의 핸들 값을 얻을 수 있다
		hEdit1 = GetDlgItem(hDlg, IDC_EDIT1);
		hEdit2 = GetDlgItem(hDlg, IDC_EDIT2);
		// 위쪽 편집 컨트롤에 입력할 수 있는 글자의 최대 개수를 설정한다
		// SendMessage(편집 컨트롤ID, EM_SETLIMITTEXT, BUFSIZE, 0) 을 호출하면 된다
		SendMessage(hEdit1, EM_SETLIMITTEXT, BUFSIZE, 0);
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			// 위쪽 편집 컨트롤에 입력된 문자열을 얻어서 아래쪽 편집 컨트롤에 출력한다
			// GetDlgItemText(대화상자 핸들, 컨트롤 ID, 버퍼 시작 주소, 버퍼 길이) 형식으로 호출하면
			// 편집 컨트롤에 입력된 문자열열 버퍼에 저장할 수 있다
			GetDlgItemText(hDlg, IDC_EDIT1, buf, BUFSIZE + 1);
			DisplayText("%s\r\n", buf);
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

// 출력 함수
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