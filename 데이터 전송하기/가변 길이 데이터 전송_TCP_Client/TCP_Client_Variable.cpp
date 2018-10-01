#pragma comment(lib,"ws2_32")
#include <WinSock2.h>
#include <stdlib.h>
#include <stdio.h>

#define SERVERIP "127.0.0.1"
#define SERVERPORT 9000
// 50����Ʈ ���۸� ����� �����͸� ������
#define BUFSIZE 50

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

	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
		err_quit(TEXT("socket()"));

	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = connect(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR)
		err_quit(TEXT("connect()"));

	// BUFSIZE ũ���� ���۸� �����Ѵ�
	char buf[BUFSIZE];
	// ������ ���ڿ� �����͸� �����Ѵ�
	const char* testdata[] = {
		"�ȳ��ϼ���",
		"�ݰ�����",
		"���õ��� �� �̾߱Ⱑ ���� �� ���׿�",
		"���� �׷��׿�",
	};

	// ���ڿ� ���̸� ����� ����� ��´�
	int len;

	// ���ڿ� �����͸� ���ۿ� �����ϰ� �� ���� '\n'�� �߰��Ѵ�
	for (int i = 0; i < 4; i++)
	{
		len = strlen(testdata[i]);
		strncpy(buf, testdata[i], len);
		buf[len++] = '\n';

		// len ũ�⸸ŭ �����͸� ������, len�� ���ڿ� �� '\n'�� ���� �������� ���̸� ��Ÿ����
		retval = send(sock, buf, len, 0);
		if (retval == SOCKET_ERROR)
		{
			err_display(TEXT("send()"));
			break;
		}

		printf("[TCP Ŭ���̾�Ʈ] %d����Ʈ�� ���½��ϴ�.\n", retval);
	}

	closesocket(sock);

	WSACleanup();

	return 0;
}