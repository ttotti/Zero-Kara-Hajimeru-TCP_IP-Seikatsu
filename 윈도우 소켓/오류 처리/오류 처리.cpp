#include <WinSock2.h>

// 네트워크 프로그램에서는 여러 원인 때문에 오류가 발생할 수 있고 비교적 발생 확률이 높다
// 따라서 오류를 체크하여 구체적인 오류내용을 알려주는 것이 매우 중요하다

// 오류 처리 방법에는 세 가지 유형이 있는데
// 1.오류를 처리할 필요가 없는 경우 : 리턴 값이 없거나 호출 시 항상 성공하는 일부 소켓 함수
// 2.리턴 값만으로 오류를 처리하는 경우 : WSAStartup()함수
// 3.리턴 값으로 오류 발생을 확인하고, 구체적인 내용은 오류 코드로 확인하는 경우 : 대부분의 소켓 함수

// 1,2번 유형은 특별한 경우이다
// 여기서는 3번 유형을 처리하는 방법을 공부한다
// 소켓 함수 호출 결과 오류가 발생했다면 다음과 같은 함수를 사용해 오류 코드를 얻을 수 있다
int WSAGetLastError(void);

// 사용예)
if (소켓 함수(...) == 실패)
{
	int errcode = WSAGetLastError();
	printf(errcode에 해당하는 오류 메시지);
}

// WSAGetLastError() 함수의 리턴 값을 화면에 그대로 표시랄 경우 사용가자 직접 오류 코드의 의미를 알아내야 하는 불편이 따른다
// 따라서 해당 오류 코드를 적절한 문자열 형태로 출력하는 것이 바람직하다

// FormatMessage() 함수를 사용하면 오류 코드에 대응하는 오류 메시지를 얻을 수 있다 (이 함수의 자세한 내용은 MSDN 설명서를 참고한다)
// DWORD FormatMessage( 1.DWORD dwFlags, LPCVOID lpSource, 2.DWORD dwMessageId, 3.DWORD dwLanguageId, 4.LPTSTR lpBuffer, DWORD nSize, va_list* Arguments)

// 1. dwFlage : FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM 값을 사용한다
// FORMAT_MESSAGE_ALLOCATE_BUFFER 는 오류 메시지를 저장할 공간을 FormatMessage() 함수가 알아서 할당한다는 의미이고
// FORMAT_MESSAGE_FROM_SYSTEM은 운영체제로 부터 오류 메시지를 가져온다는 의미다

// 2. dwMessageId : 오류 코드를 나타내며 WSAGetLastError() 함수의 리턴 값을 여기에 넣는다

// 3. dwLanguageId : 오류 메시지를 표시할 언어를 나타내며, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT)를 사용하면 사용자가 제어판에서 설정한 기본언어로 오류 메시지를 얻을 수 있다

// 4. lpBuffer : 오류 메시지의 시작 주소가 여기에 저장된다, 오류 메시지를 저장할 공간은 FormatMessage() 가 알아서 할당하므로 사용자는 주소 값을 저장할 변수를
// 여기에 넣어주면 된다, 오류 메시지 사용을 마치면 LocalFree() 함수를 사용해 시스템이 할당한 메모리를 반환해야 한다는 점에 주의하자 

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// err_quit() 함수는 msg인자로 전달된 문자열과 더불어 현재 발생한 오류 메시지를 화면에 메시지 상자로 표시하고, 응용 프로그램을 종료하는 역할을 한다
// 오류가 발생하면 메시지 상자가 화면에 표시되고 확인버튼을 누르면 응용프로그램이 종료된다
void err_quit(LPCWSTR msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

// 사용예)
if (socket(...) == INVALID_SOCKET) err_quit(TEXT("socket()"));
if (bind(...) == SOCKET_ERROR)err_quit(TEXT("bind()"));

// 사소한 오류가 발생할 때마다 종료하는 일은 바람직 하지 않다 또한 메시지 박스가 뜨고 확인을 누르고 응용프로그램이 종료되는 것은 불편하다
// 이 경우에는  메시지 박스 대신 printf() 함수를 넣어 도스창에 오류를 출력하게 하고 응용프로그램이 종료되지 않는다
void err_display(char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (char*)lpMsgBuf);
	LocalFree(lpMsgBuf);
}