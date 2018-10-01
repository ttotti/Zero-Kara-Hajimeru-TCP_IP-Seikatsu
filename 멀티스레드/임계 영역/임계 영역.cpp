
// 임계 영역

// 임계 영역은 둘 이상의 스레드가 공유 자원에 접근할 때, 오직 한 스레드만 접근을 허용해야 하는 경우에 사용한다
// 임계 영역은 대표적인 스레드 동기화 기법이지만, 생성과 사용법이 달라서 "동기화 객체" 로 분류하지는 않는다

// 대표적인 특징
/*
	1. 임계 영역은 일반 동기화 객체와 달리 개별 프로세스의 유저(user)메모리 영역에 존재하는 단순한 구조체다
	   따라서 다른 프로세스가 접근할 수 없으므로 한 프로세스에 속한 스레드 간 동기화에만 사용한다

	2. 일반 동기화 객체보다 빠르고 효율적이다
*/

// 임계 영역 사용 예는 다음과 같다

/*
	#include <windows.h>

	// CRITICAL_SECTION 구조체 변수를 전역 변수로 선언한다
	// 일반 동기화 객체는 Create*() 함수를 호출해 커널 메모리 영역에 생성하지만,
	// 임계 영역은 유저 메모리 영역에 (대개는 전역 변수 형태로) 생성한다
	1. CRITICAL_SECTION cs;

	DWORD WINAPI MyThread1(LPVOID arg)
	{
		...
		// 공유 자원에 접근하기 전에 EnterCriticalSection() 함수를 호출한다
		// 공유 자원을 사용하고 있는 스레드가 없다면 EnterCriticalSection() 함수는 곧바로 리턴한다,
		// 하지만 공유 자원을 사용하고 있는 스레드가 있다면 EnterCriticalSection() 함수는 리턴하지 못하고 스레드는 대기 상태가 된다
		3. EnterCriticalSection(&cs);

		// 공유 자원 접근

		// 공유 자원 사용을 마치면 LeaveCriticalSection() 함수를 호출한다,
		// 이때 EnterCriticalSection() 함수에서 대기 중인 스레드가 있다면 하나만 선택되어 깨어난다
		4. LeaveCriticalSection(&cs);
		...
	}

	DWORD WINAPI MyThread2(LPVOID arg)
	{
		...
		3. EnterCriticalSection(&cs);
		// 공유 자원 접근
		4. LeaveCriticalSection(&cs);
		...
	}

	int main(int argc, char* argv[])
	{
		...
		// 임계 영역을 사용하기 전에 InitializeCriticalSection() 함수를 호출해 초기화 한다
		2. InitializeCriticalSection(&cs);

		// 스레드를 두 개 이상 생성해 작업을 진행한다
		// 생성한 모든 스레드가 종료할 때까지 기다린다

		// 임계 영역을 사용하는 모든 스레드가 종료하면 DeleteCriticalSection() 함수를 호출해 삭제한다
		5. DeleteCriticalSection(&cs);
		...
	}
*/

// 각 스레드는 자기만의 CPU 레지스터 값이 별도로 존재한다
// 따라서 공유 데이터가 전혀 공유되지 않고 따로 계산되다가 결과적으로 이상한 값이 나와버리는 상황이다

//#include <Windows.h>
//#include <stdio.h>
//
//#define MAXCNT 100000000
//int g_count = 0;
//
//DWORD WINAPI MyThread1(LPVOID arg)
//{
//	for (int i = 0; i < MAXCNT; i++)
//	{
//		g_count += 2;
//	}
//
//	return 0;
//}
//
//DWORD WINAPI MyThread2(LPVOID arg)
//{
//	for (int i = 0; i < MAXCNT; i++)
//	{
//		g_count -=2;
//	}
//	
//	return 0;
//}
//
//int main(int argc, char* argv[])
//{
//	HANDLE hThread[2];
//	hThread[0] = CreateThread(NULL, 0, MyThread1, NULL, 0, NULL);
//	hThread[1] = CreateThread(NULL, 0, MyThread2, NULL, 0, NULL);
//
//	WaitForMultipleObjects(2, hThread, TRUE, INFINITE);
//
//	printf("g_count = %d\n", g_count);
//
//	return 0;
//}


// 이번에는 임계 영역을 사용해 g_count 변수에 한 스레드만 접근하게 한다
// for 루프 전체를 보호할 수도 있지만 좀 더 세밀한 단위로 제어하려고 g_count 변수에 접근하는 코드 한 행만 임계 영역으로 묶었다

// 실행 결과는 올바른 값인 0을 출력한다
// 동기화로 인한 오버헤드 때문에 결과가 나오기까지 시간이 다소 걸리겠지만
// 이 예제는 극단적인 상황에서 지나치게 세밀한 단위로 스레드 동기화를 하고 있어서 성능 저하가 두드러진다

#include <Windows.h>
#include <stdio.h>

#define MAXCNT 100000000
int g_count = 0;
CRITICAL_SECTION cs;

DWORD WINAPI MyThread1(LPVOID arg)
{
	for (int i = 0; i < MAXCNT; i++)
	{
		EnterCriticalSection(&cs);
		g_count += 2;
		LeaveCriticalSection(&cs);
	}

	return 0;
}

DWORD WINAPI MyThread2(LPVOID arg)
{
	for (int i = 0; i < MAXCNT; i++)
	{
		EnterCriticalSection(&cs);
		g_count -=2;
		LeaveCriticalSection(&cs);
	}

	return 0;
}

int main(int argc, char* argv[])
{
	// 임계 영역 초기화
	InitializeCriticalSection(&cs);

	// 스레드 두 개 생성
	HANDLE hThread[2];
	hThread[0] = CreateThread(NULL, 0, MyThread1, NULL, 0, NULL);
	hThread[1] = CreateThread(NULL, 0, MyThread2, NULL, 0, NULL);

	// 스레드 두 개 종료 대기
	WaitForMultipleObjects(2, hThread, TRUE, INFINITE);

	// 임계 영역 삭제
	DeleteCriticalSection(&cs);

	printf("g_count = %d\n", g_count);

	return 0;
}