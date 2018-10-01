#pragma comment(lib,"ws2_32")
#include <WinSock2.h>
#include <stdlib.h>
#include <stdio.h>

#define SERVERIP "127.0.0.1"
#define SERVERPORT 9000
#define BUFSIZE 512

// 소켓 함수 오류 출력 후 종료
void err_quit(const char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

// 소켓 함수 오류 출력
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

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// sock()
	SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock == INVALID_SOCKET)
		err_quit(TEXT("socket()"));

	// 소켓 주소 구조체 초기화
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
	serveraddr.sin_port = htons(SERVERPORT);

	// 데이터 통신에 사용할 변수
	SOCKADDR_IN peeraddr;
	int addrlen;
	char buf[BUFSIZE + 1];
	int len;

	// 서버와 데이터 통신
	while (1)
	{
		// 데이터 입력
		printf("\n[보낼 데이터] ");
		if (fgets(buf, BUFSIZE + 1, stdin) == NULL)
			break;

		// '\n' 문자 제거
		len = strlen(buf);
		if (buf[len - 1] == '\n')
			buf[len - 1] = '\0';
		if (strlen(buf) == 0)
			break;

		// 데이터 보내기
		// sendto() : 응용 프로그램 데이터를 운영체제의 송신 버퍼에 복사함으로써 데이터를 전송한다

		// UDP의 경우, sendto() 함수를 호출할 때 소켓의 지역 IP 주소와 지역 포트 번호가 아직 결정되지 않은 상태라면 운영체제가 자동으로 결정해준다
		// 즉, sendto() 함수가 TCP의 bind() 함수 역할을 대신한다

		// sendto() 함수 원형
		/*
			int sendto(
			SOCKET s,                  // 통신에 사용할 소켓이다

			const char* buf,           // 보낼 데이터를 담고 있는 응용 프로그램 버퍼의 주소다

			int len,                   // 보낼 데이터 크기(바이트 단위)다

			int flags,                 // sendto() 함수의 동작을 바꾸는 옵션으로, 대부분 0을 사용하면 된다
										  사용 가능한 값으로 MSG_DONTROUTE(윈속에서는 사용하더라도 무시됨)와 MSG_OOB(책 10장 참고, UDP에서는 의미 없음)가 있다

			const struct sockaddr* to, // 목적지 주소를 담고 있는 소켓 주소 구조체다
									      UDP의 경우, 특정 호스트나 라우터 주소는 물론이고 "브로드캐스트"나 "멀티캐스트" 주소를 사용할 수도 있다

			int tolen                  // 목적지 주소를 담고 있는 소켓 주소 구조체의 크기(바이트 단위)다
			);    - 성공 : 보낸 바이트 수, 실패 : SOCKET_ERROR -
		*/

		// sendto() 함수 사용 예는 다음과 같다
		/*
			// 소켓 주소 구조체를 수신자의 IP 주소와 포트 번호로 초기화한다
			SOCKADDR_IN serveraddr;

			...

			// 송신용 버퍼를 선언하고 데이터를 넣는다
			char buf[BUFSIZE];

			...

			// sendto() 함수로 데이터를 보낸다
			retval = sendto(sock, buf, strlen(buf), 0, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
			if(retval == SOCKET_ERROR) 오류 처리
			printf("%d 바이트를 보냈습니다.\n", retval);
		*/

		// sendto() 함수와 관련해 주의할 사항을 정리하면 다음과 같다
		/*
			1. sendto() 함수는 UDP 소켓은 물론이고 TCP 소켓에도 사용할 수 있으며, 이 경우 to와 tolen 인자는 무시된다, 
			   TCP 소켓에 사용할 때만 flags 인자에 MSG_OOB를 사용할 수 있다

			2. sendto() 함수로 보낸 데이터는 독립적인 UDP 데이터그램(=패킷)으로 만들어져 전송되며,
			   수신 측에서는 recvfrom() 함수 호출 한 번으로 이 데이터를 읽을 수 있다
			   따라서 UDP를 사용할 경우에는 TCP와 달리 응용 프로그램 수준에서 메시지 경계를 구분하는 작업을 할 필요가 없다

		    3. UDP 소켓에 대해 sendto() 함수를 호출할 경우 한 번에 보낼 수 있는 데이터의 크기에 제한이 있다
			   최솟값은 0, 최댓값은 65507(65535-20(IP 헤더 크기) - 8(UDP 헤더 크기)) 바이트다

		    4. sendto() 함수로 보낸 응용 프로그램 데이터는 커널(=운영체제) 영역에 복사되어 전송된 후 곧바로 버려진다
			   sendto() 함수가 리턴했다고 실제 데이터 전송이 완료된 것은 아니며,
			   데이터 전송이 끝났어도 상대방이 받았는지 확인할 수는 없다
		*/
		//retval = sendto(sock, buf, strlen(buf), 0, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
		if (retval == SOCKET_ERROR)
		{
			err_display(TEXT("sendto()"));
			continue;
		}
		printf("[UDP 클라이언트] %d바이트를 보냈습니다.\n", retval);

		// 데이터 받기
		// recvfrom() : 운영체제의 수신 버퍼에 도착한 데이터를 응용 프로그램 버퍼에 복사한다

		// TCP의 recv() 함수와 다른 점은, UDP 패킷 데이터를 한 번에 하나만 읽을 수 있다는 점이다
		// 즉, 응용 프로그램 버퍼를 크게 잡는다고 많은 데이터를 한꺼번에 읽을 수 없다

		// recvfrom() 함수 원형
		/*
			int recvfrom(
				SOCKET s,              // 통신에 사용할 소켓이다
									      sendto() 함수에 사용하는 소켓과 달리, 이 소켓은 반드시 지역주소(IP 주소, 포트 번호)가 미리 결정되어 있어야 한다

				char* buf,             // 받은 데이터를 저장할 응용 프로그램 버퍼의 주소다

				int len,               // 응용 프로그램 버퍼의 크기(바이트 단위)다
				                          도착한 UDP 패킷 데이터가 len보다 크면 len 만큼만 복사하고 나머지는 버린다, 이때 recvfrom() 함수는 SOCKET_ERROR 를 리턴한다, 
										  따라서 예상되는 UDP 패킷 데이터의 최대 크기를 감안해 응용 프로그램 버퍼를 준비해두어야 한다

				int flags,             // recv() 함수의 동작을 바꾸는 옵션으로, 대부분 0을 사용하면 된다
				                          사용 가능한 값으로 MSG_PEEK와 MSG_OOB(책 10장을 참조, UDP에서는 의미 없음)가 있다
										  recvfrom() 함수의 기본 동작은 수신 버퍼의 데이터를 응용 프로그램 버퍼에 복사한 후 
										  해당 데이터를 수신 버퍼에서 삭제하는 것이지만 MSG_PEEK 옵션을 사용하면 수신 버퍼에 데이터가 계속 남는다

				struct sockaddr* from, // 소켓 주소 구조체를 전달하면 송신자의 주소 정보(IP 주소와 포트 번호)로 채워진다

				int* fromlen           // 정수형 변수를 from이 가리키는 소켓 주소 구조체의 크기로 초기화한 후 전달한다
				                          recvfrom() 함수가 리턴하면 *fromlen 변수는 recvfrom() 함수가 채워넣은 주소 정보의 크기(바이트 단위)를 갖게 된다
			);  - 성공 : 받은 바이트 수, 실패 : SOCKET_ERROR -
		*/

		// recvfrom() 함수 사용 예는 다음과 같다
		/*
			// 통신 상대의 주소를 저장할 변수를 선언한다
			SOCKADDR_IN peeraddr;
			int addrlen;

			...

			// 수신용 버퍼를 선언한다
			char buf[BUFSIZE];

			...

			// recvfrom() 함수로 데이터를 받는다
			addrlen = sizeof(peeraddr);
			retval = recvfrom(sock, buf, BUFSIZE, 0, (SOCKADDR*)&peeraddr, &addrlen);
			if(retval == SOCKET_ERROR) 오류 처리
			printf("%d 바이트를 받았습니다\n", retval);
		*/

		// recvfrom() 함수와 관련해 주의할 사항을 정리하면 다음과 같다
		/*
			1. recvfrom() 함수는 UDP 소켓은 물론이고 TCP 소켓에도 사용할 수 있으며, 이 경우 from과 fromlen 인자는 무시된다.
			   TCP 소켓에 사용할 때만 flags 인자에 MSG_OOB를 사용할 수 있다

			2. sendto() 함수로 보낸 데이터는 독립적인 UDP 데이터 그램(=패킷)으로 만들어져 전송되며,
			   수신 측에서는 recvfrom() 함수 호출 한 번으로 이 데이터를 읽을 수 있다
			   따라서 UDP를 사용할 경우에는 TCP와 달리 응용 프로그램 수준에서 메시지 경계를 구분하는 작업을 할 필요가 없다

			3. UDP 소켓에 대해 recvfrom() 함수를 호출할 경우 리턴 값이 0이 될 수 있는데,
			   이는 상대방이 sendto() 함수 호출 시 데이터 크기를 최솟값인 0으로 설정했다는 뜻이다.
			   UDP 프로토콜에는 연결 설정과 종료 개념이 없으므로 recvfrom() 함수의 리턴 값이 0이라고 해서 특별한 의미가 있는 것은 아니다
			   반면 TCP 소켓에 대해 recvfrom() 함수를 호출할 경우 리턴 값이 0이면 정상 종료를 의미한다

			4. 블로킹 소켓을 사용할 경우, 소켓 수신 버퍼에 도착한 데이터가 없으면 recvfrom() 함수는 호출 시 블록된다
		*/

		// *추가*
		// recvfrom() 함수에 사용하는 소켓은 반드시 지역 주소(IP 주소, 포트 번호)가 미리 결정되어 있어야 한다
		// 즉 recvfrom() 함수를 호출하기 전에 어디선가 bind() 함수 또는 이에 상응하는 함수를 사용해 지역 주소를 설정하는 과정이 필요하다
		/*
			// 명시적으로 bind() 함수를 호출했으므로 전혀 문제가 없다
			bind(sock, ...);
			...
			recvfrom(sock, ...);

			// sendto() 함수는 bind() 함수 역할도 하기 때문에, sendto() 함수를 먼저 호출한 경우 문제가 없다
			sendto(sock, ...);
			...
			recvfrom(sock, ...);
		*/
		addrlen = sizeof(peeraddr);
		retval = recvfrom(sock, buf, BUFSIZE, 0, (SOCKADDR*)&peeraddr, &addrlen);
		if (retval == SOCKET_ERROR)
		{
			err_display(TEXT("recvfrom()"));
			continue;
		}

		// 송신자의 IP 주소 체크
		if (memcmp(&peeraddr, &serveraddr, sizeof(peeraddr)))
		{
			printf("[오류] 잘못된 데이터입니다!\n");
			continue;
		}

		// 받은 데이터 출력
		buf[retval] = '\n';
		printf("[UDP 클라이언트] %d바이트를 받았습니다.\n", retval);
		printf("[받은 데이터] %s\n", buf);
	}

	// closesocket()
	closesocket(sock);

	// 윈속 종료
	WSACleanup();

	return 0;
}