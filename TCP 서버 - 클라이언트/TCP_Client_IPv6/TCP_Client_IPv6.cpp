
/* 기존 IPv4 코드를 IPv6 코드로 변환하는 규칙은 다음과 같다
   1. ws2tcpip.h 헤더 파일을 포함한다
   2. 소켓 생성 시 AF_INET 대신 AF_INET6를 사용한다
   3. 소켓 주소 구조체로 SOCKADDR_IN 대신 SOCKADDR_IN6를 사용ㅎ나다
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

// IPv6 버전의 IP를 사용한다
#define SERVERIP "::1"
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

// 사용자 정의 데이터 수신 함수
int recvn(SOCKET s, char* buf, int len, int flags)
{
	int received;
	char* ptr = buf;
	int left = len;

	while (left>0)
	{
		// recv()
		received = recv(s, ptr, left, flags);
		if (received == SOCKET_ERROR)
			return SOCKET_ERROR;
		else if (received == 0)
			break;

		left -= received;
		ptr += received;
	}

	return (len - left);
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
	SOCKET sock = socket(AF_INET6, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
		err_quit(TEXT("socket()"));

	// connect()
	// SOCKADDR_IN -> SOCKADDR_IN6
	SOCKADDR_IN6 serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	// serveraddr.sin_family = AF_INET -> serveraddr.sin6_family - AF_INET6
	serveraddr.sin6_family = AF_INET6;
	// serveraddr.sin_addr.s_addr = inet_addr(SERVERIP); 을 대신해서
	// WSAStringToAddress() 함수를 사용한다
	int addrlen = sizeof(serveraddr);
	WSAStringToAddress((LPSTR)SERVERIP, AF_INET6, NULL, (SOCKADDR*)&serveraddr, &addrlen);
	// serveraddr.sin_port -> serveraddr.sin6_port
	serveraddr.sin6_port = htons(SERVERPORT);
	retval = connect(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR)
		err_quit(TEXT("connect()"));

	// 데이터 통신에 사용할 변수
	char buf[BUFSIZE + 1];
	int len;

	// 서버와 데이터 통신
	while (1)
	{
		// 데이터 입력
		printf("\n[보낼 데이터] ");
		if (fgets(buf, BUFSIZE + 1, stdin) == NULL)
			break;

		// '\n' 문자 제거
		len = strlen(buf);
		if (buf[len - 1] == '\n')
			buf[len - 1] = '\0';
		if (strlen(buf) == 0)
			break;

		// 데이터 보내기
		// send()
		retval = send(sock, buf, strlen(buf), 0);
		if (retval == SOCKET_ERROR)
		{
			err_display(TEXT("send()"));
			break;
		}
		printf("[TCP 클라이언트] %d바이트를 보냈습니다.\n", retval);

		// 데이터 받기
		retval = recvn(sock, buf, retval, 0);
		if (retval == SOCKET_ERROR)
		{
			err_display(TEXT("recv()"));
			break;
		}
		else if (retval == 0)
			break;

		// 받은 데이터 출력
		buf[retval] = '\0';
		printf("[TCP 클라이언트] %d바이트를 받았습니다.\n", retval);
		printf("[받은 데이터] %s\n", buf);
	}

	closesocket(sock);

	// 윈속 종료
	WSACleanup();
	return 0;
}