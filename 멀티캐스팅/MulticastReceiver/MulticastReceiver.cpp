#pragma comment(lib,"ws2_32")
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

// ��Ƽĳ������ ��ӵ� IP (�ڼ��Ѱ� ���� ����)
#define MULTICASTIP "235.7.8.9"
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

	// socket() ���� ����
	SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock == INVALID_SOCKET) err_quit("socket()");

	// SO_REUSEADDR �ɼ� ����
	// ��Ƽĳ��Ʈ ������ �� �̻� �����ص� ������ ���� ������ SO_REUSEADDR �ɼ��� ����߱� �����̴�
	// ��Ƽĳ��Ʈ �����͸� �����ϰ��� �ϴ� ���� ���α׷������� SO_REUSEADDR �ɼ��� �����ϱ⸦ �����ϴµ� �� ������

	// � ȣ��Ʈ�� ��Ʈ��ũ ���� ���α׷� �� ���� �ִµ� ���� ���� �ٸ� ��Ƽĳ��Ʈ �׷쿡 �����ؼ� �����͸� �����ϰ��� �Ѵ�
	// �׷��� ���������� �� ��Ƽĳ��Ʈ �׷��� IP �ּҴ� �ٸ����� (�쿬��)���� ��Ʈ ��ȣ�� �����͸� �޵��� ��ӵǾ� �ִ�
	// �� ��� ���� ������ ���α׷��� ������ ������ �� ��°�� ������ ���α׷��� ���� ��Ʈ ��ȣ�� ����Ϸ��� �ϹǷ� ���ε� ������ �߻��� ���̴�
	// Ȯ���� ���� ������ �̿� ���� �������� ������ ���� �� �ֱ� ������ ��Ƽĳ��Ʈ ���� ���� ���α׷������� SO_REUSEADDR �ɼ��� �����صδ� ���� ����
	BOOL optval = TRUE;
	retval = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char*)&optval, sizeof(optval));
	if (retval == SOCKET_ERROR) err_quit("setsockopt()");

	// bind()
	SOCKADDR_IN localaddr;
	ZeroMemory(&localaddr, sizeof(localaddr));
	localaddr.sin_family = AF_INET;
	localaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	localaddr.sin_port = htons(LOCALPORT);
	retval = bind(sock, (SOCKADDR*)&localaddr, sizeof(localaddr));
	if (retval == SOCKET_ERROR) err_quit("bind()");

	// ��Ƽĳ��Ʈ �׷� ����
	struct ip_mreq mreq;
	mreq.imr_multiaddr.s_addr = inet_addr(MULTICASTIP);
	mreq.imr_interface.s_addr = htonl(INADDR_ANY);
	retval = setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&mreq, sizeof(mreq));
	if (retval == SOCKET_ERROR) err_quit("setsockopt()");

	// ������ ��ſ� ����� ����
	SOCKADDR_IN peeraddr;
	int addrlen;
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

		// ���� ������ ���
		buf[retval] = '\0';
		printf("[UDP/%s:%d] %s\n", inet_ntoa(peeraddr.sin_addr), ntohs(peeraddr.sin_port), buf);
	}

	// ��Ƽĳ��Ʈ �׷� Ż��
	retval = setsockopt(sock, IPPROTO_IP, IP_DROP_MEMBERSHIP, (char*)&mreq, sizeof(mreq));
	if (retval = SOCKET_ERROR) err_quit("setsocketopt()");

	// closesocket()
	closesocket(sock);

	// ���� ����
	WSACleanup();

	return 0;
}
