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
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
	serveraddr.sin_port = htons(SERVERPORT);
	// connect()
	retval = connect(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR)
		err_quit(TEXT("connect()"));

	// 데이터 통신에 사용할 변수
	char buf[BUFSIZE + 1];
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
		// send() 함수는 데이터 복사가 성공하면 곧바로 리턴한다, 따라서 send() 함수가 리턴했다고 실제 데이터가 전송된 것은 아니며 일정 시간이 지나야만 하부 프로토콜(TCP/IP 등)을 통해 전송이 완료된다
		retval = send(sock, buf, strlen(buf), 0);
		if (retval == SOCKET_ERROR)
		{
			err_display(TEXT("send()"));
			break;
		}
		printf("[TCP 클라이언트] %d바이트를 보냈습니다.\n", retval);

		// 데이터 받기
		// recv() : 운영체제의 수신 버퍼에 도착한 데이터를 응용 프로그램 버퍼에 복사한다
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
