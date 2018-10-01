
// -----------------------------------------------------------//
// �� ������Ʈ�� Ư�� ��� �����Կ� C4996 �� ���ԵǾ� �ֽ��ϴ� //
//                                                           //
// �� ������Ʈ�� �������� ������ ��Ƽ����Ʈ ���� ���� �Դϴ�     //
// ----------------------------------------------------------//

#pragma comment (lib, "ws2_32")
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

// ���� ���α׷����� IP�ּҸ� ���ϰ� ��ȯ�� �� �ֵ��� ���� �Լ��� �����ȴ�

// IPv4 �ּ� ��ȯ
// inet_addr() �Լ��� ���ڿ� ���·� IPv4 �ּҸ� �Է¹޾� 32��Ʈ ����(��Ʈ��ũ ����Ʈ ����)�� �����Ѵ�
// inet_ntoa() �Լ��� 32��Ʈ ����(��Ʈ��ũ ����Ʈ ����)�� IPv4 �ּҸ� �Է¹޾� ���ڿ� ���·� �����Ѵ�

// inet_addr() / inet_ntoa() �Լ��� �޸� IPv4�� IPv6 �ּ� ��ȯ�� ��� �����ϴ�
// WSAStringToAddress() / WSAAddressToString() �Լ��� �ִ� (������ XP�� SP1 �̻󿡼� IPv6 ���������� ��ġ�ؾ� �Ѵ�)

// IPv4 �Ǵ� IPv6 �ּ� ��ȯ
/*int WSAStringToAddress(
	LPTSTR AddressString,			   // ���ڿ� ������ IP�ּ�
	INT AddressFamily,				   // AF_INET �Ǵ� AF_INET6
	LPWSAPROTOCOL_INFO lpProtocolInfo, // NULL
	LPSOCKADDR lpAddress,			   // IP �ּ�(����)�� ������ ����ü; SOCKADDR_IN �Ǵ� SOCKADDR-IN6
	LPINT lpAddressLength              // �ּ� ����ü�� ����
*/

/*int WSAAddressToString(
	LPSOCKADDR lpsaAddress,			   // ���� ������ IP �ּ�; SOCKADDR_IN �Ǵ� SOCKADDR_IN6 
	DWORD dwAddressLength,			   // �ּ� ����ü�� ����
	LPWSAPROTOCOL_INFO lpProtocolInfo, // NULL
	LPTSTR lpszAddressString,		   // IP �ּ�(���ڿ�)�� ������ ����
	LPDWORD lpdwAddressStringLength    // ������ ����
*/
int main(int argc, char* argv[])
{
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// IPv4 ��ȯ
	// ������ IPv4 �ּ� ���
	const char* ipv4test = "147.46.114.70";
	printf("Ipv4 �ּ�(��ȯ ��) = %s\n", ipv4test);
	
	// inet_addr() �Լ� ���� // 32��Ʈ ����(��Ʈ��ũ ����Ʈ ����)�� ��ȯ�� ��� ���
	printf("IPv4 �ּ�(��ȯ ��) = 0x%x\n", inet_addr(ipv4test));

	// inet_ntoa() �Լ� ���� // �ٽ� ���ڿ��� ��ȯ�� ��� ���
	IN_ADDR ipv4num;
	ipv4num.s_addr = inet_addr(ipv4test);
	printf("IPv4 �ּ�(�ٽ� ��ȯ ��) = %s\n", inet_ntoa(ipv4num));

	printf("\n");

	// IPv6 ��ȯ
	// ������ IPv6 �ּ� ���
	const char* ipv6test = "2001:0230:abcd:ffab:0023:eb00:ffff:1111";
	printf("IPv6 �ּ�(��ȯ ��) = %s\n", ipv6test);

	// WSAStrToAddress() �Լ� ����
	SOCKADDR_IN6 ipv6num;
	int addrlen = sizeof(ipv6num);
	WSAStringToAddress((LPSTR)ipv6test, AF_INET6, NULL, (SOCKADDR*)&ipv6num, &addrlen);
	printf("IPv6 �ּ�(��ȯ ��) = 0x");
	for (int i = 0; i < 16; i++)
		printf("%02x", ipv6num.sin6_addr.u.Byte[i]);

	printf("\n");

	// WSAAddressToString() �Լ� ����
	char ipaddr[50];
	DWORD ipaddrlen = sizeof(ipaddr);
	WSAAddressToString((SOCKADDR*)&ipv6num, sizeof(ipv6num), NULL, ipaddr, &ipaddrlen);
	printf("IPv6 �ּ�(�ٽ� ��ȯ ��) = %s\n", ipaddr);

	WSACleanup();

	return 0;
}