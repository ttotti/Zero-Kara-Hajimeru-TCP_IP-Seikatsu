#pragma comment(lib,"ws2_32")
#include <WinSock2.h>

#include <stdio.h>

// ��� ���� ���� ���α׷��� ������ ���� ���� ������ ������ �ִ�
// �����ʱ�ȭ -> ���� ���� -> ��Ʈ��ũ ��� -> ���� �ݱ� -> ���� ����

int main(int argc, char* argv[])
{
	// ��� ���� ���α׷��� ���� �Լ��� ȣ���ϱ� ���� �ݵ�� ���� �ʱ�ȭ �Լ��� WSAStartup() �� ȣ���ؾ� �Ѵ�
	// WSAStartup() �Լ��� ���α׷����� ����� ���� ������ ��û�����ν� ���� ���̺귯��(WS2_32.DLL)�� �ʱ�ȭ�ϴ� ������ �Ѵ�
	// WSAStartup() �Լ��� ������ ��� WS2_32.DLL�� �޸𸮿� �ε���� �ʴ´�
	// �� ��쿡�� WSAGetLastError() �Լ��� �����ϴ� ���� �ڵ�� ����Ȯ�ϹǷ� WSAStartup() �Լ��� ���� ���� �ڵ带 �����ϵ��� ����Ǿ� �ִ�

	// int WSAStartup(WORD wVersionRequested, LPWSADATA lpWSAFata);  (���� : 0, ���� : ���� �ڵ�)

	// wVersionRequested : ���α׷��� �䱸�ϴ� �ֻ��� ���� �����̴� ���� 8��Ʈ�� ��(major)����, ���� 8��Ʈ�� ��(minor)������ �־ �����Ѵ�
	// ���� ��� ����3.2 ���� ����� ��û�Ѵٸ� 0x0203 �Ǵ� MAKEWORD(3,2)�� ����Ѵ�

	// lpWSAData : WSADATA ����ü�� �����ϸ� �̸� ���� ������ �ü���� �����ϴ� ���� ������ ���� ������ ���� �� �ִ�
	// (���� ���α׷��� ������ ����ϰ� �� ���� ����, �ý����� �����ϴ� ���� �ֻ��� ���� ��)
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// WSADATA ����ü�� �����ʵ� ó�� �� �� ���� ��� (���� 2.2������ 1.1������ ��µǴ� ������ �ٸ���)
	printf("%d / %d / %s / %s", wsa.wVersion, wsa.wHighVersion, wsa.szDescription, wsa.szSystemStatus);

	MessageBox(NULL, TEXT("���� �ʱ�ȭ ����"), TEXT("�˸�"), MB_OK);
	
	// ���α׷��� ������ ���� ���� ���� �Լ��� WSACleanup()�� ȣ���ؾ� �Ѵ�
	// WSACleanup() �Լ������� ����� �������� �ü���� �˸���, ���� ���ҽ��� ��ȯ�ϴ� ������ �Ѵ�
	// �Լ� ȣ���� ������ ��� WSAGetLastError() �Լ��� ȣ�������ν� ��ü���� ���� �ڵ带 ���� �� �ִ�

	// int WSACleanup(void);  (���� : 0, ���� : SOCKET_ERROR)
	WSACleanup();

	return 0;
}