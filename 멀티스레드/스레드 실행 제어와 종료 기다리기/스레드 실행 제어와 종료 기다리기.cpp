#include <Windows.h>
#include <stdio.h>

// 스레드 종료 기다리기

// 스레드는 일단 생성되면 CPU 시간을 사용하려고 다른 스레드와 경쟁하면서 독립적으로 실행된다
// 하지만 때로는 한 스레드가 다른 스레드의 종료 여부, 즉 작업 완료 여부를 확인해야 할 때가 생긴다
// WaitForSingleObject() 함수를 사용하면 특정 스레드가 종료할 때까지 기다릴 수 있다

// WaitForSingleObject() 함수 사용 예는 다음과 같다

/* HANDLE hThread = CreateThread(...);
   DWORD retval = WaitForSingleObject(hThread, 1000);
   if(retval == WAIT_OBJECT_0) {...}     // 스레드 종료
   else if(retval == WAIT_TIMEOUT) {...} // 타임아웃(스레드는 아직 종료 안함)
   else {...}							 // 에러 발생
 */

// 여러 스레드가 종료하기를 기다리려면 WaitForSingleObject() 함수를 스레드 개수만큼 호출해야 하는데,
// 대신 WaitForMultipleObjects() 함수를 사용하면 호출 한 번으로 끝낼 수 있다

// 지정된 개체 중 하나 또는 모두가 신호를 받은 상태가되거나 시간 제한 간격이 경과 할 때까지 대기합니다
/* DWORD WaitForMultipleObjects(
     DWORD nCount,
	 const HANDLE* lpHandles,
	 // WaitForMultipleObjects() 함수를 사용할 때는 스레드 핸들을 배열에 넣어서 전달해야 한다
	    nCount는 배열 원소 개수, lpHandles는 배열의 시작 주소를 나타낸다
		nCount의 최댓값은 MAXIMUM_WAIT_OBJECTS(=64)로 정의되어 있다

	 BOOL bWaitAll. // TRUE면 모든 스레드가 종료할 때까지 기다린다, FALSE면 한 스레드가 종료하는 즉시 리턴한다

	 DWORD dwMilliseconds // 사용법과 의미는 WaitForSingleObject() 함수의 두 번째 인자와 같다
   )  - 성공 : WAIT_OBJECT_0 ~ WAIT_OBJECT_0 + nCount -1 또는 WAIT_TIMEOUT,  실패 : WAIT_FAILED*/

// WaitForMultipleObjects() 함수 사용 예는 다음과 같다
/* 
   // 모든 스레드의 종료를 기다린다
   HANDLE hThread[2];
   hThread[0] = CreateThread(...);
   hThread[1] = CreateThread(...);

   WaitForMultipleObjects(2, hThread, TRUE, INFINITE);
 */

/* 
   // 스레드 하나의 종료를 기다린다
   HANDLE hThread[2];
   hThread[0] = CreateThread(...);
   hThread[1] = CreateThread(...);

   DWORD retval = WaitForMultipleObjects(2, hThread, FALSE, INFINITE);

   switch(retval)
   {
	  case WAIT_OBJECT_0:     // hThread[0] 종료
		break;
	  case WAIT_OBJECT_0 + 1: // hThread[1] 종료
	    break;
	  case WAIT_FAILED:		  // 오류 발생
	    break;
   }
 */

// 참고로 WaitForSingleObject()와 WaitForMultipleObjects() 함수는 스레드 종료를 기다리는 전용 함수가 아니라는 점에 주의하자
// 두 함수는 모두 스레드 동기화 에 사용하는 범용 함수로, 여기서는 단지 스레드 종료를 기다리는 목적으로 이용한 것뿐이다


// 스레드 실행 중지와 재시작하기

// 스레드 핸들을 보유하고 있으면 SuspendThread() 함수를 호출해 해당 스레드 실행을 일시 중지하거나 ResumeThread() 함수를 호출해 재시작할 수 있다
// 윈도우 운영체제는 스레드의 중지 횟수(suspend count)를 관리하는데,
// 이 값은 SuspendThread() 함수를 호출할 때마다 1씩 증가하고 ResumeThread() 함수를 호출할 때마다 1씩 감소한다
// 중지 횟수가 0보다 크면 스레드는 실행 중지 상태에 있게 된다
// 따라서 한 스레드에 대해 SuspendThread() 함수를 두 번 호출했다면 ResumeThread() 함수를 두 번 호출해야 재시작할 수 있다

/* DWORD SuspendThread(
     HANDLE hThread // 스레드 핸들
   );  - 성공 : 중지 횟수, 실패 : -1 */

/* DWORD ResumeThread(
     HANDLE hThread // 스레드 핸들
   );  - 성공 : 중지 횟수, 실패 : -1 */

// 비슷한 기능을 제공하는 것으로 Sleep() 함수가 있다
// SuspendThread() 함수를 호출한 경우에는 반드시 ResumeThread() 함수를 사용해야 스레드가 재시작하지만,
// Sleep() 함수를 호출하면 dwMillseconds로 지정한 시간이 지나면 자동으로 재시작한다는 차이가 있다

// Sleep(0) 를 호출하면 스레드는 자신에게 할당된 CPU 시간을 포기하고 남은 시간을 우선순위가 같은 다른 스레드에 넘겨준다
// 이를 이용하면 스레드 간 컨텍스트 전환을 빠르게 할 수 있다

/* void Sleep(
    DWORD dwMilliseconds // 밀리초(ms)
   );*/


// 실행 결과는 스레드 실행 전에는 결과가 틀리지만 실행 후에는 올바른 결과를 보여주고 있다
// 스레드를 이용해 1부터 100까지 합을 구한다,
// 연습을 위해 스레드를 정지 상태로 시작한 후 ResumeThread() 함수를 사용해 재시작하게 할 것이다
// 또한 스레드가 계산을 완료했는지 확인하려고 WaitForSingleObject() 함수를 사용할 것이다

int sum = 0;

DWORD WINAPI MyThread(LPVOID arg)
{
	int num = (int)arg;

	for (int i = 1; i <= num; i++)
		sum += i;

	return 0;
}

int main(int argc, char* argv[])
{
	int num = 100;

	// 스레드 생성
	// 5번째 인자에 CREATE_SUSPENDED를 사용하면 스레드가 생성은 되지만 ResumeThread() 함수를 호출하기 전까지 실행되지 않는다
	HANDLE hThread = CreateThread(NULL, 0, MyThread, (LPVOID)num, CREATE_SUSPENDED, NULL);
	if (hThread == NULL)
		return 1;

	printf("스레드 실행 전, 계산 결과 = %d\n", sum);

	// 스레드 재시작
	ResumeThread(hThread);

	// 지정한 개체가 신호를 받거나 제한 시간이 경과 할 때까지 대기합니다
	/* DWORD WatiForSingleObject(
	     HANDLE hHandle,       // 종료를 기다릴 대상 스레드를 나타낸다

		 DWORD dwMilliseconds, 
		 // 대기 시간으로, 밀리초 단위를 사용한다
		    이 시간 안에 스레드가 종료하지 않으면 WaitForSinfleObject() 함수는 리턴하고, 이때 리턴 값은 WAIT_TIMEOUT이 된다.
			스레드가 종료한 경우에는 WAIT_OBJECT_0을 리턴한다,
			대기 시간으로 INFINITE 값을 사용하면 스레드가 종료할 때까지 무한히 기다린다
	   )  - 성공 : WAIT_OBJECT_0 또는 WAIT_TIMEOUT, 실패 : WAIT_FAILED*/

	// hThread 스레드가 실행을 종료할 때까지 기다린다
	WaitForSingleObject(hThread, INFINITE);

	//Sleep(1000);

	printf("스레드 실행 후, 계산 결과 = %d\n", sum);

	CloseHandle(hThread);

	return 0;
}