#pragma comment(lib, "ws2_32")
#include <winsock2.h>
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
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET)
		err_quit(TEXT("socket()"));

	// bind() : ������ ���� IP �ּҿ� ���� ��Ʈ ��ȣ�� �����Ѵ�
	/*int bind(
		SOCKET s,					 // Ŭ���̾�Ʈ ������ ������ �������� ���� ����, ���� IP �ּҿ� ���� ��Ʈ ��ȣ�� ���� �������� ���� ���´�
		const struct sockaddr* name, // ���� �ּ� ����ü(TCP/IP�� ��� SOCKADDR_IN �Ǵ� SOCKADDR_IN6)�� ���� IP �ּҿ� ���� ��Ʈ ��ȣ�� �ʱ�ȭ �Ͽ� �����Ѵ�
		int namelen					 // ���� �ּ� ����ü�� ����(����Ʈ ����)��
	)   - ���� : 0, ���� : SOCKET_ERROR - */ 

	// ���� �ּ� ����ü ������ �����ϰ� 0���� �ʱ�ȭ �Ѵ�
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	// ���ͳ� �ּ� ü��(IPv4)�� ����Ѵٴ� �ǹ̷� AF_INET�� �����Ѵ�
	serveraddr.sin_family = AF_INET;
	// ������ ���� IP �ּҸ� �����Ѵ�
	// ������ ��� Ư�� IP �ּ� ��� INADDR_ANY(0���� ���ǵ�) ���� ����ϴ� ���� �ٶ����ϴ�
	// ������ IP �ּҸ� �� �� �̻� ������ ��� INADDR_ANY ���� ���� �ּҷ� �����ϸ� Ŭ���̾�Ʈ�� ��� IP �ּҷ� �����ϵ� �޾Ƶ��� �� �ִ�
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	// ������ ���� ��Ʈ ��ȣ�� �����Ѵ�(���� ��� 9000��), htons() �Լ��� �̿��� ����Ʈ ������ ������ ���� �����ؾ� �Ѵ�
	serveraddr.sin_port = htons(SERVERPORT);
	// bind() �Լ��� ȣ���ϰ� ������ ó���Ѵ�, �� ��° ���ڴ� �׻� (SOCKADDR*)������ ��ȯ�ؾ� �Ѵ�
	retval = bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR)
		err_quit(TEXT("bind()"));

	// listen() : ������ TCP ��Ʈ ���¸� LISTENING ���� �ٲ۴�, (�̴� Ŭ���̾�Ʈ ������ �޾Ƶ��� �� �ִ� ���°� ���� �ǹ��Ѵ�)
	/*int listen(
		SOCKET s,   // Ŭ���̾�Ʈ ������ ������ �������� ���� ��������, bind() �Լ��� ���� IP �ּҿ� ���� ��Ʈ ��ȣ�� ������ ���´�
		int backlog // ������ ���� ó������ �ʴ��� ���� ������ Ŭ���̾�Ʈ�� ������
					   Ŭ���̾�Ʈ�� ���� ������ ���� ť�� ����Ǵµ�, backlog�� �� ���� ť�� ���̸� ��Ÿ����, 
					   �Ϻ� �������ݿ��� ���� ������ �ִ��� ����Ϸ��� SOMAXCONN ���� �����Ѵ�
	)   - ���� : 0, ���� : SOCKET_ERROR - */

	// backlog�� �ִ����� �Ͽ� listen() �Լ��� ȣ���ϰ� ������ ó���Ѵ�
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR)
		err_quit(TEXT("listen()"));

	// ������ ��ſ� ����� ����
	// accept() �Լ��� ���� ���� ������ ������ �����Ѵ�
	SOCKET client_sock;
	// accept() �Լ��� �� ��° ���ڷ� ����Ѵ�, (accept() �Լ��� �����ϸ� ������ Ŭ���̾�Ʈ�� IP �ּҿ� ��Ʈ ��ȣ�� ���⿡ ����ȴ�)
	SOCKADDR_IN clientaddr;
	// accept() �Լ��� �� ��° ���ڷ� ����Ѵ�
	int addrlen;
	// ���� �����͸� ������ ���� ���α׷� ���۴�
	char buf[BUFSIZE + 1];

	// �Ϲ������� ������ ��� Ŭ���̾�Ʈ ��û�� ó���ؾ� �ϹǷ� ���� ������ ����
	while (1)
	{
		// accept() : ������ Ŭ���̾�Ʈ�� ����� �� �ֵ��� ���ο� ������ �����ؼ� �����Ѵ�
		//			  ���� ������ Ŭ���̾�Ʈ�� �ּ� ����(���� ���忡���� ���� IP �ּҿ� ���� ��Ʈ ��ȣ, Ŭ���̾�Ʈ ���忡���� ����IP �ּҿ� ���� ��Ʈ ��ȣ) �� �˷��ش�

		/*SOCKET accept(
			SOCKET s,              // Ŭ���̾�Ʈ ������ ������ �������� ���� ��������, 
								      bind() �Լ��� ���� IP �ּҿ� ���� ��Ʈ ��ȣ�� �����ϰ� listen() �Լ��� TCP ��Ʈ ���¸� LISTENING ���� ������ ���´�
			struct sockaddr* addr, // ���� �ּ� ����ü�� �����ϸ� ������ Ŭ���̾�Ʈ�� �ּ� ����(IP �ּҿ� ��Ʈ ��ȣ)�� ä������
			int* addrlen		   // ������ ������ addr�� ����Ű�� ���� �ּ� ����ü�� ũ��� �ʱ�ȭ�� �� �����Ѵ�
									  accept() �Լ��� �����ϸ� *addrlen ������ accept() �Լ��� ä������ �ּ� ������ ũ��(����Ʈ ����)�� ���� �ȴ�
		)  - ���� : ���ο� ����, ���� : INVALID_SOCKET - */

		// ������ Ŭ���̾�Ʈ�� ���� ��� accept() �Լ��� ������ ��� ����(WAIT_STATE �Ǵ� SUSPENDED_STATE)�� �����
		// �̶� �۾������ڸ� ������ CPU ������ Ȯ���ϸ� 0���� ǥ�õȴ�, Ŭ���̾�Ʈ�� �����ϸ� ������ ����� accept() �Լ��� ��μ� �����ϰ� �ȴ�

		// accept() �Լ��� �� ��° ���ڷ� ������ ������ ���� addrlen�� ���� �ּ� ����ü ����(clientaddr)�� ũ��� �ʱ�ȭ �Ѵ�
		addrlen = sizeof(clientaddr);
		// accept() �Լ��� ȣ���ϰ� ������ ó���Ѵ�
		// ������ ����� ���� �Լ��� �޸� ������ �߻��ϸ� err_display() �Լ��� �̿��� ȭ�鿡 ��ü���� ���� �޽����� ǥ���ϰ� ���� ������ Ż���Ѵ�
		client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET)
		{
			err_display(TEXT("accept()"));
			break;
		}

		// ������ Ŭ���̾�Ʈ ���� ���
		// ������ Ŭ���̾�Ʈ�� IP �ּҿ� ��Ʈ ��ȣ�� ȭ�鿡 ����Ѵ� (inet_ntoa()�� ntohs() �Լ� ���)
		printf("\n[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ� = %s, ��Ʈ ��ȣ = %d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

		// Ŭ���̾�Ʈ�� ������ ���
		// accept() �Լ��� ������ ������ �̿��� Ŭ���̾�Ʈ�� ����Ѵ�
		// recv() �Լ��� ���� ���� 0(��������) �Ǵ� SOCKET_ERROR(���� �߻�)�� �� ������ ��� ������ ���� �����͸� �����Ѵ�
		while (1)
		{
			// ������ �ޱ�
			// recv() �Լ��� ȣ���ϰ� ������ ó���Ѵ�, Ŭ���̾�Ʈ�κ��� ���� ������ ũ�⸦ �̸� �� �� �����Ƿ� ����� ���� recvn()�Լ��� ����� �� ����
			retval = recv(client_sock, buf, BUFSIZE, 0);
			if (retval == SOCKET_ERROR)
			{
				err_display(TEXT("recv()"));
				break;
			}
			else if (retval == 0)
				break;

			// ���� ������ ���
			// ���� ������ ���� '\0' �� �߰��� ȭ�鿡 ����Ѵ�
			buf[retval] = '\0';
			printf("[TCP\%s:%d] %s\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port), buf);

			// ������ ������
			// send() �Լ��� ȣ���ϰ� ������ ó���Ѵ�
			retval = send(client_sock, buf, retval, 0);
			if (retval == SOCKET_ERROR)
			{
				err_display(TEXT("send()"));
				break;
			}
		}

		// closesocket()
		// Ŭ���̾�Ʈ�� ����� ��ġ�� ������ �ݰ�, ������ ������ Ŭ���̾�Ʈ�� IP �ּҿ� ��Ʈ ��ȣ�� ȭ�鿡 ����Ѵ�
		closesocket(client_sock);
		printf("[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ� = %s, ��Ʈ ��ȣ = %d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
	}

	// closesocket()
	closesocket(listen_sock);

	// ���� ����
	WSACleanup();

	return 0;
}