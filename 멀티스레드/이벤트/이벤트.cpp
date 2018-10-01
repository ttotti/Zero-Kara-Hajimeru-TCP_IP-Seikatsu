

// 이벤트(Event) 는 사건 발생을 다른 스레드에 알리는 동기화 기법이다,
// 예를 들면, 한 스레드가 작업을 완료한 후 기다리고 있는 다른 스레드에 알릴 때 사용할 수 있다

// 이벤트를 사용하는 전형적인 절차는 다음과 같다
/*
	1. 이벤트를 비신호 상태로 생성한다

	2. 한 스레드가 작업을 진행하고, 나머지 스레드는 이벤트에 대해 Wait*() 함수를 호출해 이벤트가 신호 상태가 될 때까지 대기한다(Sleep)

	3. 스레드가 작업을 완료하면 이벤트를 신호 상태로 바꾼다

	4. 기다리고 있던 스레드 중 하나 혹은 전부가 깨어난다(wakeup)
*/

// 이벤트는 대표적인 동기화 객체로, 신호와 비신호 두 가지 상태를 가진다,
// 또한 상태를 변경할 수 있도록 다음과 같은 함수가 제공된다
/*
	BOOL SetEvent(HANDLE hEvent);   // 비신호 상태 -> 신호 상태 
	BOOL ResetEvent(HANDLE hEvent); // 신호 상태 -> 비신호 상태
*/

// 이벤트는 특성에 따라 다음 두 종류가 있으므로, 용도에 맞게 선택할 수 있어야 한다
/*
	자동 리셋(auto-reset)
	- 이벤트를 신호 상태로 바꾸면, 기다리는 스레드 중 하나만 깨운 후 자동으로 비신호 상태가 된다,
	  따라서 자동 리셋 이벤트에 대해서는 ResetEvent() 함수를 사용할 필요가 없다

	수동 리셋(manual-reset)
	- 이벤트를 신호 상태로 바꾸면, 기다리는 스레드를 모두 깨운 후 계속 신호 상태를 유지한다,
	  자동 리셋 이벤트와 달리 비신호 상태로 바꾸려면 명시적으로 ResetEvent() 함수를 호출해야 한다
*/

// 이벤트 생성 함수는 다음과 같다
/*
	HANDLE CreateEvent(
	  LPSECURITY_ATTRIBUTES lpEventAttributes,
	  - 핸들 상속과 보안 디스크립터 관련 구조체로, 대부분은 기본값인 NULL을 사용하면 된다

	  BOOL bManualReset,
	  - TRUE면 수동 리셋, FALSE면 자동 리셋 이벤트가 된다

	  BOOL bInitialState,
	  - TRUE면 신호, FALSE면 비신호 상태로 시작한다

	  LPCTSTR lpName
	  - 이벤트에 부여할 이름이다, NULL을 사용하면 이름 없는 이벤트가 생성되므로 같은 프로세스에 속한 스레드 간 동기화에만 사용할 수 있다
	    서로 다른 프로세스에 속한 스레드 간 동기화를 하려면 같은 이름으로 생성해야 한다,
		예를 들어, 프로세스 P1 과 프로세스 P2에 각각 속한 두 스레드가 이벤트를 이용한 동기화를 하려면
		다음과 같이 CreateEvent() 함수를 호출하면 된다, 둘 중 먼저 CreateEvent() 함수를 호출한 쪽은 이벤트를 생성하게 되고, 다른 쪽은 이벤트를 열게 된다

		   P1에 속한 스레드
		// Handle hEvent = CreateEvent(NULL, ..., "MyEventObject");
		   P2에 속한 스레드
		// Handle hEvent = CreateEvent(NULL, ..., "MyEventObject");

    );  (- 성공 : 이벤트 핸들, 실패 : NULL -)
*/

// 데이터를 생성해 공유 버퍼에 저장하는 스레드 한 개와 공유 버퍼에서 데이터를 읽어서 처리하는 스레드 두 개를 생성한다
// 이 경우 한 스레드만 버퍼에 접근할 수 있게 해야함은 물론이고 접근 순서도 정해야 한다

// 스레드 실행 순서에 대한 제약 사항은 다음과 같다
/*
	1. 스레드 1이 쓰기를 완료한 후 스레드 2나 스레드 3이 읽을 수 있다.
	   이때 스레드 2와 스레드 3 중 한 개만 버퍼 데이터를 읽을 수 있으며, 일단 한 스레드가 읽기 시작하면 다른 스레드는 읽을 수 없다

	2. 스레드2나 스레드 3이 읽기를 완료하면 스레드 1이 다시 쓰기를 할 수 있다 
*/

// 이 문제는 공유 데이터에 접근하는 순서가 중요하므로 이벤트를 사용하는 것이 좋다
// 쓰기 완료와 읽기 완료를 알릴 목적으로 이벤트 두 개(hWriteEvent, hReadEvent)를 생성해 사용할 것이다

// 실행 결과는 총 500번에 걸쳐 버퍼에 데이터를 저장할 때마다
// 두 스레드 중 하나가 대기 상태에서 깨어나 버퍼 데이터를 읽고 화면에 출력한다
// 각 출력 행마다 스레드 ID를 표시해 어느 스레드가 읽기를 완료했는지 보여주고, 이어서 버퍼(buf[])데이터를 읽어서 출력하고 있다

#include <Windows.h>
#include <stdio.h>

#define BUFSIZE 10

// HANDLE 타입 전역 변수 선언
HANDLE hReadEvent;
HANDLE hWriteEvent;
// 데이터를 저장할 buf
int buf[BUFSIZE];

// 쓰기 스레드
DWORD WINAPI WriteThread(LPVOID arg)
{
	DWORD retval;

	for (int k = 1; k <= 500; k++)
	{
		// 읽기 완료 대기
		// 읽기 이벤트(hReadEvent)가 신호 상태가 되기를 기다린다
		// 최초에는 읽기 이벤트(hReadEvent)가 신호 상태로 시작하므로 곧바로 리턴해 다음 코드로 진행할 수 있다
		retval = WaitForSingleObject(hReadEvent, INFINITE);
		if (retval != WAIT_OBJECT_0)
			break;
		
		// 공유 버퍼에 데이터 저장
		// 맨 위의 for문에서 k 값이 매번 1씩 증가하므로 읽기 스레드는 매번 서로 다른 값을 읽게 된다
		for (int i = 0; i < BUFSIZE; i++)
			buf[i] = k;

		// 쓰기 완료 알림
		// 쓰기 이벤트(hWrtieEvent)를 신호 상태로 만들어 두 읽기 스레드 중 하나를 대기 상태에서 깨운다
		// 해당 이벤트는 자동 리셋 이벤트 이므로 따로 ResetEvent() 함수를 호출하지 않아도 자동으로 비신호 상태로 바뀐다
		SetEvent(hWriteEvent);
	}

	return 0;
}

// 읽기 스레드
DWORD WINAPI ReadThread(LPVOID arg)
{
	DWORD retval;

	while (1)
	{
		// 쓰기 완료 대기
		// 쓰기 이벤트(hWriteEvent)가 신호 상태가 되기를 기다린다
		// 최초에는 쓰기 이벤트(hWriteEvent)가 비신호 상태로 시작하므로 이 지점에서 읽기 스레드는 대기 상태가 된다
		retval = WaitForSingleObject(hWriteEvent, INFINITE);
		if (retval != WAIT_OBJECT_0)
			break;

		// 읽은 데이터 출력
		// 공유 버퍼에서 데이터를 읽어 출력한다
		// GetCurrentThreadId() 함수로 얻은 스레드 ID를 앞쪽에 표시해주므로 어느 스레드가 데이터를 읽어서 처리했는지 알 수 있다
		printf("Thread %4d: ", GetCurrentThreadId());
		for (int i = 0; i < BUFSIZE; i++)
			printf("%3d ", buf[i]);
		printf("\n");

		// 버퍼 초기화
		// 공유 버퍼를 0으로 초기화 한다
		// 만약 데이터를 새로 쓰지 않은 상태에서 다시 읽게 된다면 0이 출력될 것이므로 오류 여부를 확인할 수 있다
		ZeroMemory(buf, sizeof(buf));

		// 읽기 완료 알림
		// 읽기 이벤트(hReadEvent)를 신호 상태로 만들어 쓰기 스레드를 대기 상태에서 깨운다
		// 해당 이벤트는 자동 리셋 이벤트 이므로 따로 ResetEvent() 함수를 호출하지 않아도 자동으로 비신호 상태로 바뀐다
		SetEvent(hReadEvent);
	}

	return 0;
}

int main(int argc, char* argv[])
{
	// 자동 리셋 이벤트 두 개 생성한다(각각 비신호, 신호 상태)
	// 쓰기 이벤트(hWriteEvent)는 비신호 상태로,
	// 읽기 이벤트(hReadEvent)는 신호 상태로 시작한다
	hWriteEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (hWriteEvent == NULL)
		return 1;
	hReadEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
	if (hReadEvent == NULL)
		return 1;

	// 스레드 세 개 생성
	// hThread[0]는 쓰기 스레드,
	// hThread[1]과 hThread[2]는 읽기 스레드다
	HANDLE hThread[3];
	hThread[0] = CreateThread(NULL, 0, WriteThread, NULL, 0, NULL);

	hThread[1] = CreateThread(NULL, 0, ReadThread, NULL, 0, NULL);
	hThread[2] = CreateThread(NULL, 0, ReadThread, NULL, 0, NULL);

	// 스레드 세 개 종료 대기
	// 스레드 세 개가 종료하기를 기다린다
	// 이 예제에서 읽기 스레드는 별도의 루프 탈출 조건이 없으므로 사실상 WaitForMultipleObjects() 함수는 영원히 리턴하지 못한다
	WaitForMultipleObjects(3, hThread, TRUE, INFINITE);

	// 이벤트 제거
	// 이벤트 사용을 마쳤으므로 제거한다
	CloseHandle(hWriteEvent);
	CloseHandle(hReadEvent);

	return 0;
}