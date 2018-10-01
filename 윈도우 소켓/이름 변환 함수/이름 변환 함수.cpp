#pragma comment(lib,"ws2_32")

#include <WinSock2.h>
#include <stdio.h>

// http://pchero21.com/?p=344 출처
//말씀하신 학교 도메인의 조회가 안되는 것은 아마도 네임서버 관리자가 역방향 도메인 주소 설정을 하지 않아서 인것 같습니다.
//
//네임서버를 설정시, 원래 역방향 DNS 설정은 필수사항이 아닌 선택사항입니다.
//
//그리고 처음 네임서버를 구성할 시, 필요한 도메인(예를 들면 www 와 같은..)들에 대해서 역방향 DNS를 설정했다하더라도 후에 이 도메인의 IP 주소를 바꾸는 과정에서 역방향 DNS 주소를 설정하지 않아서 일수도 있습니다.
//
//정방향 DNS와 역방향 DNS의 주소 설정과 관련된 파일은 따로이 작성해서 운영하기 때문에 이런 현상이 발생할 수 있습니다.
//
//그래서 보통은 가장 중요한 네임서버의 IP만 역방향 조회가 가능하도록 유지를 하고 나머지는 그냥 보통은 정방향 DNS만 유지하는 것이 보통입니다.🙂
//
//그리고 네이버의 경우는 아마도 그냥 naver.com 의 주소를 입력하셨는 것 같습니다.
//
//119.205.240.165
//175.158.30.90
//119.205.240.165
//
//의 IP 주소를 입력하여 다시 실행시켜 보시면 ns1.naver.com… 등과 같은 역방향 DNS 정보를 확인하실 수 있으실 겁니다.

// IP 주소 -> 도메인 이름 변환 중 오류 발생
// 도메인 이름 -> IP 주소 변환은 성공하는데 반대는 실패한다
// gethostbyaddr() 함수가 NULL 값을 리턴한다

// IP 주소를 도메인 이름으로 바꾸는 과정은 역방향으로 조회를 하는 것이다
// 그런데 이 역방향 도메인 주소를 조회하기 위해서는 ISP에서 따로 등록을 해줘야 한다
// 예) daum.net == 211.115.77.213 이것은 조회가 가능하지만 (네임서버를 통해서)
// 211.115.77.231 == daum.net 은 조회가 불가능할 수도 있다는 것이다

//#define TESTNAME "knu.kongju.ac.kr" // 변환 성공
#define TESTNAME "www.ck.ac.kr"     // 변환 성공
//#define TESTNAME "ns1.naver.com"      // 변환 성공
//#define TESTNAME "www.naver.com"    // 변환 성공

// ----------------------------------------------------------------------------------------------

void err_display(const char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (char*)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

// 도메인 이름은 IP 주소와 마찬가지로 호스트나 라우터의 고유한 식별자이다
// www.naver.com 과 같이 문자 위주로 구성되어 있다 // IP 주소보다 기억하고 사용하기 쉽다는 장점이 있다
// TCP/IP 프로토콜은 내부적으로 숫자 형태의 IP 주소를 기반으로 동작하므로 사용자가 입력한 도메인 이름을 반드시 IP 주소로 변환해야 한다
// cmd 에서 ping 도메인 이름 을 입력하면 IPv4 또는 IPv6 주소로 변환되는 것을 보여준다

// 응용 프로그램이 도메인 이름과 IP 주소를 상호 변환할 수 있도록 윈속 함수가 제공된다

// 도메인 이름 -> IP 주소(네트워크 바이트 정렬)
/* 
struct hostent* gethostbyname(
	const char* name // 도메인 이름
);
*/

// IP 주소(네트워크 바이트 정렬) -> 도메인 이름
/*
struct hostent* gethostbyaddr(
	const char* addr,  // IP 주소(네트워크 바이트 정렬)
	int len,		   // IP 주소의 길이
	int type		   // 주소 체계(AF_INET 또는 AF_INET6)
);
*/

// getfostbyname() / gethostbyaddr() 함수는 모두 hostent 구조체형 포인터를 리턴한다

/*
typedef struct hostent{
	char* h_name;	    // 공식 도메인 이름

	char** h_aliases;   // 한 호스트가 공식 도메인 이름 외에 다른 이름을 여러 개 가질 수 있는데 이를 별명(alias name) 이라 한다 
						   호스트가 여러 별명을 가진 경우 이 포인터를 따라가면 모든 별명을 얻을 수 있다

	short h_addrtype;   // 주소 체계를 나타내는 값이다 AF_INET 또는 AF_INET6 값이 저장된다

	short h_length;     // IP 주소의 길이(바이트 단위)다 4(IPv4) 또는 16(IPv6)이 저장된다

	char** h_addr_list; // 네트워크 바이트가 정렬된 IP 주소다 한 호스트가 여러 IP 주소를 가진 경우 이 포인터를 따라가면 모든 IP 주소를 얻을 수 있다
					       특정 호스트에 접속할 때는 대개 첫 번째 IP 주소만 사용하므로 h_addr_list[0]에 접근하는데 매크로를 통해 재정의된 h_addr을 사용하면 편리하다

	#define h_addr h_addr_list[0]
} HOSTENT;
*/

// 도메인 이름 -> IPv4 주소
BOOL GetIPAddr(const char* name, IN_ADDR* addr)
{
	// gethostbyname() 함수를 호출해 HOSTENT 구조체형 포인터를 얻는다
	HOSTENT* ptr = gethostbyname(name);

	// 오류가 발생하면 오류를 출력하고 FALSE를 리턴한다
	if (ptr == NULL)
	{
		err_display(TEXT("gethostbyname()"));
		return FALSE;
	}
	// AF_INET 주소 체계가 아니면 FALSE를 리턴한다
	if (ptr->h_addrtype != AF_INET)
		return FALSE;

	// IP 주소를 복사한 후 TRUE를 리턴한다
	memcpy(addr, ptr->h_addr, ptr->h_length);
	return TRUE;
}

// IPv4 주소 -> 도메인 이름
BOOL GetDomainName(IN_ADDR addr, char* name, int namelen)
{
	// gethostbyaddr() 함수를 호출해 HOSTENT 구조체형 포인터를 얻는다
	HOSTENT* ptr = gethostbyaddr((char*)&addr, sizeof(addr), AF_INET);

	// 오류가 발생하면 오류를 출력하고 FALSE를 리턴한다
	if (ptr == NULL)
	{
		err_display(TEXT("gethostbyaddr()"));
		return FALSE;
	}
	// AF_INET 주소 체계가 아니면 FALSE를 리턴한다
	if (ptr->h_addrtype != AF_INET)
		return FALSE;

	// 도메인 이름을 복사한 후 TRUE를 리턴한다
	strncpy(name, ptr->h_name, namelen);
	return TRUE;
}

int main(int argc, char*argv[])
{
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	printf("도메인 이름(변환 전) = %s\n", TESTNAME);

	IN_ADDR addr;
	if (GetIPAddr(TESTNAME, &addr))
	{
		printf("IP 주소(변환 후) = %s\n", inet_ntoa(addr));

		char name[256];
		if (GetDomainName(addr, name, sizeof(name)))
		{
			printf("도메인 이름(다시 변환 후) = %s\n", name);
		}
	}

	WSACleanup();

	return 0;
}