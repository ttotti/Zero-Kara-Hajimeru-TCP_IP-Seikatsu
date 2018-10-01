


// IPPROTO_IP, IPPROTO_IPV6 레벨 옵션

// IPPROTO_IP, IPPROTO_IPV6 레벨 옵션은 각각 IPv4, IPv6 프로토콜 코드에서 해석하고 처리한다
// 따라서 AF_INET 또는 AF_INET6 주소 체계를 사용하는 소켓에 대해서만 적용할 수 있다

// 여기서는 멀티캐스팅 관련 옵션(IP_HDRINCL / IPV6_HDRINCL, IP_TTL / IPV6_UNICAST_HOPS 를 제외한 나머지 모두)을 다룰 것이다
// 나머지 네 개(두 종류)옵션은 12장에서 소개할 것이다
// 멀티캐스팅은 개념상 TCP 소켓에는 사용할 수 없고 UDP 소켓에만 적용할 수 있다는 점을 염두에 두고 진행하자


// ---------------------------------------------------------------------------------------------------------------------------------------------


// 멀티캐스팅 개념

// 전에 잠시 언급했듯이 멀티캐스팅을 사용하면 같은 그룹에 가입한 모든 개체(물리적으로 서로 다른 네트워크에 속할 수 있음)가 서로 통신할 수 있다
// 브로드캐스팅과 마찬가지로 멀티캐스팅을 하려면 예약된 IP 주소를 사용해야 한다

// 멀티캐스팅용 IP 주소는 다음과 같이 정의되어 있다

// IPv4 주소는 224.0.0.0 ~ 239.255.255.255 범위고, 
// IPv6 주소는 FFXX:XXXX:XXXX:XXXX:XXXX:XXXX:XXXX:XXXX 형태다(X는 0 ~ F)

// 고정된 상위비트를 제외한 나머지 부분을 그룹ID라 부르며, 이 값이 통신할 대상 멀티캐스트 그룹을 나타낸다
// 응용 프로그램은 멀티캐스트 데이터를 수신하기 위해 멀티캐스트 그룹에 자유롭게 가입하고 탈퇴할 수 있다
// 소켓 옵션을 이용한 그룹 가입과 탈퇴 방법은 실습을 통해 살펴본다

// 멀티캐스트 주소 (IPv4)
/*
<------ 4비트 ------><--------- 28비트 --------->
ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ
ㅣ 1 ㅣ 1 ㅣ 1 ㅣ 0 ㅣ          그룹 ID         ㅣ
ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ
*/

// 멀티캐스트 주소(IPv6)
/*
<------ 8 비트 -----><- 4비트 -><- 4비트 -><------- 112비트 ------->
ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ
ㅣ 1 1 1 1  1 1 1 1 ㅣ  Flags  ㅣ  Scope  ㅣ       그룹 ID       ㅣ
ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ
*/

// 인터넷의 멀티캐스팅은 다음과 같은 특징이 있다

/*
1. 그룹 가입과 탈퇴가 자유롭고 그룹 구성원 모두가 평등하다
   A, B 두 호스트는 그룹 구성원에게 언제든지 멀티캐스트 데이터를 보낼 수 있다

2. 멀티캐스트 데이터를 받으려면 반드시 그룹에 가입해야 한다

3. 멀티캐스트 데이터를 보내려고 그룹에 가입할 필요는 없다
   멀티캐스트 그룹에 가입하지 않은 호스트 C 도 언제든 멀티캐스트 그룹에 데이터를 보낼 수 있다
*/

/*
ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ     ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ
ㅣ                          ㅣ     ㅣ                          ㅣ
ㅣ  A --------> 다른 구성원  ㅣ    ㅣ   A <        > 다른 구성원 ㅣ
ㅣ    .                     ㅣ     ㅣ      .     .             ㅣ
ㅣ      .                   ㅣ     ㅣ       .   .              ㅣ
ㅣ        .                 ㅣ     ㅣ        . .               ㅣ
ㅣ          > B             ㅣ     ㅣ         B                ㅣ
ㅣ                          ㅣ     ㅣ                          ㅣ
ㅡㅡㅡ   멀티캐스트 그룹  ㅡㅡㅡ     ㅡㅡㅡ  멀티캐스트 그룹  ㅡㅡㅡ
a. 그룹 구성원이 데이터를 보내는 경우


ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ
ㅣ    A <-------------------------------------ㅣ
ㅣ                  다른 구성원  ㅣ            ㅣ
ㅣ                      ^        ㅣ           ㅣ
ㅣ                      ---------------------  C (그룹에 가입되어 있지 않은 호스트)
ㅣ                              ㅣ             ㅣ
ㅣ                              ㅣ             ㅣ
ㅣ          B <--------------------------------ㅣ
ㅣ                              ㅣ
ㅡㅡㅡㅡ   멀티캐스트 그룹  ㅡㅡㅡㅡ
b. 그룹 비구성원이 데이터를 보내는 경우
*/


// ---------------------------------------------------------------------------------------------------------------------------------------------


// 멀티캐스팅 예제 동작

/*
송신자 : 사용자가 키보드로 입력한(fgets) 문자열을 멀티캐스트 주소로 보낸다(sendto)

수신자 : 멀티캐스트 그룹에 가입한다, 멀티캐스트 데이터를 받고(recvfrom), 이를 문자열로 간주해 무조건 화면에 출력한다(printf)

fgets() ------->  송신자  sendto() -------->  recvfrom()  수신자  -------> printf()

*/


// ---------------------------------------------------------------------------------------------------------------------------------------------


// 멀티캐스팅 코드 분석

// IP_MULTICAST_IF,  IPV6_MULTICAST_IF

// IP 주소를 둘 이상 보유한 호스트에서 멀티캐스트 데이터를 보낼 네트워크 인터페이스를 선택할 때 사용한다
// 예를 들면, 다음 코드는 특정 IPv4 주소(147.46.114.70)로 멀티캐스트 데이터를 보내도록 설정한 것이다,
// 이 코드를 수행하면 다음과 같이 주소가 147.46.114.70 인 네트워크 인터페이스를 통해 멀티캐스트 데이터가 전송된다
// 옵션을 별도로 설정하지 않으면 IP 라우팅 기능에 따라 각 패킷별로 적절한 네트워크 인터페이스가 선택되어 전송된다

/*
	IN_ADDR localaddr;
	localaddr.s_addr = inet_addr("147.46.114.70");
	setsockopt(sock, IPPROTO_IP, IP_MULTICAST_IF, (char*)&localaddr, sizeof(localaddr));
*/

// IP_MULTICAST_IF 옵션 설정 결과
/*
                        응용 프로그램
			              ㅣ     ㅣ
				          ㅣ     ㅣ
				          ㅣ     ㅣ
			              ㅣ     ㅣ
        147.46.114.70 <<<--      --- 147.46.115.38 
*/


// ---------------------------------------------------------------------------------------------------------------------------------------------


// IP_MULTICAST_TTL,  IPV6_MULTICAST_HOPS

// UDP 멀티캐스트 패킷은 다음과 같이 IP 헤더, UDP 헤더 그리고 응용 프로그램 데이터로 구성된다

// UDP 멀티캐스트 패킷 구성
/*
    <--  IP 헤더 --><----- UDP 헤더 ------><--------------- 응용 프로그램 데이터 ------------->   
	ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ
	ㅣ     ㅣ      ㅣ                     ㅣ                                                ㅣ
	ㅣ     ㅣ  TTL ㅣ                     ㅣ                                                ㅣ
	ㅣ     ㅣ      ㅣ                     ㅣ                                                ㅣ
	ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ
*/

// 멀티캐스트 패킷일 생성될 때 IP 헤더의 TTL(time-to-live) 필드는 기본값 1로 설정된다
// TTL은 라우터를 통과할 때마다 1씩 감소하는데, 0이 되면 패킷이 버려진다
// 따라서 TTL = 1 은 멀티캐스트 패킷이 라우터 경계를 넘어갈 수 없음을 뜻한다
// 라우터 경계를 넘어 특정 범위까지 IPv4 멀티캐스트 패킷을 보내려면 IP_MULTICAST_TTL 옵션을 이용해 TTL 값을 변경해야 한다

// IPv6 에서는 TTL 대신 Hop Limit라는 용어를 사용하는데 개념은 같다
// 라우터 경계를 넘어 특정 범위까지 IPv6 멀티캐스트 패킷을 보내려면 IPV6_MULTICAST_HOPS 옵션을 이용해 Hop Limit 값을 변경해야한다

// multicastSender 와 MulticastSender_IPv6 에서 TTL 값을 변경하는 코드는 각각 다음과 같다

// IPv4
/*
	int ttl = 2;
	retval = setsockopt(sock,IPPROTO_IP, IP_MULTICAST_TTL, (char*)&ttl, sizeof(ttl));
	if(retval == SOCKET_ERROR) err_quit("setsockopt()");
*/

// IPv6
/*
	int tti = 2;
	retval = setsockopt(sock, IPPROTO_IPV6, IPV6_MULTICAST_HOPS, (char*)&ttl, sizeof(ttl));
	if(retval == SOCKET_ERROR) err_quit("setsockopt()");
*/


// ---------------------------------------------------------------------------------------------------------------------------------------------


// IP_MULTICAST_LOOP,  IPV6_MULTICAST_LOOP

// 멀티캐스트 그룹에 가입한 응용 프로그램이 자신의 그룹에 멀티캐스트 데이터를 보내면 자신도 받는다(기본 동작)
// 다음과 같이 IP*_MULTICAST_LOOP 옵션을 이용하면 기본 동작을 바꿀 수 있다

/*
	BOOL optval = FALSE;
	setsockopt(sock, IPPROTO_IP, IP_MULTIVAST_LOOP (char*)&optval, sizeof(optval));
*/


// ---------------------------------------------------------------------------------------------------------------------------------------------


// IP_ADD_MEMBERSHIP / IP_DROP_MEMBERSHIP,  IPV6_ADD_MEMBERSHIP / IPV6_DROP_MEMBERSHIP

// 멀티캐스트 데이터를 받으려면 반드시 멀티캐스트 그룹에 가입해야 한다, 

// IP*_ADD_MEMBERSHIP 옵션은 그룹에 가입할 때, 
// IP*_DROP_MEMBERSHIP 옵션은 그룹에서 탈퇴할 때 필요하다

// IP_ADD_MEMBERSHIP / IP_DROP_MEMBESHIP 옵션은 ip_mreq 구조체를
// IPV6_ADD_MEMBERSHIP / IPV6_DROP_MEMBESHIP 옵션은 ipv6_mreq 구조체를 옵션으로 사용한다

/*
	#include <ws2tcpip.h>

	typedef struct ip_mreq
	{
		IN_ADDR imr_multiaddr;  // 가입하거나 탈퇴할 IPv4 멀티캐스트 주소다 // IPv4 multicast address

		IN_ADDR imr_interface;  // 멀티캐스트 패킷을 받을 네트워크 인터페이스의 IPv4 주소다 
								// INADDR_ANY(0으로 정의)를 전달해 자동으로 선택되게 하는 것이 일반적이다 // Local IP address of interface
	} IP_MREQ;

	typedef struct ipv6_mreq
	{
		IN6_ADDR ipv6mr_multiaddr;  // 가입하거나 탈퇴할 IPv6 멀티캐스트 주소다 // IPv6 multicast address

		ULONG    ipv6mr_interface;  // 멀티캐스트 패킷을 받을 네트워크 인터페이스의 인덱스다
								    // 0을 전달해 자동을 선택되게 하는 것이 일반적이다// Interface index
	} IPV6_MREQ;

*/

// IP_ADD_MEMBERSHIP 옵션을 이용해 특정 네트워크 인터페이스(147.46.114.70)를 멀티캐스트 그룹(235.7.8.9)에 가입시키는 코드는 다음과 같다
// 이 코드를 수행하면 다음과 같이 주소가 147.46.114.70 인 네트워크 인터페이스가 멀티캐스트 데이터를 받는다

/*
	struct ip_mreq mreq;
	mreq.imr_multiaddr.s_addr = inet_addr(235.7.8.9);
	mreq.imr_interface.s_addr = inet_addr(147.46.114.70);
	setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&mreq, sizeof(mreq));
*/

// IP_ADD_MEMBERSHIP 옵션 설정 결과
/*
							  응용 프로그램
							    ^      ㅣ
								^      ㅣ
								ㅣ     ㅣ
								ㅣ     ㅣ
								ㅣ     ㅣ
			  147.46.114.70 -----      --- 147.46.115.38
*/

// MulticastReceiver 와 MulticastReceiver_IPv6 멀티캐스트 그룹에 가입하고 탈퇴하는 코드는 각각 다음과 같다
// 가입할 때 사용한 구조체 변수(mreq)를 탈퇴할 때 재사용함을 알 수 있다

// IPv4
/*
	// 멀티캐스트 그룹 가입
	struct ip_mreq mreq;
	mreq.imr_multiaddr.s_addr  inet_addr(MULTICASTIP);
	mreq.imr_interface.s_addr = htonl(INADDR_ANY);
	retval = setsockopt(sock, IPPROTO_IP, IPADD_MEMBERSHIP,(char*)&mreq, sizeof(mreq));
	if(retval == SOCKET_ERROR) err_quit("setsockopt()");

	// 멀티캐스트 그룹 탈퇴
	retval = setsockopt(sock, IPPROTO_IP, IP_DROP_MEMBERSHIP, (char*)&mreq, sizeof(mreq));
	if(retval == SOCKET_ERROR) err_qUit()
*/

// IPv6
/*
	// 주소 변환 (문자열 -> IPv6)
	SOCKADDR_IN6 tmpaddr;
	int addrlen = sizeof(tmpaddr);
	WSAStringToAddress(MULTICASTIP, AF_INET6, NULL,(SOCKADDR*)&tmpaddr, &addrlen);

	// 멀티캐스트 그룹 가입
	struct ipv6_mreq mreq;
	mreq.ipv6mr_multiaddr = tmpaddr.sin6_addr;
	mreq.ipv6mr_interface = 0;
	retval = setsockopt(sock, IPPROTO_INV6, IPV6_ADD_MEMBERSHIP, (char*)&mreq, sizeof(mreq));
	if(retval == SOCKET_ERROR) err_quit("setsockopt()");

	// 멀티캐스트 그룹 탈퇴
	retval = setsockopt(sock, IPPROTO_INV6, IPV6_DROP_MEMBERSHIP, (char*)&mreq, sizeof(mreq));
	if(retval == SOCKER_ERROR) err_quit("setsockopt()");
*/


// ---------------------------------------------------------------------------------------------------------------------------------------------


// TCP 에만 적용 가능한 옵션

// IPPROTO_TCP 레벨 옵션

// IPPROTO_TCP 레벨 옵션은 TCP 프로토콜 코드에서 해석하고 처리한다
// 따라서 TCP 소켓에만 적용할 수 있다
// 여기서는 TCP_NODELAY 옵션만 소개한다

// TCP_NODELAY 옵션은 Nagle(네이글) 알고리즘 작동을 중지하는 역할을 한다
// Nagle 알고리즘은 작은 패킷이 불필요하게 많이 생성되는 일을 방지해 네트워크 트래픽을 감소시키는 알고리즘이다
// TCP 전송 효율과 네트워크 활용도를 높인다는 장점이 있기 때문에 TCP 프로토콜에서는 기본으로 동작한다
// 따라서 일반적으로는 TCP_NODELAY 옵션을 설정하지 않는것이 바람직하지만, 몇 가지 이유로 TCP_NODELAY 옵션이 필요한 경우가 있다

// http://www.ietf.org/rfc/rfc896.txt
// Nalge 알고리즘을 이해하려면 TCP의 데이터 전송 원리를 먼저 알아야 한다
// TCP는 신뢰성 있는 데이터 전송 기법 중 하나로 ACK(acknowledgement)에 기반한 데이터 재전송을 한다
// 즉, 데이터를 보낸 후 상대방이 잘 받았는지 ACK 패킷을 통해 확인하고, 그렇지 않을 경우 재전송한다

// 참고로 TCP는 마지막으로 성공적으로 수신한 데이터 번호에 1을 더한 값을 ACK 번호로 보낸다
// 다음 그림에서 데이터 n에 대해 ACK n+1 로 응답하는 것은 이 때문이다

// 다음과 같이 데이터를 보내고 ACK를 받고, 다시 다음 데이터를 보내고 ACK를 받는 과정을 반복함을 보여준다
/*
		ㅣ       데이터 전송         ㅣ
		ㅣ ---------------------->  ㅣ
		ㅣ                          ㅣ
		ㅣ <----------------------  ㅣ
		ㅣ         ACK 2            ㅣ
		ㅣ                          ㅣ
		ㅣ       데이터 전송         ㅣ
		ㅣ ---------------------->  ㅣ
		ㅣ                          ㅣ
		ㅣ <----------------------  ㅣ
		ㅣ         ACK 3            ㅣ
*/
// TCP 데이터 전송 원리 (1) : ACK르 이용한 데이터 수신 확인

// 그런데 데이터를 보낼 때마다 ACK가 도착할 때까지 대기하면 네트워크 대역폭(bandwidth) 활용도가 낮아지는 문제가 생긴다
// TCP에서는 네트워크 자원을 효율적으로 활용하려고 슬라이딩 원도우(sliding window) 라는 방식을 사용한다
// 이 방식에서는 ACK를 받지 못해도 윈도우 크기만큼 데이터를 계속 보낼 수 있으므로 성능을 높일 수 있다
// 여기에 추가적으로 각 데이터에 대해 상대편이 ACK로 응답할 것을 요구하면 성능이 떨어지기 때문에
// 마지막으로 성공적으로 받은 데이터에 대해서만 ACK로 응답하는 방식을 사용한다

// 다음에서 ACK 4는 데이터 1부터 데이터 3까지 잘 받았다는 뜻으로 응답하는 것이다

/*
		ㅣ         데이터 1          ㅣ
		ㅣ  ----------------------> ㅣ
		ㅣ         데이터 2          ㅣ
		ㅣ  ----------------------> ㅣ
		ㅣ         데이터 3          ㅣ
		ㅣ  ----------------------> ㅣ
		ㅣ                          ㅣ
		ㅣ          ACK 4           ㅣ
		ㅣ  <---------------------- ㅣ
		ㅣ                          ㅣ
		ㅣ  ----------------------> ㅣ
		ㅣ         데이터 4          ㅣ
		ㅣ           ...            ㅣ
*/
// TCP 데이터 전송 원리 (2) : 슬라이딩 윈도우를 이용한 전송 효율 높임



// Nagle 알고리즘의 동작 방식은 다음 두 가지로 요약할 수 있다
/*
	1. 보낼 데이터가 MSS(maximum segment size)로 정의된 크기만큼 쌓이면 상대편에 무조건 보낸다
	   이 경우 슬라이딩 윈도우 방식으로 데이터를 계속 보낼 수 있다

	2. 보낼 데이터가 MSS보다 작으면 이전에 보낸 데이터에 대한 ACK가 오기를 기다린다
	   ACK가 도착하면 보낼 데이터가 MSS보다 작더라도 상대편에 보낸다
	   이 경우 데이터를 전송할 때마다 ACK를 기다리고, ACK를 받으면 다음 데이터를 전송하는 동작을 반복한다
*/
// 이와 같은 동작 방식을 간단히 요약하면
/*
	데이터가 충분히 크면 곧바로 보내고, 그렇지 않으면 데이터가 쌓일 때까지 대기한다
	단, 데이터가 충분히 쌓이지 않았더라도 이전에 보낸 데이터를 상대편이 받았다면 다음 데이터를 보낸다
*/



// Nagle 알고리즘의 장단점은 다음과 같다
/*
	장점 : 작은 패킷이 불필요하게 많이 생성되는 일을 방지해 네트워크 트래픽을 감소시킨다

	단점 : 데이터가 충분히 쌓일 때까지 또는 ACK가 도달할 때까지 대기하는 시간때문에
	       응용 프로그램의 반응 시간(response time)이 길어질 수 있다
*/



// TCP_NODELAY 옵션은 이와 같은 Nagle 알고리즘의 장점을 포기하는 대신
// 응용 프로그램의 반응 속도를 빠르게 할 때 사용한다

// TCP_NODELAY 옵션 사용 예는 다음과 같다
/*
	BOOL optval = TRUE;  // Nagle 알고리즘 중지

	setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char*)&optval, sizeof(optval));
*/



// 추가
// Nagle 알고리즘과 네트워크 환경

// LAN(Local Area Network)에서는 Nagle 알고리즘이 작동하더라도 크게 문제가 되지 않는다
// 그러나 WAN(Wide Area Network)에서는 패킷 전송 시간이 길어지므로 Nagle 알고리즘으로 인한 응용 프로그램의 반응 시간 지연이 뚜렷하게 나타날 수 있다
// 이런 경우에는 응용 프로그램 수준에서 데이터 전송 방식을 수정하는 등의 방법으로 대처하는 것이 바람직하며
// TCP_NODELAY 옵션 사용은 최후의 수단으로 고려해야 한다
