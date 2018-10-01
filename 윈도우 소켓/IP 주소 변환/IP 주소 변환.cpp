
// -----------------------------------------------------------//
// 이 프로젝트는 특정 경고 사용안함에 C4996 이 포함되어 있습니다 //
//                                                           //
// 이 프로젝트는 문자집합 설정이 멀티바이트 문자 집합 입니다     //
// ----------------------------------------------------------//

#pragma comment (lib, "ws2_32")
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

// 응용 프로그램에서 IP주소를 편리하게 변환할 수 있도록 윈속 함수가 제공된다

// IPv4 주소 변환
// inet_addr() 함수는 문자열 형태로 IPv4 주소를 입력받아 32비트 숫자(네트워크 바이트 정렬)로 리턴한다
// inet_ntoa() 함수는 32비트 숫자(네트워크 바이트 정렬)로 IPv4 주소를 입력받아 문자열 형태로 리턴한다

// inet_addr() / inet_ntoa() 함수와 달리 IPv4와 IPv6 주소 변환을 모두 지원하는
// WSAStringToAddress() / WSAAddressToString() 함수도 있다 (윈도우 XP는 SP1 이상에서 IPv6 프로토콜을 설치해야 한다)

// IPv4 또는 IPv6 주소 변환
/*int WSAStringToAddress(
	LPTSTR AddressString,			   // 문자열 형식의 IP주소
	INT AddressFamily,				   // AF_INET 또는 AF_INET6
	LPWSAPROTOCOL_INFO lpProtocolInfo, // NULL
	LPSOCKADDR lpAddress,			   // IP 주소(숫자)를 저장할 구조체; SOCKADDR_IN 또는 SOCKADDR-IN6
	LPINT lpAddressLength              // 주소 구조체의 길이
*/

/*int WSAAddressToString(
	LPSOCKADDR lpsaAddress,			   // 숫자 형식의 IP 주소; SOCKADDR_IN 또는 SOCKADDR_IN6 
	DWORD dwAddressLength,			   // 주소 구조체의 길이
	LPWSAPROTOCOL_INFO lpProtocolInfo, // NULL
	LPTSTR lpszAddressString,		   // IP 주소(문자열)를 저장할 버퍼
	LPDWORD lpdwAddressStringLength    // 버퍼의 길이
*/
int main(int argc, char* argv[])
{
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// IPv4 변환
	// 원래의 IPv4 주소 출력
	const char* ipv4test = "147.46.114.70";
	printf("Ipv4 주소(변환 전) = %s\n", ipv4test);
	
	// inet_addr() 함수 연습 // 32비트 숫자(네트워크 바이트 정렬)로 변환한 결과 출력
	printf("IPv4 주소(변환 후) = 0x%x\n", inet_addr(ipv4test));

	// inet_ntoa() 함수 연습 // 다시 문자열로 변환한 결과 출력
	IN_ADDR ipv4num;
	ipv4num.s_addr = inet_addr(ipv4test);
	printf("IPv4 주소(다시 변환 후) = %s\n", inet_ntoa(ipv4num));

	printf("\n");

	// IPv6 변환
	// 원래의 IPv6 주소 출력
	const char* ipv6test = "2001:0230:abcd:ffab:0023:eb00:ffff:1111";
	printf("IPv6 주소(변환 전) = %s\n", ipv6test);

	// WSAStrToAddress() 함수 연습
	SOCKADDR_IN6 ipv6num;
	int addrlen = sizeof(ipv6num);
	WSAStringToAddress((LPSTR)ipv6test, AF_INET6, NULL, (SOCKADDR*)&ipv6num, &addrlen);
	printf("IPv6 주소(변환 후) = 0x");
	for (int i = 0; i < 16; i++)
		printf("%02x", ipv6num.sin6_addr.u.Byte[i]);

	printf("\n");

	// WSAAddressToString() 함수 연습
	char ipaddr[50];
	DWORD ipaddrlen = sizeof(ipaddr);
	WSAAddressToString((SOCKADDR*)&ipv6num, sizeof(ipv6num), NULL, ipaddr, &ipaddrlen);
	printf("IPv6 주소(다시 변환 후) = %s\n", ipaddr);

	WSACleanup();

	return 0;
}