
/* ���� IPv4 �ڵ带 IPv6 �ڵ�� ��ȯ�ϴ� ��Ģ�� ������ ����
1. ws2tcpip.h ��� ������ �����Ѵ�
2. ���� ���� �� AF_INET ��� AF_INET6�� ����Ѵ�
3. ���� �ּ� ����ü�� SOCKADDR_IN ��� SOCKADDR_IN6�� ����Ѵ�
- ����ü�� �����ϸ� ����ü �ʵ�� �׿� ���� �ٲ�� �Ѵ�
- �������� �ַ� ����ϴ� INADDR_ANY(0���� ���ǵ�) ���� in6addr_any(���� 0���� ���ǵ�; �ҹ����ӿ� ����)�� �����Ѵ�
4. IPv4���� �����ϴ� �ּ� ��ȯ �Լ��� IPv4/IPv6 ���� �Լ��� ��ü�Ѵ�
- ���� ���, inet_ntoa() �Լ��� WSAAddressToString() ����, inet_addr() �Լ��� WSAStringToAddress()�� �����Ѵ�
5. ������ ���� �Լ��� ������ send() / recv( )�Լ��� ���� ���� �״�� ����Ѵ�

���⼭ ������ ������ �� ��Ģ�� ���� ������ �ڵ�� IPv6���� �����Ѵٴ� ���̴�
��, ������ Ŭ���̾�Ʈ �ڵ带 ��� ������ ��쿡�� ��ȣ ����� �����ϴ�
(IPv4/IPv6 �� ���ÿ� �����ϴ� ����� ���� �ִ� (���߿� ����))*/

#pragma comment(lib,"ws2_32")
#include <WinSock2.h>

// IPv6 ������ �����ϱ� ���ؼ��� "ws2tcpip.h" ��� ������ �����ؾ� �Ѵ�
#include <WS2tcpip.h>

#include <stdlib.h>
#include <stdio.h>

#define SERVERPORT 9000
#define BUFSIZE 512

// ���� �Լ� ���� ��� �� ����
void err_quit(const char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

// ���� �Լ� ���� ���
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

	// ���� �ʱ�ȭ
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

	// ������ ��ſ� ����� ����
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

		// ������ Ŭ���̾�Ʈ ���� ���
		// inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port) �� ����ؼ�
		// WSAAddressToString() �Լ��� ����Ѵ�
		char ipaddr[50];
		DWORD ipaddrlen = sizeof(ipaddr);
		WSAAddressToString((SOCKADDR*)&clientaddr, sizeof(clientaddr), NULL, ipaddr, &ipaddrlen);
		printf("\n[TCP ����] Ŭ���̾�Ʈ ���� : %s\n", ipaddr);

		// Ŭ���̾�Ʈ�� ������ ���
		while (1)
		{
			// ������ �ޱ�
			// recv()
			retval = recv(client_sock, buf, BUFSIZE, 0);
			if (retval == SOCKET_ERROR)
			{
				err_display(TEXT("recv()"));
				break;
			}
			else if (retval == 0)
				break;

			// ���� ������ ���
			buf[retval] = '\0';
			// inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port) �� ����ؼ�
			// ������ ipaddr[] �ȿ� ������ ���ڿ��� ����Ѵ�
			printf("[TCP\%s] %s\n", ipaddr, buf);

			// ������ ������
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
		printf("[TCP ����] Ŭ���̾�Ʈ ����: %s\n", ipaddr);
	}

	// closesocket()
	closesocket(listen_sock);

	// ���� ����
	WSACleanup();

	return 0;
}