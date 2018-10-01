

// 멀티스레드 TCP 서버의 기본 형태는 다음과 같다
/* DWORD WINAPI ProcessClient(LPVOID arg)
   {
       3. 전달된 소켓 저장
	   SOCKET client_sock = (SOCKET)arg;

	   4. 클라이언트 정보 얻기
	   addrlen = sizeof(clientaddr);
	   getpeername(client_sock, (SOCKADDR*)&clientaddr, &addrlen);

	   5. 클라이언트와 데이터 통신
	   while(1)
	   {
	     ...
	   }

	   ...
   }
   
   int main(int argc, char* argv[])
   {
       ...

	   while(1)
	   {
	      1. 클라이언트 접속 수용
	      client_sock = accept(lisen_sock, ...);

		  ...
		  2. 스레드 생성
		  CreateThread(NULL, 0, ProcessClient, (LPVOID)client_sock, 0, NULL);
	   }
   }*/

// 스레드 함수에 소켓만 전달한 경우에는 별도의 주소 정보가 없으므로, 소켓을 통해 주소 정보를 얻는 기능이 필요하다
// 이런 경우를 위해 다음 두 소켓 함수가 준비되어 있다

// getpeername() 함수는 소켓 데이터 구조체에 저장된 원격 IP 주소와 원격 포트 번호를 리턴한다
/* int getpeername(
     SOCKET s,			    // 소켓
	 struct sockaddr* name  // 소켓 주소 구조체
	 int* namelen			// 소켓 주소 구조체의 크기 (값 - 결과 인자(value-result argument)므로 함수 호출 전에 초기화를 해야 한다)
   )  - 성공 : 0, 실패 : SOCKET_ERROR - */

// getsockname() 함수는 지역 IP 주소와 지역 포트 번호를 리턴한다
/* int getsockname(
     SOCKET s,              // 소켓
     struct sockaddr* name  // 소켓 주소 구조체
     int* namelen			// 소켓 주소 구조체의 크기 (값 - 결과 인자(value-result argument)므로 함수 호출 전에 초기화를 해야 한다)
)  - 성공 : 0, 실패 : SOCKET_ERROR - */

#pragma comment(lib,"ws2_32")
#include <WinSock2.h>
#include <stdlib.h>
#include <stdio.h>

#define SERVERPORT 9000
#define BUFSIZE 512

void err_quit(const char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

void err_display(const char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (char*)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

// 클라이언트와 데이터 통신
// TCP 클라이언트가 보낸 데이터를 받아 화면에 표시하고 다시 보내는 역할을 하는 스레드 함수다
DWORD WINAPI ProcessClient(LPVOID arg)
{
	// 전달받은 인자를 SOCKET 타입으로 형변환하는 부분
	SOCKET client_sock = (SOCKET)arg;
	int retval;
	SOCKADDR_IN clientaddr;
	int addrlen;
	char buf[BUFSIZE + 1];

	// 클라이언트 정보 얻기
	// 클라이언트 주소 정보를 얻는 부분
	addrlen = sizeof(clientaddr);
	getpeername(client_sock, (SOCKADDR*)&clientaddr, &addrlen);

	while (1)
	{
		// 데이터 받기
		// recv()
		retval = recv(client_sock, buf, BUFSIZE, 0);
		if (retval == SOCKET_ERROR)
		{
			err_display(TEXT("recv()"));
			break;
		}
		else if (retval == 0)
			break;

		// 받은 데이터 출력
		buf[retval] = '\0';
		printf("[TCP/%s:%d] %s\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port), buf);

		// 데이터 보내기
		// send()
		retval = send(client_sock, buf, retval, 0);
		if (retval == SOCKET_ERROR)
		{
			err_display(TEXT("send()"));
			break;
		}
	}

	// closesocket()
	closesocket(client_sock);

	printf("[TCP 서버] 클라이언트 종료 : IP 주소 = %s, 포트 번호 = %d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

	return 0;
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

	// bind()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR)
		err_quit(TEXT("bind()"));

	// listen()
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR)
		err_quit(TEXT("listen()"));

	// 데이터 통신에 사용할 변수
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;
	// 스레드 핸들을 저장할 변수를 선언한다
	HANDLE hThread;

	while (1)
	{
		// accept()
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET)
		{
			err_display(TEXT("accept()"));
			break;
		}

		// 접속한 클라이언트 정보 출력
		printf("\n[TCP 서버] 클라이언트 접속 : IP 주소 = %s, 포트 번호 = %d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

		// 스레드 생성
		// 접속한 클라이언트를 처리할 스레드를 생성한다
		// 이때 스레드 함수 인자로 client_sock 값을 전달한다
		// SOCKET 타입은 크기가 32비트로, void형 포인터보다 작거나(64비트 컴파일러) 같으므로(32비트 컴파일러) 변수의 주소 대신 값 자체를 직접 전달해도 된다
		hThread = CreateThread(NULL, 0, ProcessClient, (LPVOID)client_sock, 0, NULL);
		if (hThread == NULL)
		{
			closesocket(client_sock);
		}
		else
		{
			CloseHandle(hThread);
		}
	}

	// closesocket()
	closesocket(listen_sock);

	// 윈속 종료
	WSACleanup();

	return 0;
}