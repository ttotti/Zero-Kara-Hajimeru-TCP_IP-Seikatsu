

// ------------------------------------------------------//
// HelloNetwork.cpp �ҽ�									 //
//                                                       //
// �� ������Ʈ�� �������� ������ ��Ƽ����Ʈ ���� ���� �Դϴ� //
// ------------------------------------------------------//

#pragma comment(lib,"ws2_32")
#include <WinSock2.h>

// IP �ּҸ� �����ϱ� ���� in_addr�� in6_addr ����ü�� inaddr.h(WinSock2.h ����) �� in6addr.h(WS2tcpip.h�� ����) �� ������ ���� ���ǵǾ� �ִ�

// IPv4 �ּҸ� ��� in_addr ����ü�� ���� �޸� ������ ����
// 8��Ʈ(S_un_b), 16��Ʈ(S_un_w), 32��Ʈ(S_addr) ������ ������ �� �ְ� ���� ����ü(S_un)���� �� �� �ִ�
// ���� ���α׷������� �밳 32��Ʈ ������ �����ϹǷ� S_un, S_addr �ʵ带 ����ϸ�, ��ũ�θ� ���� �����ǵ� s_addr�� ����ϸ� ���ϴ�

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

// in_addr ����ü�� �޸� IPv6 �ּҸ� ��� in6_addr ����ü�� �ܼ��� ����Ʈ �Ǵ� ���� �迭�� ���ǵǾ� �ִ�

/*typedef struct in6_addr
{
	union
	{
		u_char Byte[16];
		u_short Word[8];
	} u;
}IN6_ADDR; */

// SOCKADDR_IN ����ü�� ������ ������ ���� �ּ� ����ü�� ũ�Ⱑ SOCKADDR ����ü�� �ٸ��� �� �� �ִ�
// IrDA �������ݿ� ����ϴ� SOCKADDR_IRDA ����ü��
// ������� �������ݿ� ����ϴ� SOCKADDR_BTH ����ü�� ���� ǥ���ߴ�

/* ���� �ּ� ����ü ũ�� �� (����Ʈ ����)
   SOCKADDR      16
   SOCKADDR_IN   16
   SOCKADDR_IN6  28
   SOCKADDR_IRDA 32
   SOCKADDR_BTH  30 */

// ���� ���α׷����� ���� �ּ� ����ü�� ����ϴ� ��Ȳ�� ���� �� ������ ���� �� �ִ�

// 1. ���� ���α׷��� ���� �ּ� ����ü�� �ʱ�ȭ �ϰ� ���� �Լ��� �Ѱ��ִ� ��� (SockerFunc()�� ������ ���� �Լ��� ��Ÿ��)
/*
	���� �ּ� ����ü�� �ʱ�ȭ �Ѵ�
	SOCKADDR_IN addr;
	SocketFunc(..., (SOCKADDR*)&addr, sizeof(addr), ...);
*/

// 2. ���� �Լ��� ���� �ּ� ����ü�� �Է����� �޾� ������ ä���, ���� ���α׷��� �̸� ��� ���� �������� ����ϴ� ��� (SockerFunc()�� ������ ���� �Լ��� ��Ÿ��)
/*
	 SOCKADDR_IN addr;
	 SocketFunc(..., (SOCKADDR*)&addr, sizeof(addr), ...);

	 ���� �ּ� ����ü�� ����Ѵ�
	 ...
*/

// ���� �ּ� ����ü�� ����Ҷ� ������ ������ �ִ�
// ���� �ּ� ����ü�� ũ�Ⱑ ũ�� ������ ���� �Լ� ���ڷ� ������ ���� �׻� �ּ� ���� ����ϸ�,
// �ݵ�� SOCKADDR ������ ������ ��ȯ�ؾ� �Ѵ�
// ���� ����� �������ݿ� ���� ���� �ּ� ����ü�� ũ�Ⱑ �޶����Ƿ� sizeof �����ڸ� ����� ���� ũ�� ������ ���� �����ؾ� �Ѵ�

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


// ���� �ּ� ����ü�� ��Ʈ��ũ ���α׷����� �ʿ��� �ּ� ������ ��� �ִ� ����ü�� �پ��� ���� �Լ��� ���ڷ� ����Ѵ�
// �������� ü�迡 ���� �ּ� ���� ����� �ٸ��Ƿ� �پ��� ���� �ּ� ����ü�� �����Ѵ�
// �⺻�� �Ǵ� ���� SOCKADDR ����ü�� winsock2.h ���Ͽ� ������ ���� ���ǵǾ� �ִ�

/* typedef struct sockaddr
   {
 	 u_short sa_family;
	 char sa_data[14];
   } SOCKADDR; */

// �߰� u_short = unsigned short

// sa_family : �ּ� ü�踦 ��Ÿ���� 16��Ʈ ���� ���̴� ��) TCP/IP �������� - AF_INET �Ǵ� AF_INET6
// sa_data[14] : �ش� �ּ� ü�迡�� ����� �ּ� ������ ��´� ��) TCP/IP ���������� ����Ѵٸ� IP �ּҿ� ��Ʈ ��ȣ�� ����ȴ�

// ���� ���α׷��ֿ����� ���� ���α׷��� ����� ���������� ������ �´� ������ ���� �ּ� ����ü�� ����Ѵ�
// ���� ��� TCP/IP ������ SOCKADDR_IN �Ǵ� SOCKADDR_IN6
// IrDa(���ܼ� ���� ��� ��������) ������ SOCKADDR_IRDA �� ����Ѵ�

// TCP ����(IPv4)
DWORD WINAPI TCPServer4(LPVOID arg)
{
	int retval;

	// socket() TCP(IPv4) ���� ����
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET)err_quit("socket()");

	// bind()
	/* IPv4 ���� �ּ� ����ü*/

	/*typedef struct sockaddr_in
	{
		short          sin_fimily;
		u_short        sin_port;
		struct in_addr sin_addr;
		char           sin_zero[8]; // (0���� ����)
	} SOCKDDR_IN;
	*/

	// ���� ����ϴ� ����ü �ʵ�
	// sin_family : �ּ� ü�踦 �ǹ��ϸ�, AF_INET ���� ����Ѵ�
	// sin_port : ��Ʈ ��ȣ�� �ǹ��ϸ�, ��ȣ ���� 16��Ʈ ���� ���� ����Ѵ�
	// sin_addr : IP �ּҸ� �ǹ��ϸ�, 32��Ʈ in_addr ����ü�� ����Ѵ�

	SOCKADDR_IN serveraddr;
	// ZeroMesmory(Destination(���), Length(����)) - �� ����� �޸𸮸� 0���� ä��ϴ�
	ZeroMemory(&serveraddr, sizeof(serveraddr));

	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);

	// ���� ���α׷����� ���� �ּ� ����ü�� ����ϴ� ��Ȳ (���� �ּ��� ���ǻ����� �����ִ�)
	retval = bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR)err_quit("bind()");

	// listen()
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) err_quit("listen()");

	// ������ ��ſ� ����� ����
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

		// ������ Ŭ���̾�Ʈ ���� ���
#pragma warning(suppress : 4996)
		printf("\n[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

		// Ŭ���̾�Ʈ�� ������ ���
		while (1)
		{
			// ������ �ޱ�
			retval = recv(client_sock, buf, BUFSIZE, 0);

			if (retval == SOCKET_ERROR)
			{
				err_display("recv()");
				break;
			}
			else if (retval == 0)
				break;

			// ���� ������ ���
			buf[retval] = '\0';
			printf("%s", buf);
		}

		// closesocket()
		closesocket(client_sock);

#pragma warning(suppress : 4996)
		printf("\n[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
	}

	// closesocket()
	closesocket(listen_sock);

	return 0;
}

// TCP ����(IPv6)
DWORD WINAPI TCPServer6(LPVOID arg)
{
	int retval;

	// socket()
	SOCKET listen_sock = socket(AF_INET6, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET) err_quit("socket()");

	// bind()
	/* IPv6 ���� �ּ� ����ü*/

	/*typedef struct sockaddr_in6
	{
		short           sin6_fimily;
		u_short         sin6_port;
		u_long          sin6_flowinfo  // ��κ� 0���� ����
		struct in6_addr sin6_addr;
		u_long          sin6_scope_id // ��κ� 0���� ����
	} SOCKDDR_IN6;
	*/

	// ���� ����ϴ� ����ü �ʵ�
	// sin6_family : �ּ� ü�踦 �ǹ��ϸ�, AF_INET6 ���� ����Ѵ�
	// sin6_port : ��Ʈ ��ȣ�� �ǹ��ϸ�, ��ȣ ���� 16��Ʈ ���� ���� ����Ѵ�
	// sin6_addr : IP �ּҸ� �ǹ��ϸ�, 128��Ʈ in6_addr ����ü�� ����Ѵ�

	SOCKADDR_IN6 serveraddr;
	// ZeroMesmory(Destination(���), Length(����)) - �� ����� �޸𸮸� 0���� ä��ϴ�
	ZeroMemory(&serveraddr, sizeof(serveraddr));

	serveraddr.sin6_family = AF_INET6;
	serveraddr.sin6_addr = in6addr_any;
	serveraddr.sin6_port = htons(SERVERPORT);

	// ���� ���α׷����� ���� �ּ� ����ü�� ����ϴ� ��Ȳ (���� �ּ��� ���ǻ����� �����ִ�)
	retval = bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("bind()");

	// listen()
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) err_quit("listen()");

	// ������ ��ſ� ����� ����
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

		// ������ Ŭ���̾�Ʈ ���� ���
		char ipaddr[50];
		DWORD ipaddrlen = sizeof(ipaddr);
#pragma warning(suppress : 4996)
		WSAAddressToString((SOCKADDR*)&clientaddr, sizeof(clientaddr), NULL, ipaddr, &ipaddrlen);
		printf("\n[TCP ����] Ŭ���̾�Ʈ ����: %s\n", ipaddr);

		// Ŭ���̾�Ʈ�� ������ ���
		while (1)
		{
			// ������ �ޱ�
			retval = recv(client_sock, buf, BUFSIZE, 0);
			if (retval == SOCKET_ERROR)
			{
				err_display("recv()");
				break;
			}
			else if (retval == 0)
				break;

			// ���� ������ ���
			buf[retval] = '\0';
			printf("%s", buf);
		}

		// closesocket()
		closesocket(client_sock);
		printf("\n[TCP ����] Ŭ���̾�Ʈ ����: %s\n", ipaddr);
	}

	// closesocket
	closesocket(listen_sock);

	return 0;
}

int main(int argc, char* argv[])
{
	// ���� �ʱ�ȭ
	WSADATA wsa;

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	HANDLE hThread[2];
	hThread[0] = CreateThread(NULL, 0, TCPServer4, NULL, 0, NULL);
	hThread[1] = CreateThread(NULL, 0, TCPServer6, NULL, 0, NULL);

	WaitForMultipleObjects(2, hThread, TRUE, INFINITE);

	// ���� ����
	WSACleanup();

	return 0;
}