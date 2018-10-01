#pragma comment(lib,"ws2_32")
#include <WinSock2.h>
#include <stdlib.h>
#include <stdio.h>

#define SERVERIP "127.0.0.1"
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

	// sock()
	SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock == INVALID_SOCKET)
		err_quit(TEXT("socket()"));

	// ���� �ּ� ����ü �ʱ�ȭ
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
	serveraddr.sin_port = htons(SERVERPORT);

	// ������ ��ſ� ����� ����
	SOCKADDR_IN peeraddr;
	int addrlen;
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
		// sendto() : ���� ���α׷� �����͸� �ü���� �۽� ���ۿ� ���������ν� �����͸� �����Ѵ�

		// UDP�� ���, sendto() �Լ��� ȣ���� �� ������ ���� IP �ּҿ� ���� ��Ʈ ��ȣ�� ���� �������� ���� ���¶�� �ü���� �ڵ����� �������ش�
		// ��, sendto() �Լ��� TCP�� bind() �Լ� ������ ����Ѵ�

		// sendto() �Լ� ����
		/*
			int sendto(
			SOCKET s,                  // ��ſ� ����� �����̴�

			const char* buf,           // ���� �����͸� ��� �ִ� ���� ���α׷� ������ �ּҴ�

			int len,                   // ���� ������ ũ��(����Ʈ ����)��

			int flags,                 // sendto() �Լ��� ������ �ٲٴ� �ɼ�����, ��κ� 0�� ����ϸ� �ȴ�
										  ��� ������ ������ MSG_DONTROUTE(���ӿ����� ����ϴ��� ���õ�)�� MSG_OOB(å 10�� ����, UDP������ �ǹ� ����)�� �ִ�

			const struct sockaddr* to, // ������ �ּҸ� ��� �ִ� ���� �ּ� ����ü��
									      UDP�� ���, Ư�� ȣ��Ʈ�� ����� �ּҴ� �����̰� "��ε�ĳ��Ʈ"�� "��Ƽĳ��Ʈ" �ּҸ� ����� ���� �ִ�

			int tolen                  // ������ �ּҸ� ��� �ִ� ���� �ּ� ����ü�� ũ��(����Ʈ ����)��
			);    - ���� : ���� ����Ʈ ��, ���� : SOCKET_ERROR -
		*/

		// sendto() �Լ� ��� ���� ������ ����
		/*
			// ���� �ּ� ����ü�� �������� IP �ּҿ� ��Ʈ ��ȣ�� �ʱ�ȭ�Ѵ�
			SOCKADDR_IN serveraddr;

			...

			// �۽ſ� ���۸� �����ϰ� �����͸� �ִ´�
			char buf[BUFSIZE];

			...

			// sendto() �Լ��� �����͸� ������
			retval = sendto(sock, buf, strlen(buf), 0, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
			if(retval == SOCKET_ERROR) ���� ó��
			printf("%d ����Ʈ�� ���½��ϴ�.\n", retval);
		*/

		// sendto() �Լ��� ������ ������ ������ �����ϸ� ������ ����
		/*
			1. sendto() �Լ��� UDP ������ �����̰� TCP ���Ͽ��� ����� �� ������, �� ��� to�� tolen ���ڴ� ���õȴ�, 
			   TCP ���Ͽ� ����� ���� flags ���ڿ� MSG_OOB�� ����� �� �ִ�

			2. sendto() �Լ��� ���� �����ʹ� �������� UDP �����ͱ׷�(=��Ŷ)���� ������� ���۵Ǹ�,
			   ���� �������� recvfrom() �Լ� ȣ�� �� ������ �� �����͸� ���� �� �ִ�
			   ���� UDP�� ����� ��쿡�� TCP�� �޸� ���� ���α׷� ���ؿ��� �޽��� ��踦 �����ϴ� �۾��� �� �ʿ䰡 ����

		    3. UDP ���Ͽ� ���� sendto() �Լ��� ȣ���� ��� �� ���� ���� �� �ִ� �������� ũ�⿡ ������ �ִ�
			   �ּڰ��� 0, �ִ��� 65507(65535-20(IP ��� ũ��) - 8(UDP ��� ũ��)) ����Ʈ��

		    4. sendto() �Լ��� ���� ���� ���α׷� �����ʹ� Ŀ��(=�ü��) ������ ����Ǿ� ���۵� �� ��ٷ� ��������
			   sendto() �Լ��� �����ߴٰ� ���� ������ ������ �Ϸ�� ���� �ƴϸ�,
			   ������ ������ ����� ������ �޾Ҵ��� Ȯ���� ���� ����
		*/
		//retval = sendto(sock, buf, strlen(buf), 0, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
		if (retval == SOCKET_ERROR)
		{
			err_display(TEXT("sendto()"));
			continue;
		}
		printf("[UDP Ŭ���̾�Ʈ] %d����Ʈ�� ���½��ϴ�.\n", retval);

		// ������ �ޱ�
		// recvfrom() : �ü���� ���� ���ۿ� ������ �����͸� ���� ���α׷� ���ۿ� �����Ѵ�

		// TCP�� recv() �Լ��� �ٸ� ����, UDP ��Ŷ �����͸� �� ���� �ϳ��� ���� �� �ִٴ� ���̴�
		// ��, ���� ���α׷� ���۸� ũ�� ��´ٰ� ���� �����͸� �Ѳ����� ���� �� ����

		// recvfrom() �Լ� ����
		/*
			int recvfrom(
				SOCKET s,              // ��ſ� ����� �����̴�
									      sendto() �Լ��� ����ϴ� ���ϰ� �޸�, �� ������ �ݵ�� �����ּ�(IP �ּ�, ��Ʈ ��ȣ)�� �̸� �����Ǿ� �־�� �Ѵ�

				char* buf,             // ���� �����͸� ������ ���� ���α׷� ������ �ּҴ�

				int len,               // ���� ���α׷� ������ ũ��(����Ʈ ����)��
				                          ������ UDP ��Ŷ �����Ͱ� len���� ũ�� len ��ŭ�� �����ϰ� �������� ������, �̶� recvfrom() �Լ��� SOCKET_ERROR �� �����Ѵ�, 
										  ���� ����Ǵ� UDP ��Ŷ �������� �ִ� ũ�⸦ ������ ���� ���α׷� ���۸� �غ��صξ�� �Ѵ�

				int flags,             // recv() �Լ��� ������ �ٲٴ� �ɼ�����, ��κ� 0�� ����ϸ� �ȴ�
				                          ��� ������ ������ MSG_PEEK�� MSG_OOB(å 10���� ����, UDP������ �ǹ� ����)�� �ִ�
										  recvfrom() �Լ��� �⺻ ������ ���� ������ �����͸� ���� ���α׷� ���ۿ� ������ �� 
										  �ش� �����͸� ���� ���ۿ��� �����ϴ� �������� MSG_PEEK �ɼ��� ����ϸ� ���� ���ۿ� �����Ͱ� ��� ���´�

				struct sockaddr* from, // ���� �ּ� ����ü�� �����ϸ� �۽����� �ּ� ����(IP �ּҿ� ��Ʈ ��ȣ)�� ä������

				int* fromlen           // ������ ������ from�� ����Ű�� ���� �ּ� ����ü�� ũ��� �ʱ�ȭ�� �� �����Ѵ�
				                          recvfrom() �Լ��� �����ϸ� *fromlen ������ recvfrom() �Լ��� ä������ �ּ� ������ ũ��(����Ʈ ����)�� ���� �ȴ�
			);  - ���� : ���� ����Ʈ ��, ���� : SOCKET_ERROR -
		*/

		// recvfrom() �Լ� ��� ���� ������ ����
		/*
			// ��� ����� �ּҸ� ������ ������ �����Ѵ�
			SOCKADDR_IN peeraddr;
			int addrlen;

			...

			// ���ſ� ���۸� �����Ѵ�
			char buf[BUFSIZE];

			...

			// recvfrom() �Լ��� �����͸� �޴´�
			addrlen = sizeof(peeraddr);
			retval = recvfrom(sock, buf, BUFSIZE, 0, (SOCKADDR*)&peeraddr, &addrlen);
			if(retval == SOCKET_ERROR) ���� ó��
			printf("%d ����Ʈ�� �޾ҽ��ϴ�\n", retval);
		*/

		// recvfrom() �Լ��� ������ ������ ������ �����ϸ� ������ ����
		/*
			1. recvfrom() �Լ��� UDP ������ �����̰� TCP ���Ͽ��� ����� �� ������, �� ��� from�� fromlen ���ڴ� ���õȴ�.
			   TCP ���Ͽ� ����� ���� flags ���ڿ� MSG_OOB�� ����� �� �ִ�

			2. sendto() �Լ��� ���� �����ʹ� �������� UDP ������ �׷�(=��Ŷ)���� ������� ���۵Ǹ�,
			   ���� �������� recvfrom() �Լ� ȣ�� �� ������ �� �����͸� ���� �� �ִ�
			   ���� UDP�� ����� ��쿡�� TCP�� �޸� ���� ���α׷� ���ؿ��� �޽��� ��踦 �����ϴ� �۾��� �� �ʿ䰡 ����

			3. UDP ���Ͽ� ���� recvfrom() �Լ��� ȣ���� ��� ���� ���� 0�� �� �� �ִµ�,
			   �̴� ������ sendto() �Լ� ȣ�� �� ������ ũ�⸦ �ּڰ��� 0���� �����ߴٴ� ���̴�.
			   UDP �������ݿ��� ���� ������ ���� ������ �����Ƿ� recvfrom() �Լ��� ���� ���� 0�̶�� �ؼ� Ư���� �ǹ̰� �ִ� ���� �ƴϴ�
			   �ݸ� TCP ���Ͽ� ���� recvfrom() �Լ��� ȣ���� ��� ���� ���� 0�̸� ���� ���Ḧ �ǹ��Ѵ�

			4. ���ŷ ������ ����� ���, ���� ���� ���ۿ� ������ �����Ͱ� ������ recvfrom() �Լ��� ȣ�� �� ��ϵȴ�
		*/

		// *�߰�*
		// recvfrom() �Լ��� ����ϴ� ������ �ݵ�� ���� �ּ�(IP �ּ�, ��Ʈ ��ȣ)�� �̸� �����Ǿ� �־�� �Ѵ�
		// �� recvfrom() �Լ��� ȣ���ϱ� ���� ��𼱰� bind() �Լ� �Ǵ� �̿� �����ϴ� �Լ��� ����� ���� �ּҸ� �����ϴ� ������ �ʿ��ϴ�
		/*
			// ��������� bind() �Լ��� ȣ�������Ƿ� ���� ������ ����
			bind(sock, ...);
			...
			recvfrom(sock, ...);

			// sendto() �Լ��� bind() �Լ� ���ҵ� �ϱ� ������, sendto() �Լ��� ���� ȣ���� ��� ������ ����
			sendto(sock, ...);
			...
			recvfrom(sock, ...);
		*/
		addrlen = sizeof(peeraddr);
		retval = recvfrom(sock, buf, BUFSIZE, 0, (SOCKADDR*)&peeraddr, &addrlen);
		if (retval == SOCKET_ERROR)
		{
			err_display(TEXT("recvfrom()"));
			continue;
		}

		// �۽����� IP �ּ� üũ
		if (memcmp(&peeraddr, &serveraddr, sizeof(peeraddr)))
		{
			printf("[����] �߸��� �������Դϴ�!\n");
			continue;
		}

		// ���� ������ ���
		buf[retval] = '\n';
		printf("[UDP Ŭ���̾�Ʈ] %d����Ʈ�� �޾ҽ��ϴ�.\n", retval);
		printf("[���� ������] %s\n", buf);
	}

	// closesocket()
	closesocket(sock);

	// ���� ����
	WSACleanup();

	return 0;
}