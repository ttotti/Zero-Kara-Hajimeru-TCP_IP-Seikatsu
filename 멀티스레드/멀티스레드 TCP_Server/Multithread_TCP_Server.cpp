

// ��Ƽ������ TCP ������ �⺻ ���´� ������ ����
/* DWORD WINAPI ProcessClient(LPVOID arg)
   {
       3. ���޵� ���� ����
	   SOCKET client_sock = (SOCKET)arg;

	   4. Ŭ���̾�Ʈ ���� ���
	   addrlen = sizeof(clientaddr);
	   getpeername(client_sock, (SOCKADDR*)&clientaddr, &addrlen);

	   5. Ŭ���̾�Ʈ�� ������ ���
	   while(1)
	   {
	     ...
	   }

	   ...
   }
   
   int main(int argc, char* argv[])
   {
       ...

	   while(1)
	   {
	      1. Ŭ���̾�Ʈ ���� ����
	      client_sock = accept(lisen_sock, ...);

		  ...
		  2. ������ ����
		  CreateThread(NULL, 0, ProcessClient, (LPVOID)client_sock, 0, NULL);
	   }
   }*/

// ������ �Լ��� ���ϸ� ������ ��쿡�� ������ �ּ� ������ �����Ƿ�, ������ ���� �ּ� ������ ��� ����� �ʿ��ϴ�
// �̷� ��츦 ���� ���� �� ���� �Լ��� �غ�Ǿ� �ִ�

// getpeername() �Լ��� ���� ������ ����ü�� ����� ���� IP �ּҿ� ���� ��Ʈ ��ȣ�� �����Ѵ�
/* int getpeername(
     SOCKET s,			    // ����
	 struct sockaddr* name  // ���� �ּ� ����ü
	 int* namelen			// ���� �ּ� ����ü�� ũ�� (�� - ��� ����(value-result argument)�Ƿ� �Լ� ȣ�� ���� �ʱ�ȭ�� �ؾ� �Ѵ�)
   )  - ���� : 0, ���� : SOCKET_ERROR - */

// getsockname() �Լ��� ���� IP �ּҿ� ���� ��Ʈ ��ȣ�� �����Ѵ�
/* int getsockname(
     SOCKET s,              // ����
     struct sockaddr* name  // ���� �ּ� ����ü
     int* namelen			// ���� �ּ� ����ü�� ũ�� (�� - ��� ����(value-result argument)�Ƿ� �Լ� ȣ�� ���� �ʱ�ȭ�� �ؾ� �Ѵ�)
)  - ���� : 0, ���� : SOCKET_ERROR - */

#pragma comment(lib,"ws2_32")
#include <WinSock2.h>
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

// Ŭ���̾�Ʈ�� ������ ���
// TCP Ŭ���̾�Ʈ�� ���� �����͸� �޾� ȭ�鿡 ǥ���ϰ� �ٽ� ������ ������ �ϴ� ������ �Լ���
DWORD WINAPI ProcessClient(LPVOID arg)
{
	// ���޹��� ���ڸ� SOCKET Ÿ������ ����ȯ�ϴ� �κ�
	SOCKET client_sock = (SOCKET)arg;
	int retval;
	SOCKADDR_IN clientaddr;
	int addrlen;
	char buf[BUFSIZE + 1];

	// Ŭ���̾�Ʈ ���� ���
	// Ŭ���̾�Ʈ �ּ� ������ ��� �κ�
	addrlen = sizeof(clientaddr);
	getpeername(client_sock, (SOCKADDR*)&clientaddr, &addrlen);

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
		printf("[TCP/%s:%d] %s\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port), buf);

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

	printf("[TCP ����] Ŭ���̾�Ʈ ���� : IP �ּ� = %s, ��Ʈ ��ȣ = %d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

	return 0;
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
	// ������ �ڵ��� ������ ������ �����Ѵ�
	HANDLE hThread;

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

		// ������ ����
		// ������ Ŭ���̾�Ʈ�� ó���� �����带 �����Ѵ�
		// �̶� ������ �Լ� ���ڷ� client_sock ���� �����Ѵ�
		// SOCKET Ÿ���� ũ�Ⱑ 32��Ʈ��, void�� �����ͺ��� �۰ų�(64��Ʈ �����Ϸ�) �����Ƿ�(32��Ʈ �����Ϸ�) ������ �ּ� ��� �� ��ü�� ���� �����ص� �ȴ�
		hThread = CreateThread(NULL, 0, ProcessClient, (LPVOID)client_sock, 0, NULL);
		if (hThread == NULL)
		{
			closesocket(client_sock);
		}
		else
		{
			CloseHandle(hThread);
		}
	}

	// closesocket()
	closesocket(listen_sock);

	// ���� ����
	WSACleanup();

	return 0;
}