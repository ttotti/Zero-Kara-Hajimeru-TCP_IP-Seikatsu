
/* ���� IPv4 �ڵ带 IPv6 �ڵ�� ��ȯ�ϴ� ��Ģ�� ������ ����
   1. ws2tcpip.h ��� ������ �����Ѵ�
   2. ���� ���� �� AF_INET ��� AF_INET6�� ����Ѵ�
   3. ���� �ּ� ����ü�� SOCKADDR_IN ��� SOCKADDR_IN6�� ��뤾����
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

// IPv6 ������ IP�� ����Ѵ�
#define SERVERIP "::1"
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

// ����� ���� ������ ���� �Լ�
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

	// ���� �ʱ�ȭ
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
	// serveraddr.sin_addr.s_addr = inet_addr(SERVERIP); �� ����ؼ�
	// WSAStringToAddress() �Լ��� ����Ѵ�
	int addrlen = sizeof(serveraddr);
	WSAStringToAddress((LPSTR)SERVERIP, AF_INET6, NULL, (SOCKADDR*)&serveraddr, &addrlen);
	// serveraddr.sin_port -> serveraddr.sin6_port
	serveraddr.sin6_port = htons(SERVERPORT);
	retval = connect(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR)
		err_quit(TEXT("connect()"));

	// ������ ��ſ� ����� ����
	char buf[BUFSIZE + 1];
	int len;

	// ������ ������ ���
	while (1)
	{
		// ������ �Է�
		printf("\n[���� ������] ");
		if (fgets(buf, BUFSIZE + 1, stdin) == NULL)
			break;

		// '\n' ���� ����
		len = strlen(buf);
		if (buf[len - 1] == '\n')
			buf[len - 1] = '\0';
		if (strlen(buf) == 0)
			break;

		// ������ ������
		// send()
		retval = send(sock, buf, strlen(buf), 0);
		if (retval == SOCKET_ERROR)
		{
			err_display(TEXT("send()"));
			break;
		}
		printf("[TCP Ŭ���̾�Ʈ] %d����Ʈ�� ���½��ϴ�.\n", retval);

		// ������ �ޱ�
		retval = recvn(sock, buf, retval, 0);
		if (retval == SOCKET_ERROR)
		{
			err_display(TEXT("recv()"));
			break;
		}
		else if (retval == 0)
			break;

		// ���� ������ ���
		buf[retval] = '\0';
		printf("[TCP Ŭ���̾�Ʈ] %d����Ʈ�� �޾ҽ��ϴ�.\n", retval);
		printf("[���� ������] %s\n", buf);
	}

	closesocket(sock);

	// ���� ����
	WSACleanup();
	return 0;
}