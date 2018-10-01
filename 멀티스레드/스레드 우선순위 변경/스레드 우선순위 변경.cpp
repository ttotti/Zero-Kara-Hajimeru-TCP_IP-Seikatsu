#include <Windows.h>
#include <stdio.h>

// 스레드 제어
// 스레드는 윈도우 운영체제의 실행 단위므로, 우선순위를 변경하거나 실행을 중지하고 재시작 하는 등의 제어 기능을 윈도우API 수준에서 지원한다,

// 스레드 우선순위 변경하기

// 윈도우 운영체제에서는 항상 여러 스레드가 CPU 시간을 사용하려고 경쟁한다
// 따라서 각 스레드에 CPU 시간을 적절히 분배하기 위한 정책을 사용하는데,
// 이를 "스레드 스케줄링" 또는 "CPU 스케줄링" 이라 부른다

// 윈도우 운영체제의 스케줄링 기법은 우선순위에 기반한 것으로,
// 우선순위가 높은 스레드에 우선적으로 CPU 시간을 할당한다

// 스레드의 우선순위를 결정하는 요소는 다음과 같다

// 프로세스 우선순위 : "우선순위 클래스" 라 부른다
// 스레드 우선순위   : "우선순위 레벨" 이라 부른다

// 우선순위 클래스는 프로세스 속성으로, 한 프로세스가 생성한 스레드는 우선순위 클래스가 모두 같다는 특징이 있다
// 윈도우 운영체제에서 제공하는 우선순위 클래스는 다음과 같다

/* REALTIME_PRIORITY_CLASS 
   (실시간)

   HIGH_PRIORITY_CLASS 
   (높음)

   ABOVE_NORMAL_PRIORITY 
   (높은 우선순위; 윈도우 2000 이상)

   NORMAL_PRIORITY_CLASS 
   (보통)

   BELOW_NORMAL_PRIORITY_CLASS 
   (낮은 우선순위; 윈도우 2000 이상)

   IDLE_PRIORITY_CLASS 
   (낮음)*/

// 우선순위 레벨은 스레드 속성으로, 같은 프로세스에 속한 스레드 간 상대적인 우선순위를 결정할 때 사용한다
// 원도우 운영체제에서 제공하는 우선순위 레벨은 다음과 같다

/* THREAD_PRIORITY_TIME_CRITICAL
   THREAD_PRIORITY_HIGHEST
   THREAD_PRIORITY_ABOVE_NORMAL
   THREAD_PRIORITY_NORMAL
   THREAD_PRIORITY_BELOW_NORMAL
   THREAD_PRIORITY_LOWEST
   THREAD_PRIORITY_IDLE*/

// 우선순위 클래스와 우선순위 레벨을 결합하면 스레드의 기본 우선순위가 결정되고, 이 값이 스레드 스케줄링에 사용된다
// 윈도우의 스케줄링 방식에서는 우선순위가 가장 높은 스레드에 CPU 시간을 할당하되,
// 우선순위가 같은 스레드가 여러 개 있을 때는 CPU 시간을 번갈아가며 할당한다

// 이런 방식에서는 우선순위가 높은 스레드가 계속 CPU 시간을 요구함녀 우선순위가 낮은 스레드는 CPU 시간을 전혀 할당받지 못하는 문제가 발생한다
// 이를 기아(starvation) 라고 한다
// 기아 문제를 해결하려고 윈도우 운영체제는 오랜 시간 CPU 시간을 할당받지 못한 스레드의 우선순위를 단계적으로 끌어올려서 우선순위가 낮은 스레드도 궁극적으로 CPU를 사용할 수 있게 한다

// 또한 현재 사용자가 작업하고 있는 프로그램의 반응 속도를 빠르게 하려고 우선순위를 동적으로 변경하기도 한다
// 예를 들면, 같은 프로그램이 두 개 실행 중일 때 사용자가 작업하고 있는 포그라운드 프로그램의 스레드가 더 높은 우선순위를 받는다

// 멀티스레드를 이용할 때 작업의 중요도에 따라 응용 프로그램이 직접 우선순위를 변경하기도 한다
// 이때 우선순위 클래스를 변경하는 경우는 흔치 않으며, 대개는 우선순위 레벨을 변경한다

// 우선순위 레벨 관련 API 함수는 다음과 같다

// 우선순위 레벨을 변경할 때
/* BOOL SetThreadPriority(
     HAMDLE hThread, // 스레드 핸들
	 int nPriority   // 우선순위 레벨
   )   - 성공 : 0이 아닌 값, 실패 : 0 - */

// 우선순위 레벨을 얻을 때
/* int GetThreadPriority(
     HANDLE hThread  // 스레드 핸들
   )  - 성공 : 우선순위 레벨, 실패 : THREAD_PRIORITY_ERROR_RETURN - */

DWORD WINAPI MyThread(LPVOID arg)
{
	// 무한 루프를 돌면서 CPU 시간을 계속 요구하므로
	// 우선순위가 고정되어 있다면 MyThread 스레드만 CPU 시간을 할당받게 된다
	// 이때 기아가 발생할 수 있으나, 윈도우 운영체제에서는 오랜 시간 CPU 시간을 할당받지 못한 스레드의 우선순위를 단계적으로 끌어올리기 때문에
	// main 스레드도 실행할 기회를 얻는다
	while (1);
	return 0;
}

int main()
{
	// CPU 개수를 알아낸다
	SYSTEM_INFO si;
	GetSystemInfo(&si);

	// CPU 개수만큼 스레드를 생성한다
	// CPU 개수만큼 생성한 MyThread 스레드의 우선순위 레벨을 main 스레드보다 높게 설정한다
	for (int i = 0; i < (int)si.dwNumberOfProcessors; i++)
	{
		HANDLE hThread = CreateThread(NULL, 0, MyThread, NULL, 0, NULL);
		if (hThread == NULL)
			return 1;

		// 최고 우선순위로 변경한다 (실시간)
		SetThreadPriority(hThread, THREAD_PRIORITY_TIME_CRITICAL);
		CloseHandle(hThread);
	}

	Sleep(1000);

	while (1)
	{
	    // 윈도우 운영체제에서는 오랜 시간 CPU 시간을 할당받지 못한 스레드의 우선순위를 단계적으로 끌어올리기 때문에
		// main 스레드도 실행할 기회를 얻는다
		printf("주 스레드 실행!\n");
		break;
	}

	return 0;
}