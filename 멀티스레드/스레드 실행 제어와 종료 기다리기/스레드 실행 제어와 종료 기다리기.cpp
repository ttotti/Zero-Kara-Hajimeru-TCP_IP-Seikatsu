#include <Windows.h>
#include <stdio.h>

// ������ ���� ��ٸ���

// ������� �ϴ� �����Ǹ� CPU �ð��� ����Ϸ��� �ٸ� ������� �����ϸ鼭 ���������� ����ȴ�
// ������ ���δ� �� �����尡 �ٸ� �������� ���� ����, �� �۾� �Ϸ� ���θ� Ȯ���ؾ� �� ���� �����
// WaitForSingleObject() �Լ��� ����ϸ� Ư�� �����尡 ������ ������ ��ٸ� �� �ִ�

// WaitForSingleObject() �Լ� ��� ���� ������ ����

/* HANDLE hThread = CreateThread(...);
   DWORD retval = WaitForSingleObject(hThread, 1000);
   if(retval == WAIT_OBJECT_0) {...}     // ������ ����
   else if(retval == WAIT_TIMEOUT) {...} // Ÿ�Ӿƿ�(������� ���� ���� ����)
   else {...}							 // ���� �߻�
 */

// ���� �����尡 �����ϱ⸦ ��ٸ����� WaitForSingleObject() �Լ��� ������ ������ŭ ȣ���ؾ� �ϴµ�,
// ��� WaitForMultipleObjects() �Լ��� ����ϸ� ȣ�� �� ������ ���� �� �ִ�

// ������ ��ü �� �ϳ� �Ǵ� ��ΰ� ��ȣ�� ���� ���°��ǰų� �ð� ���� ������ ��� �� ������ ����մϴ�
/* DWORD WaitForMultipleObjects(
     DWORD nCount,
	 const HANDLE* lpHandles,
	 // WaitForMultipleObjects() �Լ��� ����� ���� ������ �ڵ��� �迭�� �־ �����ؾ� �Ѵ�
	    nCount�� �迭 ���� ����, lpHandles�� �迭�� ���� �ּҸ� ��Ÿ����
		nCount�� �ִ��� MAXIMUM_WAIT_OBJECTS(=64)�� ���ǵǾ� �ִ�

	 BOOL bWaitAll. // TRUE�� ��� �����尡 ������ ������ ��ٸ���, FALSE�� �� �����尡 �����ϴ� ��� �����Ѵ�

	 DWORD dwMilliseconds // ������ �ǹ̴� WaitForSingleObject() �Լ��� �� ��° ���ڿ� ����
   )  - ���� : WAIT_OBJECT_0 ~ WAIT_OBJECT_0 + nCount -1 �Ǵ� WAIT_TIMEOUT,  ���� : WAIT_FAILED*/

// WaitForMultipleObjects() �Լ� ��� ���� ������ ����
/* 
   // ��� �������� ���Ḧ ��ٸ���
   HANDLE hThread[2];
   hThread[0] = CreateThread(...);
   hThread[1] = CreateThread(...);

   WaitForMultipleObjects(2, hThread, TRUE, INFINITE);
 */

/* 
   // ������ �ϳ��� ���Ḧ ��ٸ���
   HANDLE hThread[2];
   hThread[0] = CreateThread(...);
   hThread[1] = CreateThread(...);

   DWORD retval = WaitForMultipleObjects(2, hThread, FALSE, INFINITE);

   switch(retval)
   {
	  case WAIT_OBJECT_0:     // hThread[0] ����
		break;
	  case WAIT_OBJECT_0 + 1: // hThread[1] ����
	    break;
	  case WAIT_FAILED:		  // ���� �߻�
	    break;
   }
 */

// ����� WaitForSingleObject()�� WaitForMultipleObjects() �Լ��� ������ ���Ḧ ��ٸ��� ���� �Լ��� �ƴ϶�� ���� ��������
// �� �Լ��� ��� ������ ����ȭ �� ����ϴ� ���� �Լ���, ���⼭�� ���� ������ ���Ḧ ��ٸ��� �������� �̿��� �ͻ��̴�


// ������ ���� ������ ������ϱ�

// ������ �ڵ��� �����ϰ� ������ SuspendThread() �Լ��� ȣ���� �ش� ������ ������ �Ͻ� �����ϰų� ResumeThread() �Լ��� ȣ���� ������� �� �ִ�
// ������ �ü���� �������� ���� Ƚ��(suspend count)�� �����ϴµ�,
// �� ���� SuspendThread() �Լ��� ȣ���� ������ 1�� �����ϰ� ResumeThread() �Լ��� ȣ���� ������ 1�� �����Ѵ�
// ���� Ƚ���� 0���� ũ�� ������� ���� ���� ���¿� �ְ� �ȴ�
// ���� �� �����忡 ���� SuspendThread() �Լ��� �� �� ȣ���ߴٸ� ResumeThread() �Լ��� �� �� ȣ���ؾ� ������� �� �ִ�

/* DWORD SuspendThread(
     HANDLE hThread // ������ �ڵ�
   );  - ���� : ���� Ƚ��, ���� : -1 */

/* DWORD ResumeThread(
     HANDLE hThread // ������ �ڵ�
   );  - ���� : ���� Ƚ��, ���� : -1 */

// ����� ����� �����ϴ� ������ Sleep() �Լ��� �ִ�
// SuspendThread() �Լ��� ȣ���� ��쿡�� �ݵ�� ResumeThread() �Լ��� ����ؾ� �����尡 �����������,
// Sleep() �Լ��� ȣ���ϸ� dwMillseconds�� ������ �ð��� ������ �ڵ����� ������Ѵٴ� ���̰� �ִ�

// Sleep(0) �� ȣ���ϸ� ������� �ڽſ��� �Ҵ�� CPU �ð��� �����ϰ� ���� �ð��� �켱������ ���� �ٸ� �����忡 �Ѱ��ش�
// �̸� �̿��ϸ� ������ �� ���ؽ�Ʈ ��ȯ�� ������ �� �� �ִ�

/* void Sleep(
    DWORD dwMilliseconds // �и���(ms)
   );*/


// ���� ����� ������ ���� ������ ����� Ʋ������ ���� �Ŀ��� �ùٸ� ����� �����ְ� �ִ�
// �����带 �̿��� 1���� 100���� ���� ���Ѵ�,
// ������ ���� �����带 ���� ���·� ������ �� ResumeThread() �Լ��� ����� ������ϰ� �� ���̴�
// ���� �����尡 ����� �Ϸ��ߴ��� Ȯ���Ϸ��� WaitForSingleObject() �Լ��� ����� ���̴�

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

	// ������ ����
	// 5��° ���ڿ� CREATE_SUSPENDED�� ����ϸ� �����尡 ������ ������ ResumeThread() �Լ��� ȣ���ϱ� ������ ������� �ʴ´�
	HANDLE hThread = CreateThread(NULL, 0, MyThread, (LPVOID)num, CREATE_SUSPENDED, NULL);
	if (hThread == NULL)
		return 1;

	printf("������ ���� ��, ��� ��� = %d\n", sum);

	// ������ �����
	ResumeThread(hThread);

	// ������ ��ü�� ��ȣ�� �ްų� ���� �ð��� ��� �� ������ ����մϴ�
	/* DWORD WatiForSingleObject(
	     HANDLE hHandle,       // ���Ḧ ��ٸ� ��� �����带 ��Ÿ����

		 DWORD dwMilliseconds, 
		 // ��� �ð�����, �и��� ������ ����Ѵ�
		    �� �ð� �ȿ� �����尡 �������� ������ WaitForSinfleObject() �Լ��� �����ϰ�, �̶� ���� ���� WAIT_TIMEOUT�� �ȴ�.
			�����尡 ������ ��쿡�� WAIT_OBJECT_0�� �����Ѵ�,
			��� �ð����� INFINITE ���� ����ϸ� �����尡 ������ ������ ������ ��ٸ���
	   )  - ���� : WAIT_OBJECT_0 �Ǵ� WAIT_TIMEOUT, ���� : WAIT_FAILED*/

	// hThread �����尡 ������ ������ ������ ��ٸ���
	WaitForSingleObject(hThread, INFINITE);

	//Sleep(1000);

	printf("������ ���� ��, ��� ��� = %d\n", sum);

	CloseHandle(hThread);

	return 0;
}