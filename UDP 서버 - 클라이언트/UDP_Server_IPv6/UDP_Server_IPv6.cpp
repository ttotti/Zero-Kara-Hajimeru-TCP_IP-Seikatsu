

// UDP ���� - Ŭ���̾�Ʈ(IPv6)

// ���� IPv4 �ڵ带 IPv6 �ڵ�� ��ȯ�ϴ� ��Ģ�� �ٽ� �Ұ��ϸ� ������ ����
/*
	1. ws2tcpip.h ��� ������ �����Ѵ�

	2. ���� ���� �� AF_INET ��� AF_INET6�� ����Ѵ�

	3. ���� �ּ� ����ü�� SOCKADDR_IN ��� SOCKADDR_IN6�� ����Ѵ�
		- ����ü�� �����ϸ� ����ü �ʵ�� ���� ���� �ٲ�� �Ѵ�
		- �������� �ַ� ����ϴ� INADDR_ANY(0���� ���ǵ�) ���� in6addr_any(���� 0���� ���ǵ� : �ҹ����ӿ� ����!)�� �����Ѵ�

	4. IPv4���� �����ϴ� �ּ� ��ȯ �Լ��� IPv4 / IPv6 ���� �Լ��� ��ü�Ѵ�
		- ���� ���, inet_ntoa() �Լ��� WSAAddressToString() ����, inet_addr() �Լ��� WSAStringToAddress()�� �����Ѵ�

	5. ������ ���� �Լ��� ������ sendto() / recvfrom() �Լ��� ���� ���� �״�� ����Ѵ�
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