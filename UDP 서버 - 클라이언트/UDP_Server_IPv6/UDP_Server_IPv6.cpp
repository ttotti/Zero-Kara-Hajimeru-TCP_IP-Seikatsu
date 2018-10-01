

// UDP 서버 - 클라이언트(IPv6)

// 기존 IPv4 코드를 IPv6 코드로 변환하는 규칙을 다시 소개하면 다음과 같다
/*
	1. ws2tcpip.h 헤더 파일을 포함한다

	2. 소켓 생성 시 AF_INET 대신 AF_INET6를 사용한다

	3. 소켓 주소 구조체로 SOCKADDR_IN 대신 SOCKADDR_IN6를 사용한다
		- 구조체를 변경하면 구조체 필드명도 구에 따라 바꿔야 한다
		- 서버에서 주로 사용하는 INADDR_ANY(0으로 정의됨) 값은 in6addr_any(역시 0으로 정의됨 : 소문자임에 주의!)로 변경한다

	4. IPv4만을 지원하는 주소 변환 함수를 IPv4 / IPv6 지원 함수로 대체한다
		- 예를 들면, inet_ntoa() 함수는 WSAAddressToString() 으로, inet_addr() 함수로 WSAStringToAddress()로 변경한다

	5. 데이터 전송 함수는 기존의 sendto() / recvfrom() 함수를 변경 없이 그대로 사용한다
*/

#pragma comment(lib,"ws2_32")
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#define SERVERPORT 9000
#define BUFSIZE 512

void err_quit(const char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

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

	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	SOCKET sock = socket(AF_INET6, SOCK_DGRAM, 0);
	if (sock == INVALID_SOCKET)
		err_quit(TEXT("socket()"));

	SOCKADDR_IN6 serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin6_family = AF_INET6;
	serveraddr.sin6_addr = in6addr_any;
	serveraddr.sin6_port = htons(SERVERPORT);
	retval = bind(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR)
		err_quit(TEXT("bind()"));

	SOCKADDR_IN6 clientaddr;
	int addrlen;
	char buf[BUFSIZE + 1];

	while (1)
	{
		addrlen = sizeof(clientaddr);
		retval = recvfrom(sock, buf, BUFSIZE, 0, (SOCKADDR*)&clientaddr, &addrlen);
		if (retval == SOCKET_ERROR)
		{
			err_display(TEXT("recvfrom()"));
			continue;
		}

		char ipaddr[50];
		DWORD ipaddrlen = sizeof(ipaddr);
		WSAAddressToString((SOCKADDR*)&clientaddr, sizeof(clientaddr), NULL, ipaddr, &ipaddrlen);

		buf[retval] = '\0';
		printf("[UDP/%s] %s\n", ipaddr, buf);

		retval = sendto(sock, buf, retval, 0, (SOCKADDR*)&clientaddr, sizeof(clientaddr));
		if (retval == SOCKET_ERROR)
		{
			err_display(TEXT("sendto()"));
			continue;
		}
	}

	closesocket(sock);

	WSACleanup();

	return 0;
}