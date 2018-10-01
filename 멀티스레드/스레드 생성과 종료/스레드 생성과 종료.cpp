#include <Windows.h>
#include <stdio.h>

// 실행 결과는 주 스레드, 첫 번째 스레드, 두 번째 스레드 순으로 교대로 실행되는데
// 이때 다른 프로그램을 몇 개 실행하고 돌아오면 실행 패턴이 바뀔 수 있다
// 한 프로세스 내의 스레드 실행 순서는 시스템에 존재하는 다른 스레드의 영향을 받으므로 이와 같은 변화가 생긴다


// 스레드 함수에 32비트보다 큰 값을 전달하기 위한 구조체를 정의한다
struct Point3D
{
	int x, y, z;
};

DWORD WINAPI MyThread(LPVOID arg)
{  
	// 무한 루프를 돌면서 1초마다 스레드 ID와 x, y, z를 출력한다
	Point3D* pt = (Point3D*)arg;
	
	while (1)
	{
		printf("Running MyThread() %d : %d, %d, %d\n", GetCurrentThreadId(), pt->x, pt->y, pt->z);
		Sleep(1000);
	}

	// 스레드 함수가 리턴함으로써 스레드가 종료한다, 무한 루프를 돌기 때문에 실제로 이 코드는 수행되지 않는다
	return 0;
}

int main(int argc, char* argv[])
{
	// 스레드 두 개를 생성하되, 오류가 발생하면 1(실패)을 리턴하면서
	// 응용 프로그램을 종료한다.
	// CreateThread() 함수가 리턴하는 핸들은 나중에 스레드를 별도로 조작할 필요가 없으면 가급적 빨리 닫는 것이 좋다
	// CloseHandle() 함수를 호출해 핸들을 닫는다고 해서 스레드가 종료되는 것은 아니라는 점에 유의하자

	// 첫 번째 스레드 생성
	Point3D pt1 = { 10,20,30 };

	HANDLE hThread1 = CreateThread(NULL, 0, MyThread, &pt1, 0, NULL);
	if (hThread1 == NULL)
		return 1;

	CloseHandle(hThread1);

	// 두 번째 스레드 생성
	Point3D pt2 = { 40,50,60 };

	HANDLE hThread2 = CreateThread(NULL, 0, MyThread, &pt2, 0, NULL);
	if (hThread2 == NULL)
		return 1;

	CloseHandle(hThread2);

	// 주 스레드는 무한 루프를 돌면서 1초마다 스레드 ID를 출력한다
	// 이 부분을 제거하면 주 스레드가 0을 리턴하면서 응용 프로그램, 즉 프로세스가 종료한다
	// 프로세스가 종료하면 프로세스 내부에서 실행 중인 모든 스레드는 자동으로 종료한다
	while (1)
	{
		// GetCurrentThreadId() : 스레드 식별자(ID) 를 가져온다
		printf("Running main() %d\n", GetCurrentThreadId());
		Sleep(1000);
	}

	return 0;
}


// 추가

// C/C++ 라이브러리 함수를 사용하는 응용 프로그램에서는 CreateThread(), ExitThread() API 함수보다는
// _beginthreadex(), _endthreadex() 라이브러리 함수를 사용하는 것이 바람직하다

// _beginthreadex(), _endthreadex() 함수는 CreateThread(), ExitThread()함수와 같은 역할을 하며,
// 실제로 내부적으로 이들 API를 호출한다, 차이가 있다면 C/C++ 라이브러리가 멀티스레드 환경에서 문제없이 동작하도록 부가적인 작업을 한다는 점이다

// 사용 예는 다음과 같다, API 함수와 인자의 타입만 약간 다를 뿐 순서와 의미가 같으므로 기존 코드를 거의 변경할 필요가 없다

/* #include <process.h>
   
   uintptr_t _beginthreadex(
     void* security,
	 unsingned stack_size,
	 unsingned (__stdcall* start_address) (void*),
	 void* arglist,
	 unsingend initflag,
	 unsingend* thrdaddr
   );
   
   void _endthreadex(
     unsingned retval
   );
   
*/