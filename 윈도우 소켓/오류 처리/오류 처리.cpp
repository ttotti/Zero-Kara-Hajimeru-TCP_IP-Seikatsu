#include <WinSock2.h>

// ��Ʈ��ũ ���α׷������� ���� ���� ������ ������ �߻��� �� �ְ� ���� �߻� Ȯ���� ����
// ���� ������ üũ�Ͽ� ��ü���� ���������� �˷��ִ� ���� �ſ� �߿��ϴ�

// ���� ó�� ������� �� ���� ������ �ִµ�
// 1.������ ó���� �ʿ䰡 ���� ��� : ���� ���� ���ų� ȣ�� �� �׻� �����ϴ� �Ϻ� ���� �Լ�
// 2.���� �������� ������ ó���ϴ� ��� : WSAStartup()�Լ�
// 3.���� ������ ���� �߻��� Ȯ���ϰ�, ��ü���� ������ ���� �ڵ�� Ȯ���ϴ� ��� : ��κ��� ���� �Լ�

// 1,2�� ������ Ư���� ����̴�
// ���⼭�� 3�� ������ ó���ϴ� ����� �����Ѵ�
// ���� �Լ� ȣ�� ��� ������ �߻��ߴٸ� ������ ���� �Լ��� ����� ���� �ڵ带 ���� �� �ִ�
int WSAGetLastError(void);

// ��뿹)
if (���� �Լ�(...) == ����)
{
	int errcode = WSAGetLastError();
	printf(errcode�� �ش��ϴ� ���� �޽���);
}

// WSAGetLastError() �Լ��� ���� ���� ȭ�鿡 �״�� ǥ�ö� ��� ��밡�� ���� ���� �ڵ��� �ǹ̸� �˾Ƴ��� �ϴ� ������ ������
// ���� �ش� ���� �ڵ带 ������ ���ڿ� ���·� ����ϴ� ���� �ٶ����ϴ�

// FormatMessage() �Լ��� ����ϸ� ���� �ڵ忡 �����ϴ� ���� �޽����� ���� �� �ִ� (�� �Լ��� �ڼ��� ������ MSDN ������ �����Ѵ�)
// DWORD FormatMessage( 1.DWORD dwFlags, LPCVOID lpSource, 2.DWORD dwMessageId, 3.DWORD dwLanguageId, 4.LPTSTR lpBuffer, DWORD nSize, va_list* Arguments)

// 1. dwFlage : FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM ���� ����Ѵ�
// FORMAT_MESSAGE_ALLOCATE_BUFFER �� ���� �޽����� ������ ������ FormatMessage() �Լ��� �˾Ƽ� �Ҵ��Ѵٴ� �ǹ��̰�
// FORMAT_MESSAGE_FROM_SYSTEM�� �ü���� ���� ���� �޽����� �����´ٴ� �ǹ̴�

// 2. dwMessageId : ���� �ڵ带 ��Ÿ���� WSAGetLastError() �Լ��� ���� ���� ���⿡ �ִ´�

// 3. dwLanguageId : ���� �޽����� ǥ���� �� ��Ÿ����, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT)�� ����ϸ� ����ڰ� �����ǿ��� ������ �⺻���� ���� �޽����� ���� �� �ִ�

// 4. lpBuffer : ���� �޽����� ���� �ּҰ� ���⿡ ����ȴ�, ���� �޽����� ������ ������ FormatMessage() �� �˾Ƽ� �Ҵ��ϹǷ� ����ڴ� �ּ� ���� ������ ������
// ���⿡ �־��ָ� �ȴ�, ���� �޽��� ����� ��ġ�� LocalFree() �Լ��� ����� �ý����� �Ҵ��� �޸𸮸� ��ȯ�ؾ� �Ѵٴ� ���� �������� 

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// err_quit() �Լ��� msg���ڷ� ���޵� ���ڿ��� ���Ҿ� ���� �߻��� ���� �޽����� ȭ�鿡 �޽��� ���ڷ� ǥ���ϰ�, ���� ���α׷��� �����ϴ� ������ �Ѵ�
// ������ �߻��ϸ� �޽��� ���ڰ� ȭ�鿡 ǥ�õǰ� Ȯ�ι�ư�� ������ �������α׷��� ����ȴ�
void err_quit(LPCWSTR msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

// ��뿹)
if (socket(...) == INVALID_SOCKET) err_quit(TEXT("socket()"));
if (bind(...) == SOCKET_ERROR)err_quit(TEXT("bind()"));

// ����� ������ �߻��� ������ �����ϴ� ���� �ٶ��� ���� �ʴ� ���� �޽��� �ڽ��� �߰� Ȯ���� ������ �������α׷��� ����Ǵ� ���� �����ϴ�
// �� ��쿡��  �޽��� �ڽ� ��� printf() �Լ��� �־� ����â�� ������ ����ϰ� �ϰ� �������α׷��� ������� �ʴ´�
void err_display(char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (char*)lpMsgBuf);
	LocalFree(lpMsgBuf);
}