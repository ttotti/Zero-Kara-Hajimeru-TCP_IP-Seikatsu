#pragma comment(lib,"ws2_32")
#include <WinSock2.h>
#include <stdlib.h>
#include <stdio.h>

#define SERVERPORT 9000
// 512 ����Ʈ ���۸� ����� �����͸� �д´�, ������ ���� �����ʹ� �̺��� ���� ���� ���� �����Ͷ�� �����Ѵ�
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

// ���� ������ �Լ�
// ���� ���� ���ۿ��� �����͸� �� ���� ���� ���� �� 1����Ʈ�� �������ִ� ����� ���� �Լ���
// recvline() �Լ����� ȣ���ؼ� ����� ���̴�
int _recv_ahead(SOCKET s, char* p)
{
	// ���� ���� ���ۿ��� ���� �����͸� �� ����Ʈ�� �����ϴ� �� �ʿ��� �ٽ� ������
	// �Լ��� �����ص� ���� ��� �����ؾ� �ϹǷ� static ������ �����ߴ�
	// __declspec(thread)�� ��Ƽ������ ȯ�濡���� �Լ��� �������� �����ϴµ� �ʿ��ѵ�, �� ���������� �����ص� �������
	// __declspec(thread)�� ������ ��Ƽ������ �κ��� �����Ѵ�
	__declspec(thread)static int nbytes = 0;
	__declspec(thread)static char buf[1024];
	__declspec(thread)static char* ptr;

	// ���� ���� ���ۿ��� �о���� �����Ͱ� ���� ���ų� �����Ͽ� ��� �Ҹ��� ��쿡�� 
	// ���� �о� buf[] �迭�� �����صΰ� ������ ���� ptr�� �� ���� ����Ʈ�� ����Ű�� �Ѵ�
	if (nbytes == 0 || nbytes == SOCKET_ERROR)
	{
		// recv()
		nbytes = recv(s, buf, sizeof(buf), 0);
		if (nbytes == SOCKET_ERROR)
		{
			return SOCKET_ERROR;
		}
		else if (nbytes == 0)
			return 0;
		ptr = buf;
	}

	// ���� ����Ʈ ��(nbytes)�� 1 ���ҽ�Ű��, 
	// ������ ���� ptr�� ����Ű�� �����͸� ������ ���� p �� ����Ű�� �޸� ������ �־ �����Ѵ�
	--nbytes;
	*p = *ptr++;
	return 1;
}

// ����� ���� ������ ���� �Լ�
// '\n' �� ���� ������ �����͸� �б� ���� recvline() �Լ��� �����Ѵ�.
int recvline(SOCKET s, char* buf, int maxlen)
{
	int n, nbytes;
	char c;
	char* ptr = buf;

	// ���� s���� �����͸� �� ����Ʈ�� �о buf�� ����Ű�� �޸� ������ �����ϵ�
	// '\n'�� �����ų� �ִ����(maxlen - 1)�� �����ϸ� '0'�� �ٿ� �����Ѵ�
	// '\0'�� �ݵ�� �ʿ������� ������ �����͸� ���ڿ��� ������ ����� �� �����ϴ�
	for (n = 1; n < maxlen; n++)
	{
		nbytes = _recv_ahead(s, &c);
		if (nbytes == 1)
		{
			*ptr++ = c;
			if (c == '\n')
				break;
		}
		else if (nbytes == 0)
		{
			*ptr = 0;
			return n - 1;
		}
		else
			return SOCKET_ERROR;
	}

	*ptr = 0;
	return n;
}

int main(int argc, char* argv[])
{
	int retval;

	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET)
		err_quit(TEXT("socket()"));

	// bind()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR)
		err_quit(TEXT("bind()"));

	// listen()
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR)
		err_quit(TEXT("listen()"));

	// ������ ��ſ� ����� ����
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;
	// 1 ����Ʈ �� ���ڸ� ������ BUFSIZE + 1 ũ���� ���۸� �����Ѵ�
	char buf[BUFSIZE + 1];

	while (1)
	{
		// accept()
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET)
		{
			err_display(TEXT("accept()"));
			break;
		}

		// ������ Ŭ���̾�Ʈ ���� ���
		printf("\n[TCP ����] Ŭ���̾�Ʈ ���� : IP �ּ� = %s, ��Ʈ ��ȣ = %d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

		// Ŭ���̾�Ʈ ������ ���
		while (1)
		{
			// ������ �ޱ�
			// recvline() �Լ��� ȣ���� '\n'�� ���� ������ �����͸� �޴´�
			// +1�� 1����Ʈ �� ���ڸ� ������ ������, ������ �ִ� BUFSIZE(=512)ũ�⸸ŭ �����͸� �д´�
			retval = recvline(client_sock, buf, BUFSIZE + 1);
			if (retval == SOCKET_ERROR)
			{
				err_display(TEXT("recv()"));
				break;
			}
			else if (retval == 0)
				break;

			// ���� ������ ���
			// buf[] �迭�� �ִ� ������ ��ü�� '\n\0' �� ���ԵǾ� �����Ƿ� �ٹٲ� ���� ȭ�鿡 ���ڿ��� ����ϸ� �ȴ�
			printf("[TCP/%s:%d] %s", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port), buf);
		}

		// closesocket()
		closesocket(client_sock);
		printf("[TCP ����] Ŭ���̾�Ʈ ���� : IP �ּ� = %s, ��Ʈ ��ȣ = %d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
	}

	// closesocket()
	closesocket(listen_sock);

	// ���� ����
	WSACleanup();

	return 0;
}