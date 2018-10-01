
/* 기존 IPv4 코드를 IPv6 코드로 변환하는 규칙은 다음과 같다
1. ws2tcpip.h 헤더 파일을 포함한다
2. 소켓 생성 시 AF_INET 대신 AF_INET6를 사용한다
3. 소켓 주소 구조체로 SOCKADDR_IN 대신 SOCKADDR_IN6를 사용한다
- 구조체를 변경하면 구조체 필드명도 그에 따라 바꿔야 한다
- 서버에서 주로 사용하는 INADDR_ANY(0으로 정의됨) 값은 in6addr_any(역시 0으로 정의됨; 소문자임에 주의)로 변경한다
4. IPv4만을 지원하는 주소 변환 함수를 IPv4/IPv6 지원 함수로 대체한다
- 예를 들면, inet_ntoa() 함수는 WSAAddressToString() 으로, inet_addr() 함수는 WSAStringToAddress()로 변경한다
5. 데이터 전송 함수는 기존의 send() / recv( )함수를 변경 없이 그대로 사용한다

여기서 주의할 사항은 위 규칙을 따라 변경한 코드는 IPv6만을 지원한다는 점이다
즉, 서버와 클라이언트 코드를 모두 변경한 경우에만 상호 통신이 가능하다
(IPv4/IPv6 를 동시에 지원하는 방법은 따로 있다 (나중에 배운다))*/

#pragma comment(lib,"ws2_32")
#include <WinSock2.h>

// IPv6 버전을 지원하기 위해서는 "ws2tcpip.h" 헤더 파일을 포함해야 한다
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

int main(int argc, char* argv[])
{
	int retval;

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()

	// AF_INET -> AF_INET6
	SOCKET listen_sock = socket(AF_INET6, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET)
		err_quit(TEXT("socket()"));

	// bind()

	// SOCKET_IN -> SOCKET_IN6
	SOCKADDR_IN6 serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	//serveraddr.sin -> serveraddr.sin6
	// sin_family = AF_INET -> sin6_family = AF_INET6
	serveraddr.sin6_family = AF_INET6;
	// sin_addr.s_addr = htonl(INADDR_ANY) -> sin6_addr = in6addr_any
	serveraddr.sin6_addr = in6addr_any;
	// sin_port -> sin6_port
	serveraddr.sin6_port = htons(SERVERPORT);
	retval = bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR)
		err_quit(TEXT("bind()"));

	// listen()

	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR)
		err_quit(TEXT("listen()"));

	// 데이터 통신에 사용할 변수
	SOCKET client_sock;
	SOCKADDR_IN6 clientaddr;
	int addrlen;
	char buf[BUFSIZE + 1];

	while (1)
	{
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET)
		{
			err_display(TEXT("accept()"));
			break;
		}

		// 접속한 클라이언트 정보 출력
		// inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port) 을 대신해서
		// WSAAddressToString() 함수를 사용한다
		char ipaddr[50];
		DWORD ipaddrlen = sizeof(ipaddr);
		WSAAddressToString((SOCKADDR*)&clientaddr, sizeof(clientaddr), NULL, ipaddr, &ipaddrlen);
		printf("\n[TCP 서버] 클라이언트 접속 : %s\n", ipaddr);

		// 클라이언트와 데이터 통신
		while (1)
		{
			// 데이터 받기
			// recv()
			retval = recv(client_sock, buf, BUFSIZE, 0);
			if (retval == SOCKET_ERROR)
			{
				err_display(TEXT("recv()"));
				break;
			}
			else if (retval == 0)
				break;

			// 받은 데이터 출력
			buf[retval] = '\0';
			// inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port) 을 대신해서
			// 위에서 ipaddr[] 안에 저장한 문자열을 사용한다
			printf("[TCP\%s] %s\n", ipaddr, buf);

			// 데이터 보내기
			// send()
			retval = send(client_sock, buf, retval, 0);
			if (retval == SOCKET_ERROR)
			{
				err_display(TEXT("send()"));
				break;
			}

		}

		// closesocket()
		closesocket(client_sock);
		printf("[TCP 서버] 클라이언트 종료: %s\n", ipaddr);
	}

	// closesocket()
	closesocket(listen_sock);

	// 윈속 종료
	WSACleanup();

	return 0;
}