#pragma comment(lib,"ws2_32")
#include <WinSock2.h>

// 오류 처리
void err_quit(LPCWSTR msg)
{
	LPVOID lpMsgBuf;

	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);

	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);

	LocalFree(lpMsgBuf);

	exit(1);
}

int main(int argc, char* argv[])
{
	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	MessageBox(NULL, TEXT("윈속 초기화 성공"), TEXT("알림"), MB_OK);

	// 소켓 생성하기
	// 소켓을 사용해 통신하기 위한 기본 요건은 통신 양단이 같은 프로토콜을 사용하는 것이다(TCP를 사용할 것인지 UDP를 사용할 것인지를 약속해야 한다)
	// socket()함수는 사용자가 요청한 프로토콜을 사용해 통신할 수 있도록 내부적으로 리소스를 할당하고 이에 접근할 수 있는 일종의 핸들 값(SOCKET 타입, 32비트 정수)을 리턴한다
	// 이 값을 소켓 디스크립터(socket descriptor)라 부르며 각동 소켓 함수를 호출할 때 인자로 전달해 사용한다

	// SOCKET socket(int af, int type, int protocol)  (성공 : 새로운 소켓, 실패 : INVALID_SOCKET)
	// af : 주소 체계를 지정한다
	// type : 소켓 타입을 지정한다
	// protocol : 사용할 프로토콜을 지정한다

	// 주소 체계 - 통신을 하려면 통신 상대를 유일하게 지정할 수 있는 주소가 필요한데 이러한 주소 지정 방법을 지칭하는 용어다
	// 주소 체계는 네트워크 프로토콜의 종류에 따라 달라지므로 주소 체계지정은 자신이 사용할 프로토콜을 선택하기 위한 첫 번째 관문이다
	// winsock2.h 또는 ws2def.h 파일을 찾아보면 AF_ 로 시작하는 상수를 찾을 수 있는데
	// 자신이 사용할 프로토콜에 대응하는 값을 선택해 socket() 함수의 첫 번째 인자로 전달하면 된다
	// 예를 들면 IPv4 기반 TCP나 UDP 프로토콜을 사용하려면 AF_INET 값을 선택한다 (IPv6 기반 - AF_INET6)

	// 소켓 타입 - 사용할 프로토콜의 특성을 나타내는 값이다
	// 자주 사용하는 소켓 타입은 다음과 같다
	// SOCK_STREAM - 신뢰성 있는 데이터 전송 기능 제공, 연결형 프로토콜 (TCP 와 같이 쓴다)
	// SOCK_DGRAM - 신뢰성 없는 데이터 전송 기능 제공, 비연결형 프로토콜 (UDP 와 같이 쓴다)
	// 소켓 타입은 네트워크 프로토콜의 종류에 따라 달라지므로 소켓 타입 지정은 자신이 사용할 프로토콜을 선택하기 위한 두 번째 관문이다

	// 프로토콜 - 주소 체계와 소켓 타입만으로 프로토콜을 결정할 수 있는 경우가 있다
	// 그러나 일반적으로는 주소 체계와 소켓 타입이 같아도 이에 해당하는 프로토콜이 두 개 이상 존재할 수 있다
	// 이때는 프로토콜을 명시적으로 지정해야 하는데 socket() 함수의 세 번째 인자가 이런 역할을 한다
	// IPPROTO_TCP - TCP와 같이 쓴다
	// IPPROTO_UDP - UDP와 같이 쓴다
	// 하지만 TCP나 UDP 프로토콜은 주소 체계와 소켓 타입만으로 프로토콜을 지정할 수 있으므로 대개는 프로토콜 부분에 0을 사용한다
	SOCKET tcp_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (tcp_sock == INVALID_SOCKET) 
		err_quit(TEXT("tcp_socket()"));

	MessageBox(NULL, TEXT("TCP 소켓 생성 성공"), TEXT("알림"), MB_OK);

	// UDP 소켓 생성
	// socket() 함수와 기본 역할은 같지만 좀 더 향상된 기능을 제공하는 WSASocket() 함수가 있다(자세한 내용은 인터넷 참고)
	SOCKET udp_sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (udp_sock == INVALID_SOCKET)
		err_quit(TEXT("udp_socket()"));

	MessageBox(NULL, TEXT("UDP 소켓 생성 성공"), TEXT("알림"), MB_OK);

	// 소켓을 사용한 통신을 마치면 관련 리소스를 반환해야 한다 closesocket()함수는 해당 소켓을 닫고 관련 리소스를 반환한다
	// int closesocket(SOCKET s)  (성공 : 0, 실패 : SOCKET_ERROR)
	closesocket(udp_sock);
	closesocket(tcp_sock);

	// 윈속 닫기
	WSACleanup();

	return 0;
}