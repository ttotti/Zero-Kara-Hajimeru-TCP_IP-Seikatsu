#pragma comment(lib,"ws2_32")
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

// ��Ƽĳ������ ��ӵ� IP (�ڼ��Ѱ� ���� ����)
#define MULTICASTIP "FF12::1:2:3:4"
#define LOCALPORT 9000
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

	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()
	// IPv6 ������ �°� ����
	SOCKET sock = socket(AF_INET6, SOCK_DGRAM, 0);
	if (sock == INVALID_SOCKET) err_quit("socket()");

	// SO_REUSEADDR �ɼ� ����
	BOOL optval = TRUE;
	retval = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char*)&optval, sizeof(optval));
	if (retval == SOCKET_ERROR) err_quit("setsockopt()");

	// bind()
	// IPv6 ������ �°� ����
	SOCKADDR_IN6 localaddr;

	ZeroMemory(&localaddr, sizeof(localaddr));

	localaddr.sin6_family = AF_INET6;
	localaddr.sin6_addr = in6addr_any;
	localaddr.sin6_port = htons(LOCALPORT);

	retval = bind(sock, (SOCKADDR*)&localaddr, sizeof(localaddr));
	if (retval == SOCKET_ERROR) err_quit("bind()");

	// �ּ� ��ȯ (���ڿ� -> IPv6)
	// IPv6 ������ �°� ����
	SOCKADDR_IN6 tmpaddr;
	int addrlen = sizeof(tmpaddr);
	WSAStringToAddress((LPSTR)MULTICASTIP, AF_INET6, NULL, (SOCKADDR*)&tmpaddr, &addrlen);

	// ��Ƽĳ��Ʈ �׷� ����
	// IPv6 ������ �°� ����
	struct ipv6_mreq mreq;
	mreq.ipv6mr_multiaddr = tmpaddr.sin6_addr;
	mreq.ipv6mr_interface = 0;
	retval = setsockopt(sock, IPPROTO_IPV6, IPV6_ADD_MEMBERSHIP, (char*)&mreq, sizeof(mreq));
	if (retval == SOCKET_ERROR) err_quit("setsockopt()");

	// ������ ��ſ� ����� ����
	SOCKADDR_IN6 peeraddr;
	char buf[BUFSIZE + 1];

	// ��Ƽĳ��Ʈ ������ �ޱ�
	while (1)
	{
		// ������ �ޱ�
		addrlen = sizeof(peeraddr);
		retval = recvfrom(sock, buf, BUFSIZE, 0, (SOCKADDR*)&peeraddr, &addrlen);
		if (retval == SOCKET_ERROR)
		{
			err_display("recvfrom()");
			continue;
		}

		// �ּ� ��ȯ (IPv6 -> ���ڿ�)
		char ipaddr[50];
		DWORD ipaddrlen = sizeof(ipaddr);
		WSAAddressToString((SOCKADDR*)&peeraddr, sizeof(peeraddr), NULL, ipaddr, &ipaddrlen);

		// ���� ������ ���
		buf[retval] = '\0';
		printf("[UDP/%s] %s\n", ipaddr, buf);
	}

	// ��Ƽĳ��Ʈ �׷� Ż��
	// IPv6 ������ �°� ����
	retval = setsockopt(sock, IPPROTO_IPV6, IPV6_DROP_MEMBERSHIP, (char*)&mreq, sizeof(mreq));
	if (retval == SOCKET_ERROR) err_quit("setsockopt()");

	// closesocket()
	closesocket(sock);

	// ���� ����
	WSACleanup();
	return 0;
}