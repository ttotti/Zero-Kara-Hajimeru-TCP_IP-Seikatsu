#pragma comment(lib, "ws2_32")
#include <winsock2.h>
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

// 사용자 정의 데이터 수신 함수
// recvn() 함수 : 사용자 정의 함수로 recv() 함수와 형태가 같고 recv() 함수로 구현한 기존 코드를 손쉽게 recvn() 함수로 대체할 수 있다
int recvn(SOCKET s, char* buf, int len, int flags)
{
	// 내부적으로 호출하는 recv() 함수의 리턴 값을 저장할 변수다
	int received;
	// 포인터 변수 ptr은 응용 프로그램 버퍼의 시작 주소를 가리킨다, 데이터를 읽을 때마다 ptr 값은 증가한다
	char* ptr = buf;
	// left 변수는 아직 읽지 않은 데이터 크기다, 데이터를 읽을 때마다 left 값은 감소한다
	int left = len;

	// 아직 읽지 않은 데이터가 있으면 계속 루프를 돈다
	while (left > 0)
	{
		// recv() 함수를 호출해 오류가 발생하면 곧바로 리턴한다
		received = recv(s, ptr, left, flags);
		if (received == SOCKET_ERROR)
			return SOCKET_ERROR;
		// recv() 함수의 리턴 값이 0이면(정상 종료), 상대가 데이터를 더 보내지 않을 것이므로 루프를 빠져나간다
		else if (received == 0)
			break;
		// recv() 함수가 성공한 경우므로 left와 ptr 변수를 갱신한다
		left -= received;
		ptr += received;
	}

	// 읽은 바이트 수를 리턴한다, 오류가 발생하거나 상대가 접속을 종료한 경우가 아니면 left변수는 항상 0이므로 리턴 값은 len이 된다
	return (len - left);
}

int main(int argc, char* argv[])
{
	int retval;

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
		err_quit(TEXT("socket()"));

	// connect() : TCP 프로토콜 수준에서 서버와 논리적 연결을 설정한다

	/* int connect(
		SOCKET s,					 // 서버와 통신할 목적으로 만든 소켓이다
		const struct sockaddr* name, // 소켓 주소 구조체(TCP/IP의 경우 SOCKADDR_IN 또는 SOCKADDR_IN6)를 서버 주소(즉, 원격 IP 주소와 원격 포트 번호)로 초기화하여 전달한다
		int namelen					 // 소켓 주소 구조체의 길이(바이트 단위)다
	)   - 성공 : 0, 실패 : SOCKET_ERROR - */

	// 일반적으로 클라이언트는 서버와 달리 bind() 함수를 호출하지 않는다, bind() 함수를 호출하지 않은 상태에서 connect() 함수를 호출하면,
	// 운영체제가 자동으로 지역 IP 주소와 지역 포트 번호를 할당해준다 (이때 자동으로 할당되는 포트 번호는 윈도우 버전에 따라 다를 수 있다)

	// 소켓 주소 구조체 변수를 0으로 초기화 하고 ,IP 주소와 포트 번호를 대입한다
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
	serveraddr.sin_port = htons(SERVERPORT);
	// connect() 함수를 호출하고 오류를 처리한다
	retval = connect(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR)
		err_quit(TEXT("connect()"));

	// 데이터 통신에 사용할 변수
	// 보낼 데이터 또는 받은 데이터를 저장할 버퍼다
	char buf[BUFSIZE + 1];
	// 사용자가 입력한 문자열 데이터의 길이를 계산할 때 사용한다
	int len;

	// 서버와 데이터 통신
	while (1)
	{
		// 데이터 입력
		// fgets() 함수를 사용해 사용자로부터 문자열을 입력받는다
		printf("\n[보낼 데이터] ");
		if (fgets(buf, BUFSIZE + 1, stdin) == NULL)
			break;

		// '\n' 문자 제거
		// '\n' 문자를 제거한다, 데이터 출력 시 줄바꿈 여부 혹은 줄바꿈 방식을 서버가 결정하게 하기 위함이다
		len = strlen(buf);
		if (buf[len - 1] == '\n')
			buf[len - 1] = '\0';
		// '\n' 문자를 제거한 후 문자열 길이가 0이면, 사용자가 글자를 입력하지 않고 곧바로 Enter 키를 눌렀다는 뜻이다
		// 이 경우 루프를 빠져나가고 closesocket() 함수를 호출해 접속을 정상 종료한다
		if (strlen(buf) == 0)
			break;

		// 데이터 보내기
		// send() : 응용 프로그램 데이터를 운영체제의 송신 버퍼에 복사함으로써 데이터를 전송한다
		//		    send() 함수는 데이터 복사가 성공하면 곧바로 리턴한다, 따라서 send() 함수가 리턴했다고 실제 데이터가 전송된 것은 아니며 일정 시간이 지나야만 하부 프로토콜(TCP/IP 등)을 통해 전송이 완료된다

		/* int send(
			SOCKET s,       // 통신할 대상과 연결된 소켓이다
			const char* buf // 보낼 데이터를 담고 있는 응용 프로그램 버퍼의 주소다
			int len,		// 보낼 데이터 크기(바이트 단위)다
			int flags		// send() 함수의 동작을 바꾸는 옵션으로, 대부분 0을 사용하면 된다
							   사용 가능한 값으로 MSG_DONTROUTE(윈속에서는 사용하더라도 무시됨)와 MSG_OOB(거의 사용 안 함)가 있다
		)   - 성공 : 보낸 바이트 수,  실패 : SOCKET_ERROR - */

		// send() 함수는 첫 번째 인자로 전달하는 소켓의 특성에 따라 다음과 같이 두 종류의 성공적인 리턴을 할 수 있다
		/* 블로킹 소켓 : 지금 까지 생성한 소켓은 모두 블로킹 소켓이다
						블로킹 소켓을 대상으로 send() 함수를 호출하면, 송신 버퍼의 여유 공간이 send() 함수의 세 번째 인자인 len보다 작을 경우 해당 프로세스는 대기 상태가 된다
						송신 버퍼에 충분한 공간이 생기면 프로세스는 깨어나고 len 크기만큼 데이터 복사가 일어난 후 send() 함수가 리턴한다 이 경우 send() 함수의 리턴 값은 len과 같다*/

		/* 넌블로킹 소켓 : ioctlsocket() 함수를 이용하면 블로킹 소켓을 넌블로킹 소켓으로 바꿀 수 있다, 
						  넌블로킹 소켓을 대상으로 send() 함수를 호출하면, 송신 버퍼의 여유 공간만큼 데이터를 복사한 후 실제 복사한 바이트 수를 리턴한다, 
						  이 경우 send() 함수의 리턴 값은 최소 1, 최대 len 이다*/
		
		// send() 함수를 호출하고 오류를 처리한다
		// 블로킹 소켓을 사용하고 있으므로 send() 함수의 리턴 값은 strlen(buf) 값과 같을 것이다
		retval = send(sock, buf, strlen(buf), 0);
		if (retval == SOCKET_ERROR)
		{
			err_display(TEXT("send()"));
			break;
		}
		printf("[TCP 클라이언트] %d바이트를 보냈습니다.\n", retval);

		// 데이터 받기
		// recv() : 운영체제의 수신 버퍼에 도착한 데이터를 응용 프로그램 버퍼에 복사한다

		/* int recv(
			SOCKET s,  // 통신할 대상과 연결된 소켓이다

			char* buf, // 받은 데이터를 저장할 응용 프로그램 버퍼의 주소다

			int len,   // 운영체제의 수신 버퍼로부터 복사할 최대 데이터 크기(바이트 단위)다
					      이 값은 buf가 가리키는 응용 프로그램 버퍼보다 크지 않아야 한다

			int flags  //  recv() 함수의 동작을 바꾸는 옵션으로, 대부분 0을 사용하면 된다
					       사용 가능한 값으로 MSG_PEEK와 MSG_OOB(거의 사용 안 함)가 있다
						   recv() 함수의 기본 동작은 수신 버퍼의 데이터를 응용 프로그램 버퍼에 복사한 후 해당 데이터를 수신 버퍼에서 삭제하는 것이다, 
						   하지만 MSG_PEEK 옵션을 사용하면 수신 버퍼에 데이터가 계속 남는다
		)   - 성공 : 받은 바이트 수 또는 0(연결 종료 시), 실패 : SOCKET_ERROR */

		// recv() 함수는 다음 두 종류의 성공적인 리턴을 할 수 있다
		/* 수신 버퍼에 데이터가 도달한 경우 : recv() 함수의 세 번째 인자인 len보다 크지 않은 범위에서 가능하면 많은 데이터를 응용 프로그램 버퍼에 복사한 후 실제 복사한 바이트 수를 리턴한다
											이 경우 recv() 함수의 리턴 값은 최소 1, 최대 len이다*/

		/* 접속이 정상 종료한 경우 : 상대편 응용 프로그램이 closesocket() 함수를 호출해 접속을 종료함녀, TCP 프로토콜 수준에서 접속 종료를 위한 패킷 교환 절차가 일어난다
									이 경우 recv() 함수는 0을 리턴한다, recv() 함수의 리턴 값이 0인 경우를 정상 종료라 부른다*/

		// recv() 함수 사용 시 특히 주의할 점은 세 번째 인자인 len으로 저장한 크기보다 적은 데이터가 응용 프로그램 버퍼에 복사될 수 있다는 사실이다
		// 이는 TCP가 데이터 경계를 구분하지 않는다는 특성에 기인한다
		// 따라서 자신이 받을 데이터의 크기를 미리 알고 있다면 그만큼 받을때까지 recv() 함수를 여러 번 호출해야 한다
		// 본문 예제에서는 사용자 정의 함수 recvn() - 30줄 을 정의해서 편리하게 처리하고 있다

		// recvn() 함수를 호출하고 오류를 처리한다
		// 서버로 부터 받을 데이터의 크기를 미리 알고 있으므로 사용자 정의 함수 recvn() 을 사용했다
		retval = recvn(sock, buf, retval, 0);
		if (retval == SOCKET_ERROR)
		{
			err_display(TEXT("recv()"));
			break;
		}
		else if (retval == 0)
			break;

		// 받은 데이터 출력
		// 받은 데이터 끝에 '\0'을 추가하여 화면에 출력한다
		buf[retval] = '\0';
		printf("[TCP 클라이언트] %d바이트를 받았습니다.\n", retval);
		printf("[받은 데이터] %s\n", buf);
	}

	// closesocket()
	closesocket(sock);

	// 윈속 종료
	WSACleanup();

	return 0;
}

