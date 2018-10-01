#include <Windows.h>
// 가변 길이 인자를 처리하기 위해 va_list타입과, va_start(), va_end() 함수를 사용하는데, 이를 위해 필요한 헤더 파일이다
#include <stdio.h>

// 원도우 프로시저
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// 편집 컨트롤 출력 함수
void DisplayText(const char* fmt, ...);

// 인스턴스 핸들
HINSTANCE hInst;
// 편집 컨트롤
HWND hEdit;

// 메인 함수
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// 인스턴스 행들을 전역 변수에 저장한다
	hInst = hInstance;

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

	MSG msg;
	while (GetMessage(&msg, 0, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

// 윈도우 프로시저
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
		// 편집 컨트롤을 메인 윈도우의 자식 윈도우로 생성한 후 DisplayText() 함수를 사용해 편집 컨트롤에 문자열을 출력하고 있다
		hEdit = CreateWindow("edit", NULL, WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL | ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_MULTILINE, 0, 0, 0, 0, hWnd, (HMENU)100, hInst, NULL);

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

	char cbuf[256];
	vsprintf(cbuf, fmt, arg);

	// 편집 컨트롤에 문자열을 추가하는 코드이다
	// GetWindowTextLength() 함수로 편집 컨트롤에 들어 있는 문자열의 길이를 얻은 후
	// EM_SETSEL / EM_REPLACESEL 메시지를 이용해 맨 끝에 새로운 문자열을 추가한다
	int nLength = GetWindowTextLength(hEdit);
	SendMessage(hEdit, EM_SETSEL, nLength, nLength);
	SendMessage(hEdit, EM_REPLACESEL, FALSE, (LPARAM)cbuf);

	va_end(arg);
}
