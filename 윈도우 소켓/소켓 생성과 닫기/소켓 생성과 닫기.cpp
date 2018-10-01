#pragma comment(lib,"ws2_32")
#include <WinSock2.h>

// ���� ó��
void err_quit(LPCWSTR msg)
{
	LPVOID lpMsgBuf;

	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);

	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);

	LocalFree(lpMsgBuf);

	exit(1);
}

int main(int argc, char* argv[])
{
	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	MessageBox(NULL, TEXT("���� �ʱ�ȭ ����"), TEXT("�˸�"), MB_OK);

	// ���� �����ϱ�
	// ������ ����� ����ϱ� ���� �⺻ ����� ��� ����� ���� ���������� ����ϴ� ���̴�(TCP�� ����� ������ UDP�� ����� �������� ����ؾ� �Ѵ�)
	// socket()�Լ��� ����ڰ� ��û�� ���������� ����� ����� �� �ֵ��� ���������� ���ҽ��� �Ҵ��ϰ� �̿� ������ �� �ִ� ������ �ڵ� ��(SOCKET Ÿ��, 32��Ʈ ����)�� �����Ѵ�
	// �� ���� ���� ��ũ����(socket descriptor)�� �θ��� ���� ���� �Լ��� ȣ���� �� ���ڷ� ������ ����Ѵ�

	// SOCKET socket(int af, int type, int protocol)  (���� : ���ο� ����, ���� : INVALID_SOCKET)
	// af : �ּ� ü�踦 �����Ѵ�
	// type : ���� Ÿ���� �����Ѵ�
	// protocol : ����� ���������� �����Ѵ�

	// �ּ� ü�� - ����� �Ϸ��� ��� ��븦 �����ϰ� ������ �� �ִ� �ּҰ� �ʿ��ѵ� �̷��� �ּ� ���� ����� ��Ī�ϴ� ����
	// �ּ� ü��� ��Ʈ��ũ ���������� ������ ���� �޶����Ƿ� �ּ� ü�������� �ڽ��� ����� ���������� �����ϱ� ���� ù ��° �����̴�
	// winsock2.h �Ǵ� ws2def.h ������ ã�ƺ��� AF_ �� �����ϴ� ����� ã�� �� �ִµ�
	// �ڽ��� ����� �������ݿ� �����ϴ� ���� ������ socket() �Լ��� ù ��° ���ڷ� �����ϸ� �ȴ�
	// ���� ��� IPv4 ��� TCP�� UDP ���������� ����Ϸ��� AF_INET ���� �����Ѵ� (IPv6 ��� - AF_INET6)

	// ���� Ÿ�� - ����� ���������� Ư���� ��Ÿ���� ���̴�
	// ���� ����ϴ� ���� Ÿ���� ������ ����
	// SOCK_STREAM - �ŷڼ� �ִ� ������ ���� ��� ����, ������ �������� (TCP �� ���� ����)
	// SOCK_DGRAM - �ŷڼ� ���� ������ ���� ��� ����, �񿬰��� �������� (UDP �� ���� ����)
	// ���� Ÿ���� ��Ʈ��ũ ���������� ������ ���� �޶����Ƿ� ���� Ÿ�� ������ �ڽ��� ����� ���������� �����ϱ� ���� �� ��° �����̴�

	// �������� - �ּ� ü��� ���� Ÿ�Ը����� ���������� ������ �� �ִ� ��찡 �ִ�
	// �׷��� �Ϲ������δ� �ּ� ü��� ���� Ÿ���� ���Ƶ� �̿� �ش��ϴ� ���������� �� �� �̻� ������ �� �ִ�
	// �̶��� ���������� ��������� �����ؾ� �ϴµ� socket() �Լ��� �� ��° ���ڰ� �̷� ������ �Ѵ�
	// IPPROTO_TCP - TCP�� ���� ����
	// IPPROTO_UDP - UDP�� ���� ����
	// ������ TCP�� UDP ���������� �ּ� ü��� ���� Ÿ�Ը����� ���������� ������ �� �����Ƿ� �밳�� �������� �κп� 0�� ����Ѵ�
	SOCKET tcp_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (tcp_sock == INVALID_SOCKET) 
		err_quit(TEXT("tcp_socket()"));

	MessageBox(NULL, TEXT("TCP ���� ���� ����"), TEXT("�˸�"), MB_OK);

	// UDP ���� ����
	// socket() �Լ��� �⺻ ������ ������ �� �� ���� ����� �����ϴ� WSASocket() �Լ��� �ִ�(�ڼ��� ������ ���ͳ� ����)
	SOCKET udp_sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (udp_sock == INVALID_SOCKET)
		err_quit(TEXT("udp_socket()"));

	MessageBox(NULL, TEXT("UDP ���� ���� ����"), TEXT("�˸�"), MB_OK);

	// ������ ����� ����� ��ġ�� ���� ���ҽ��� ��ȯ�ؾ� �Ѵ� closesocket()�Լ��� �ش� ������ �ݰ� ���� ���ҽ��� ��ȯ�Ѵ�
	// int closesocket(SOCKET s)  (���� : 0, ���� : SOCKET_ERROR)
	closesocket(udp_sock);
	closesocket(tcp_sock);

	// ���� �ݱ�
	WSACleanup();

	return 0;
}