#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>

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

	// socket()
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET)
		err_quit(TEXT("socket()"));

	// bind() : 소켓의 지역 IP 주소와 지역 포트 번호를 결정한다
	/*int bind(
		SOCKET s,					 // 클라이언트 접속을 수용할 목적으로 만든 소켓, 지역 IP 주소와 지역 포트 번호가 아직 결정되지 않은 상태다
		const struct sockaddr* name, // 소켓 주소 구조체(TCP/IP의 경우 SOCKADDR_IN 또는 SOCKADDR_IN6)를 지역 IP 주소와 지역 포트 번호로 초기화 하여 전달한다
		int namelen					 // 소켓 주소 구조체의 길이(바이트 단위)다
	)   - 성공 : 0, 실패 : SOCKET_ERROR - */ 

	// 소켓 주소 구조체 변수를 선언하고 0으로 초기화 한다
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	// 인터넷 주소 체계(IPv4)를 사용한다는 의미로 AF_INET을 대입한다
	serveraddr.sin_family = AF_INET;
	// 서버의 지역 IP 주소를 설정한다
	// 서버의 경우 특정 IP 주소 대신 INADDR_ANY(0으로 정의됨) 값을 사용하는 것이 바람직하다
	// 서버가 IP 주소를 두 개 이상 보유한 경우 INADDR_ANY 값을 지역 주소로 설정하면 클라이언트가 어느 IP 주소로 접속하든 받아들일 수 있다
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	// 서버의 지역 포트 번호를 설정한다(예를 들면 9000번), htons() 함수를 이용해 바이트 정렬을 변경한 값을 대입해야 한다
	serveraddr.sin_port = htons(SERVERPORT);
	// bind() 함수를 호출하고 오류를 처리한다, 두 번째 인자는 항상 (SOCKADDR*)형으로 변환해야 한다
	retval = bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR)
		err_quit(TEXT("bind()"));

	// listen() : 소켓의 TCP 포트 상태를 LISTENING 으로 바꾼다, (이는 클라이언트 접속을 받아들일 수 있는 상태가 됨을 의미한다)
	/*int listen(
		SOCKET s,   // 클라이언트 접속을 수용할 목적으로 만든 소켓으로, bind() 함수로 지역 IP 주소와 지역 포트 번호를 설정한 상태다
		int backlog // 서버가 당장 처리하지 않더라도 접속 가능한 클라이언트의 개수다
					   클라이언트의 접속 정보는 연결 큐에 저장되는데, backlog는 이 연결 큐의 길이를 나타낸다, 
					   하부 프로토콜에서 지원 가능한 최댓값을 사용하려면 SOMAXCONN 값을 대입한다
	)   - 성공 : 0, 실패 : SOCKET_ERROR - */

	// backlog를 최댓값으로 하여 listen() 함수를 호출하고 오류를 처리한다
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR)
		err_quit(TEXT("listen()"));

	// 데이터 통신에 사용할 변수
	// accept() 함수의 리턴 값을 저장할 소켓을 선언한다
	SOCKET client_sock;
	// accept() 함수의 두 번째 인자로 사용한다, (accept() 함수가 리턴하면 접속한 클라이언트의 IP 주소와 포트 번호가 여기에 저장된다)
	SOCKADDR_IN clientaddr;
	// accept() 함수의 세 번째 인자로 사용한다
	int addrlen;
	// 받은 데이터를 저장할 응용 프로그램 버퍼다
	char buf[BUFSIZE + 1];

	// 일반적으로 서버는 계속 클라이언트 요청을 처리해야 하므로 무한 루프를 돈다
	while (1)
	{
		// accept() : 접속한 클라이언트와 통신할 수 있도록 새로운 소켓을 생성해서 리턴한다
		//			  또한 접속한 클라이언트의 주소 정보(서버 입장에서는 원격 IP 주소와 원격 포트 번호, 클라이언트 입장에서는 지역IP 주소와 지역 포트 번호) 도 알려준다

		/*SOCKET accept(
			SOCKET s,              // 클라이언트 접속을 수용할 목적으로 만든 소켓으로, 
								      bind() 함수로 지역 IP 주소와 지역 포트 번호를 설정하고 listen() 함수로 TCP 포트 상태를 LISTENING 으로 변경한 상태다
			struct sockaddr* addr, // 소켓 주소 구조체를 전달하면 접속한 클라이언트의 주소 정보(IP 주소와 포트 번호)로 채워진다
			int* addrlen		   // 정수형 변수를 addr이 가리키는 소켓 주소 구조체의 크기로 초기화한 후 전달한다
									  accept() 함수가 리턴하면 *addrlen 변수는 accept() 함수가 채워넣은 주소 정보의 크기(바이트 단위)를 갖게 된다
		)  - 성공 : 새로운 소켓, 실패 : INVALID_SOCKET - */

		// 접속한 클라이언트가 없을 경우 accept() 함수는 서버를 대기 상태(WAIT_STATE 또는 SUSPENDED_STATE)로 만든다
		// 이때 작업관리자를 실행해 CPU 사용률을 확인하면 0으로 표시된다, 클라이언트가 접속하면 서버는 깨어나고 accept() 함수는 비로소 리턴하게 된다

		// accept() 함수의 세 번째 인자로 전달할 정수형 변수 addrlen을 소켓 주소 구조체 변수(clientaddr)의 크기로 초기화 한다
		addrlen = sizeof(clientaddr);
		// accept() 함수를 호출하고 오류를 처리한다
		// 이전에 사용한 소켓 함수와 달리 오류가 발생하면 err_display() 함수를 이용해 화면에 구체적인 오류 메시지를 표시하고 무한 루프를 탈출한다
		client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET)
		{
			err_display(TEXT("accept()"));
			break;
		}

		// 접속한 클라이언트 정보 출력
		// 접속한 클라이언트의 IP 주소와 포트 번호를 화면에 출력한다 (inet_ntoa()와 ntohs() 함수 사용)
		printf("\n[TCP 서버] 클라이언트 접속: IP 주소 = %s, 포트 번호 = %d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

		// 클라이언트와 데이터 통신
		// accept() 함수가 리턴한 소켓을 이용해 클라이언트와 통신한다
		// recv() 함수의 리턴 값이 0(정상종료) 또는 SOCKET_ERROR(오류 발생)가 될 때까지 계속 루프를 돌며 데이터를 수신한다
		while (1)
		{
			// 데이터 받기
			// recv() 함수를 호출하고 오류를 처리한다, 클라이언트로부터 받을 데이터 크기를 미리 알 수 없으므로 사용자 정의 recvn()함수는 사용할 수 없다
			retval = recv(client_sock, buf, BUFSIZE, 0);
			if (retval == SOCKET_ERROR)
			{
				err_display(TEXT("recv()"));
				break;
			}
			else if (retval == 0)
				break;

			// 받은 데이터 출력
			// 받은 데이터 끝에 '\0' 을 추가해 화면에 출력한다
			buf[retval] = '\0';
			printf("[TCP\%s:%d] %s\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port), buf);

			// 데이터 보내기
			// send() 함수를 호출하고 오류를 처리한다
			retval = send(client_sock, buf, retval, 0);
			if (retval == SOCKET_ERROR)
			{
				err_display(TEXT("send()"));
				break;
			}
		}

		// closesocket()
		// 클라이언트와 통신을 마치면 소켓을 닫고, 접속을 종료한 클라이언트의 IP 주소와 포트 번호를 화면에 출력한다
		closesocket(client_sock);
		printf("[TCP 서버] 클라이언트 종료: IP 주소 = %s, 포트 번호 = %d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
	}

	// closesocket()
	closesocket(listen_sock);

	// 윈속 종료
	WSACleanup();

	return 0;
}