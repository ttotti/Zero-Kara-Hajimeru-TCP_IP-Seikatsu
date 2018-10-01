#pragma comment(lib, "ws2_32")
#include <winsock2.h>
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

// ����� ���� ������ ���� �Լ�
// recvn() �Լ� : ����� ���� �Լ��� recv() �Լ��� ���°� ���� recv() �Լ��� ������ ���� �ڵ带 �ս��� recvn() �Լ��� ��ü�� �� �ִ�
int recvn(SOCKET s, char* buf, int len, int flags)
{
	// ���������� ȣ���ϴ� recv() �Լ��� ���� ���� ������ ������
	int received;
	// ������ ���� ptr�� ���� ���α׷� ������ ���� �ּҸ� ����Ų��, �����͸� ���� ������ ptr ���� �����Ѵ�
	char* ptr = buf;
	// left ������ ���� ���� ���� ������ ũ���, �����͸� ���� ������ left ���� �����Ѵ�
	int left = len;

	// ���� ���� ���� �����Ͱ� ������ ��� ������ ����
	while (left > 0)
	{
		// recv() �Լ��� ȣ���� ������ �߻��ϸ� ��ٷ� �����Ѵ�
		received = recv(s, ptr, left, flags);
		if (received == SOCKET_ERROR)
			return SOCKET_ERROR;
		// recv() �Լ��� ���� ���� 0�̸�(���� ����), ��밡 �����͸� �� ������ ���� ���̹Ƿ� ������ ����������
		else if (received == 0)
			break;
		// recv() �Լ��� ������ ���Ƿ� left�� ptr ������ �����Ѵ�
		left -= received;
		ptr += received;
	}

	// ���� ����Ʈ ���� �����Ѵ�, ������ �߻��ϰų� ��밡 ������ ������ ��찡 �ƴϸ� left������ �׻� 0�̹Ƿ� ���� ���� len�� �ȴ�
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
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
		err_quit(TEXT("socket()"));

	// connect() : TCP �������� ���ؿ��� ������ ���� ������ �����Ѵ�

	/* int connect(
		SOCKET s,					 // ������ ����� �������� ���� �����̴�
		const struct sockaddr* name, // ���� �ּ� ����ü(TCP/IP�� ��� SOCKADDR_IN �Ǵ� SOCKADDR_IN6)�� ���� �ּ�(��, ���� IP �ּҿ� ���� ��Ʈ ��ȣ)�� �ʱ�ȭ�Ͽ� �����Ѵ�
		int namelen					 // ���� �ּ� ����ü�� ����(����Ʈ ����)��
	)   - ���� : 0, ���� : SOCKET_ERROR - */

	// �Ϲ������� Ŭ���̾�Ʈ�� ������ �޸� bind() �Լ��� ȣ������ �ʴ´�, bind() �Լ��� ȣ������ ���� ���¿��� connect() �Լ��� ȣ���ϸ�,
	// �ü���� �ڵ����� ���� IP �ּҿ� ���� ��Ʈ ��ȣ�� �Ҵ����ش� (�̶� �ڵ����� �Ҵ�Ǵ� ��Ʈ ��ȣ�� ������ ������ ���� �ٸ� �� �ִ�)

	// ���� �ּ� ����ü ������ 0���� �ʱ�ȭ �ϰ� ,IP �ּҿ� ��Ʈ ��ȣ�� �����Ѵ�
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
	serveraddr.sin_port = htons(SERVERPORT);
	// connect() �Լ��� ȣ���ϰ� ������ ó���Ѵ�
	retval = connect(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR)
		err_quit(TEXT("connect()"));

	// ������ ��ſ� ����� ����
	// ���� ������ �Ǵ� ���� �����͸� ������ ���۴�
	char buf[BUFSIZE + 1];
	// ����ڰ� �Է��� ���ڿ� �������� ���̸� ����� �� ����Ѵ�
	int len;

	// ������ ������ ���
	while (1)
	{
		// ������ �Է�
		// fgets() �Լ��� ����� ����ڷκ��� ���ڿ��� �Է¹޴´�
		printf("\n[���� ������] ");
		if (fgets(buf, BUFSIZE + 1, stdin) == NULL)
			break;

		// '\n' ���� ����
		// '\n' ���ڸ� �����Ѵ�, ������ ��� �� �ٹٲ� ���� Ȥ�� �ٹٲ� ����� ������ �����ϰ� �ϱ� �����̴�
		len = strlen(buf);
		if (buf[len - 1] == '\n')
			buf[len - 1] = '\0';
		// '\n' ���ڸ� ������ �� ���ڿ� ���̰� 0�̸�, ����ڰ� ���ڸ� �Է����� �ʰ� ��ٷ� Enter Ű�� �����ٴ� ���̴�
		// �� ��� ������ ���������� closesocket() �Լ��� ȣ���� ������ ���� �����Ѵ�
		if (strlen(buf) == 0)
			break;

		// ������ ������
		// send() : ���� ���α׷� �����͸� �ü���� �۽� ���ۿ� ���������ν� �����͸� �����Ѵ�
		//		    send() �Լ��� ������ ���簡 �����ϸ� ��ٷ� �����Ѵ�, ���� send() �Լ��� �����ߴٰ� ���� �����Ͱ� ���۵� ���� �ƴϸ� ���� �ð��� �����߸� �Ϻ� ��������(TCP/IP ��)�� ���� ������ �Ϸ�ȴ�

		/* int send(
			SOCKET s,       // ����� ���� ����� �����̴�
			const char* buf // ���� �����͸� ��� �ִ� ���� ���α׷� ������ �ּҴ�
			int len,		// ���� ������ ũ��(����Ʈ ����)��
			int flags		// send() �Լ��� ������ �ٲٴ� �ɼ�����, ��κ� 0�� ����ϸ� �ȴ�
							   ��� ������ ������ MSG_DONTROUTE(���ӿ����� ����ϴ��� ���õ�)�� MSG_OOB(���� ��� �� ��)�� �ִ�
		)   - ���� : ���� ����Ʈ ��,  ���� : SOCKET_ERROR - */

		// send() �Լ��� ù ��° ���ڷ� �����ϴ� ������ Ư���� ���� ������ ���� �� ������ �������� ������ �� �� �ִ�
		/* ���ŷ ���� : ���� ���� ������ ������ ��� ���ŷ �����̴�
						���ŷ ������ ������� send() �Լ��� ȣ���ϸ�, �۽� ������ ���� ������ send() �Լ��� �� ��° ������ len���� ���� ��� �ش� ���μ����� ��� ���°� �ȴ�
						�۽� ���ۿ� ����� ������ ����� ���μ����� ����� len ũ�⸸ŭ ������ ���簡 �Ͼ �� send() �Լ��� �����Ѵ� �� ��� send() �Լ��� ���� ���� len�� ����*/

		/* �ͺ��ŷ ���� : ioctlsocket() �Լ��� �̿��ϸ� ���ŷ ������ �ͺ��ŷ �������� �ٲ� �� �ִ�, 
						  �ͺ��ŷ ������ ������� send() �Լ��� ȣ���ϸ�, �۽� ������ ���� ������ŭ �����͸� ������ �� ���� ������ ����Ʈ ���� �����Ѵ�, 
						  �� ��� send() �Լ��� ���� ���� �ּ� 1, �ִ� len �̴�*/
		
		// send() �Լ��� ȣ���ϰ� ������ ó���Ѵ�
		// ���ŷ ������ ����ϰ� �����Ƿ� send() �Լ��� ���� ���� strlen(buf) ���� ���� ���̴�
		retval = send(sock, buf, strlen(buf), 0);
		if (retval == SOCKET_ERROR)
		{
			err_display(TEXT("send()"));
			break;
		}
		printf("[TCP Ŭ���̾�Ʈ] %d����Ʈ�� ���½��ϴ�.\n", retval);

		// ������ �ޱ�
		// recv() : �ü���� ���� ���ۿ� ������ �����͸� ���� ���α׷� ���ۿ� �����Ѵ�

		/* int recv(
			SOCKET s,  // ����� ���� ����� �����̴�

			char* buf, // ���� �����͸� ������ ���� ���α׷� ������ �ּҴ�

			int len,   // �ü���� ���� ���۷κ��� ������ �ִ� ������ ũ��(����Ʈ ����)��
					      �� ���� buf�� ����Ű�� ���� ���α׷� ���ۺ��� ũ�� �ʾƾ� �Ѵ�

			int flags  //  recv() �Լ��� ������ �ٲٴ� �ɼ�����, ��κ� 0�� ����ϸ� �ȴ�
					       ��� ������ ������ MSG_PEEK�� MSG_OOB(���� ��� �� ��)�� �ִ�
						   recv() �Լ��� �⺻ ������ ���� ������ �����͸� ���� ���α׷� ���ۿ� ������ �� �ش� �����͸� ���� ���ۿ��� �����ϴ� ���̴�, 
						   ������ MSG_PEEK �ɼ��� ����ϸ� ���� ���ۿ� �����Ͱ� ��� ���´�
		)   - ���� : ���� ����Ʈ �� �Ǵ� 0(���� ���� ��), ���� : SOCKET_ERROR */

		// recv() �Լ��� ���� �� ������ �������� ������ �� �� �ִ�
		/* ���� ���ۿ� �����Ͱ� ������ ��� : recv() �Լ��� �� ��° ������ len���� ũ�� ���� �������� �����ϸ� ���� �����͸� ���� ���α׷� ���ۿ� ������ �� ���� ������ ����Ʈ ���� �����Ѵ�
											�� ��� recv() �Լ��� ���� ���� �ּ� 1, �ִ� len�̴�*/

		/* ������ ���� ������ ��� : ����� ���� ���α׷��� closesocket() �Լ��� ȣ���� ������ �����Գ�, TCP �������� ���ؿ��� ���� ���Ḧ ���� ��Ŷ ��ȯ ������ �Ͼ��
									�� ��� recv() �Լ��� 0�� �����Ѵ�, recv() �Լ��� ���� ���� 0�� ��츦 ���� ����� �θ���*/

		// recv() �Լ� ��� �� Ư�� ������ ���� �� ��° ������ len���� ������ ũ�⺸�� ���� �����Ͱ� ���� ���α׷� ���ۿ� ����� �� �ִٴ� ����̴�
		// �̴� TCP�� ������ ��踦 �������� �ʴ´ٴ� Ư���� �����Ѵ�
		// ���� �ڽ��� ���� �������� ũ�⸦ �̸� �˰� �ִٸ� �׸�ŭ ���������� recv() �Լ��� ���� �� ȣ���ؾ� �Ѵ�
		// ���� ���������� ����� ���� �Լ� recvn() - 30�� �� �����ؼ� ���ϰ� ó���ϰ� �ִ�

		// recvn() �Լ��� ȣ���ϰ� ������ ó���Ѵ�
		// ������ ���� ���� �������� ũ�⸦ �̸� �˰� �����Ƿ� ����� ���� �Լ� recvn() �� ����ߴ�
		retval = recvn(sock, buf, retval, 0);
		if (retval == SOCKET_ERROR)
		{
			err_display(TEXT("recv()"));
			break;
		}
		else if (retval == 0)
			break;

		// ���� ������ ���
		// ���� ������ ���� '\0'�� �߰��Ͽ� ȭ�鿡 ����Ѵ�
		buf[retval] = '\0';
		printf("[TCP Ŭ���̾�Ʈ] %d����Ʈ�� �޾ҽ��ϴ�.\n", retval);
		printf("[���� ������] %s\n", buf);
	}

	// closesocket()
	closesocket(sock);

	// ���� ����
	WSACleanup();

	return 0;
}

