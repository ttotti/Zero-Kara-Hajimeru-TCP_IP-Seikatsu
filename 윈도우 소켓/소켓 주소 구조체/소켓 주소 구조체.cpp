

// ------------------------------------------------------//
// HelloNetwork.cpp 소스									 //
//                                                       //
// 이 프로젝트는 문자집합 설정이 멀티바이트 문자 집합 입니다 //
// ------------------------------------------------------//

#pragma comment(lib,"ws2_32")
#include <WinSock2.h>

// IP 주소를 저장하기 위한 in_addr과 in6_addr 구조체는 inaddr.h(WinSock2.h 포함) 와 in6addr.h(WS2tcpip.h에 포함) 에 다음과 같이 정의되어 있다

// IPv4 주소를 담는 in_addr 구조체는 동일 메모리 영역을 각각
// 8비트(S_un_b), 16비트(S_un_w), 32비트(S_addr) 단위로 접근할 수 있게 만든 공용체(S_un)임을 알 수 있다
// 응용 프로그램에서는 대개 32비트 단위로 접근하므로 S_un, S_addr 필드를 사용하며, 매크로를 통해 재정의된 s_addr을 사용하면 편리하다

/*typedef struct in_addr
{
	union
	{
		struct { u_char s_b1, s_b2, s_b3, sb_4; } S_un_b;
		struct { u_short s_w1, s_w2; } S_un_W;
		u_long S_addr;
  	} S_un;
  
#define s_addr S_un.S_addr
} IN_ADDR; */

// in_addr 구조체와 달리 IPv6 주소를 담는 in6_addr 구조체는 단순한 바이트 또는 위더 배열로 정의되어 있다

/*typedef struct in6_addr
{
	union
	{
		u_char Byte[16];
		u_short Word[8];
	} u;
}IN6_ADDR; */

// SOCKADDR_IN 구조체를 제외한 나머지 소켓 주소 구조체는 크기가 SOCKADDR 구조체와 다름을 알 수 있다
// IrDA 프로토콜에 사용하는 SOCKADDR_IRDA 구조체와
// 블루투스 프로토콜에 사용하는 SOCKADDR_BTH 구조체도 같이 표시했다

/* 소켓 주소 구조체 크기 비교 (바이트 단위)
   SOCKADDR      16
   SOCKADDR_IN   16
   SOCKADDR_IN6  28
   SOCKADDR_IRDA 32
   SOCKADDR_BTH  30 */

// 응용 프로그램에서 소켓 주소 구조체를 사용하는 상황은 다음 두 가지로 나눌 수 있다

// 1. 응용 프로그램이 소켓 주소 구조체를 초기화 하고 소켓 함수에 넘겨주는 경우 (SockerFunc()는 임의의 소켓 함수를 나타냄)
/*
	소켓 주소 구조체를 초기화 한다
	SOCKADDR_IN addr;
	SocketFunc(..., (SOCKADDR*)&addr, sizeof(addr), ...);
*/

// 2. 소켓 함수가 소켓 주소 구조체를 입력으로 받아 내용을 채우면, 응용 프로그램이 이를 출력 등의 목적으로 사용하는 경우 (SockerFunc()는 임의의 소켓 함수를 나타냄)
/*
	 SOCKADDR_IN addr;
	 SocketFunc(..., (SOCKADDR*)&addr, sizeof(addr), ...);

	 소켓 주소 구조체를 사용한다
	 ...
*/

// 소켓 주소 구조체를 사용할때 주의할 사항이 있다
// 소켓 주소 구조체는 크기가 크기 때문에 소켓 함수 인자로 전달할 때는 항상 주소 값을 사용하며,
// 반드시 SOCKADDR 포인터 형으로 변환해야 한다
// 또한 사용할 프로토콜에 따라 소켓 주소 구조체의 크기가 달라지므로 sizeof 연산자를 사용해 얻은 크기 정보를 같이 전달해야 한다

#include <WS2tcpip.h>

#include <stdlib.h>
#include <stdio.h>

#define SERVERPORT 9000
#define BUFSIZE 512

// 소켓 함수 오류 출력 후 종료
void err_quit(const char* msg)
{
	LPVOID lpMsgBuf;

	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);

	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);

	LocalFree(lpMsgBuf);
	exit(1);
}

// 소켓 함수 오류 출력
void err_display(const char* msg)
{
	LPVOID lpMsgBuf;

	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);

	printf("[%s] %s", msg, (char*)lpMsgBuf);

	LocalFree(lpMsgBuf);
}


// 소켓 주소 구조체는 네트워크 프로그램에서 필요한 주소 정보를 담고 있는 구조체로 다양한 소켓 함수의 인자로 사용한다
// 프로토콜 체계에 따라 주소 지정 방식이 다르므로 다양한 소켓 주소 구조체가 존재한다
// 기본이 되는 것은 SOCKADDR 구조체로 winsock2.h 파일에 다음과 같이 정의되어 있다

/* typedef struct sockaddr
   {
 	 u_short sa_family;
	 char sa_data[14];
   } SOCKADDR; */

// 추가 u_short = unsigned short

// sa_family : 주소 체계를 나타내는 16비트 정수 값이다 예) TCP/IP 프로토콜 - AF_INET 또는 AF_INET6
// sa_data[14] : 해당 주소 체계에서 사용할 주소 정보를 담는다 예) TCP/IP 프로토콜을 사용한다면 IP 주소와 포트 번호가 저장된다

// 실제 프로그래밍에서는 응용 프로그램이 사용할 프로토콜의 종류에 맞는 별도의 소켓 주소 구조체를 사용한다
// 예를 들면 TCP/IP 에서는 SOCKADDR_IN 또는 SOCKADDR_IN6
// IrDa(적외선 무선 통신 프로토콜) 에서는 SOCKADDR_IRDA 를 사용한다

// TCP 서버(IPv4)
DWORD WINAPI TCPServer4(LPVOID arg)
{
	int retval;

	// socket() TCP(IPv4) 소켓 생성
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET)err_quit("socket()");

	// bind()
	/* IPv4 소켓 주소 구조체*/

	/*typedef struct sockaddr_in
	{
		short          sin_fimily;
		u_short        sin_port;
		struct in_addr sin_addr;
		char           sin_zero[8]; // (0으로 설정)
	} SOCKDDR_IN;
	*/

	// 자주 사용하는 구조체 필드
	// sin_family : 주소 체계를 의미하며, AF_INET 값을 사용한다
	// sin_port : 포트 번호를 의미하며, 부호 없는 16비트 정수 값을 사용한다
	// sin_addr : IP 주소를 의미하며, 32비트 in_addr 구조체를 사용한다

	SOCKADDR_IN serveraddr;
	// ZeroMesmory(Destination(대상), Length(길이)) - 한 블록의 메모리를 0으로 채웁니다
	ZeroMemory(&serveraddr, sizeof(serveraddr));

	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);

	// 응용 프로그램에서 소켓 주소 구조체를 사용하는 상황 (위의 주석에 주의사항이 적혀있다)
	retval = bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR)err_quit("bind()");

	// listen()
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) err_quit("listen()");

	// 데이터 통신에 사용할 변수
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;
	char buf[BUFSIZE + 1];

	while (1)
	{
		// accept()
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);

		if (client_sock == INVALID_SOCKET)
		{
			err_display("accept()");
			break;
		}

		// 접속한 클라이언트 정보 출력
#pragma warning(suppress : 4996)
		printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

		// 클라이언트와 데이터 통신
		while (1)
		{
			// 데이터 받기
			retval = recv(client_sock, buf, BUFSIZE, 0);

			if (retval == SOCKET_ERROR)
			{
				err_display("recv()");
				break;
			}
			else if (retval == 0)
				break;

			// 받은 데이터 출력
			buf[retval] = '\0';
			printf("%s", buf);
		}

		// closesocket()
		closesocket(client_sock);

#pragma warning(suppress : 4996)
		printf("\n[TCP 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
	}

	// closesocket()
	closesocket(listen_sock);

	return 0;
}

// TCP 서버(IPv6)
DWORD WINAPI TCPServer6(LPVOID arg)
{
	int retval;

	// socket()
	SOCKET listen_sock = socket(AF_INET6, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET) err_quit("socket()");

	// bind()
	/* IPv6 소켓 주소 구조체*/

	/*typedef struct sockaddr_in6
	{
		short           sin6_fimily;
		u_short         sin6_port;
		u_long          sin6_flowinfo  // 대부분 0으로 설정
		struct in6_addr sin6_addr;
		u_long          sin6_scope_id // 대부분 0으로 설정
	} SOCKDDR_IN6;
	*/

	// 자주 사용하는 구조체 필드
	// sin6_family : 주소 체계를 의미하며, AF_INET6 값을 사용한다
	// sin6_port : 포트 번호를 의미하며, 부호 없는 16비트 정수 값을 사용한다
	// sin6_addr : IP 주소를 의미하며, 128비트 in6_addr 구조체를 사용한다

	SOCKADDR_IN6 serveraddr;
	// ZeroMesmory(Destination(대상), Length(길이)) - 한 블록의 메모리를 0으로 채웁니다
	ZeroMemory(&serveraddr, sizeof(serveraddr));

	serveraddr.sin6_family = AF_INET6;
	serveraddr.sin6_addr = in6addr_any;
	serveraddr.sin6_port = htons(SERVERPORT);

	// 응용 프로그램에서 소켓 주소 구조체를 사용하는 상황 (위의 주석에 주의사항이 적혀있다)
	retval = bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("bind()");

	// listen()
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) err_quit("listen()");

	// 데이터 통신에 사용할 변수
	SOCKET client_sock;
	SOCKADDR_IN6 clientaddr;
	int addrlen;
	char buf[BUFSIZE + 1];

	while (1)
	{
		// accept()
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET)
		{
			err_display("accept()");
			break;
		}

		// 접속한 클라이언트 정보 출력
		char ipaddr[50];
		DWORD ipaddrlen = sizeof(ipaddr);
#pragma warning(suppress : 4996)
		WSAAddressToString((SOCKADDR*)&clientaddr, sizeof(clientaddr), NULL, ipaddr, &ipaddrlen);
		printf("\n[TCP 서버] 클라이언트 접속: %s\n", ipaddr);

		// 클라이언트와 데이터 통신
		while (1)
		{
			// 데이터 받기
			retval = recv(client_sock, buf, BUFSIZE, 0);
			if (retval == SOCKET_ERROR)
			{
				err_display("recv()");
				break;
			}
			else if (retval == 0)
				break;

			// 받은 데이터 출력
			buf[retval] = '\0';
			printf("%s", buf);
		}

		// closesocket()
		closesocket(client_sock);
		printf("\n[TCP 서버] 클라이언트 종료: %s\n", ipaddr);
	}

	// closesocket
	closesocket(listen_sock);

	return 0;
}

int main(int argc, char* argv[])
{
	// 윈속 초기화
	WSADATA wsa;

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	HANDLE hThread[2];
	hThread[0] = CreateThread(NULL, 0, TCPServer4, NULL, 0, NULL);
	hThread[1] = CreateThread(NULL, 0, TCPServer6, NULL, 0, NULL);

	WaitForMultipleObjects(2, hThread, TRUE, INFINITE);

	// 윈속 종료
	WSACleanup();

	return 0;
}