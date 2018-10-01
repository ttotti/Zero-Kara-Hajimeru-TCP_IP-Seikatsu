#pragma comment(lib,"ws2_32")
#include <WinSock2.h>

#include <stdio.h>

// 모든 윈속 응용 프로그램은 다음과 같은 공통 구조를 가지고 있다
// 윈속초기화 -> 소켓 생성 -> 네트워크 통신 -> 소켓 닫기 -> 윈속 종료

int main(int argc, char* argv[])
{
	// 모든 윈속 프로그램은 소켓 함수를 호출하기 전에 반드시 윈속 초기화 함수인 WSAStartup() 을 호출해야 한다
	// WSAStartup() 함수는 프로그램에서 사용할 윈속 버전을 요청함으로써 윈속 라이브러리(WS2_32.DLL)를 초기화하는 역할을 한다
	// WSAStartup() 함수가 실패할 경우 WS2_32.DLL이 메모리에 로드되지 않는다
	// 이 경우에는 WSAGetLastError() 함수가 리턴하는 오류 코드는 부정확하므로 WSAStartup() 함수는 직접 오류 코드를 리턴하도록 설계되어 있다

	// int WSAStartup(WORD wVersionRequested, LPWSADATA lpWSAFata);  (성공 : 0, 실패 : 오류 코드)

	// wVersionRequested : 프로그램이 요구하는 최상위 윈속 버전이다 하위 8비트에 주(major)버전, 상위 8비트에 부(minor)버전을 넣어서 전달한다
	// 예를 들어 윈속3.2 버전 사용을 요청한다면 0x0203 또는 MAKEWORD(3,2)를 사용한다

	// lpWSAData : WSADATA 구조체를 전달하면 이를 통해 윈도우 운영체제가 제공하는 윈속 구현에 관한 정보를 얻을 수 있다
	// (응용 프로그램이 실제로 사용하게 될 윈속 버전, 시스템이 지원하는 윈속 최상위 버전 등)
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// WSADATA 구조체의 변수필드 처음 네 개 내용 출력 (윈속 2.2버전과 1.1버전이 출력되는 내용이 다르다)
	printf("%d / %d / %s / %s", wsa.wVersion, wsa.wHighVersion, wsa.szDescription, wsa.szSystemStatus);

	MessageBox(NULL, TEXT("윈속 초기화 성공"), TEXT("알림"), MB_OK);
	
	// 프로그램을 종료할 때는 윈속 종료 함수인 WSACleanup()을 호출해아 한다
	// WSACleanup() 함수는윈속 사용을 중지함을 운영체제에 알리고, 관련 리소스를 반환하는 역할을 한다
	// 함수 호출이 실패할 경우 WSAGetLastError() 함수를 호출함으로써 구체적인 오류 코드를 얻을 수 있다

	// int WSACleanup(void);  (성공 : 0, 실패 : SOCKET_ERROR)
	WSACleanup();

	return 0;
}