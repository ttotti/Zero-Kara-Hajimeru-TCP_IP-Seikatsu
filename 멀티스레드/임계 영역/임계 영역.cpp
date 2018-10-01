
// �Ӱ� ����

// �Ӱ� ������ �� �̻��� �����尡 ���� �ڿ��� ������ ��, ���� �� �����常 ������ ����ؾ� �ϴ� ��쿡 ����Ѵ�
// �Ӱ� ������ ��ǥ���� ������ ����ȭ ���������, ������ ������ �޶� "����ȭ ��ü" �� �з������� �ʴ´�

// ��ǥ���� Ư¡
/*
	1. �Ӱ� ������ �Ϲ� ����ȭ ��ü�� �޸� ���� ���μ����� ����(user)�޸� ������ �����ϴ� �ܼ��� ����ü��
	   ���� �ٸ� ���μ����� ������ �� �����Ƿ� �� ���μ����� ���� ������ �� ����ȭ���� ����Ѵ�

	2. �Ϲ� ����ȭ ��ü���� ������ ȿ�����̴�
*/

// �Ӱ� ���� ��� ���� ������ ����

/*
	#include <windows.h>

	// CRITICAL_SECTION ����ü ������ ���� ������ �����Ѵ�
	// �Ϲ� ����ȭ ��ü�� Create*() �Լ��� ȣ���� Ŀ�� �޸� ������ ����������,
	// �Ӱ� ������ ���� �޸� ������ (�밳�� ���� ���� ���·�) �����Ѵ�
	1. CRITICAL_SECTION cs;

	DWORD WINAPI MyThread1(LPVOID arg)
	{
		...
		// ���� �ڿ��� �����ϱ� ���� EnterCriticalSection() �Լ��� ȣ���Ѵ�
		// ���� �ڿ��� ����ϰ� �ִ� �����尡 ���ٸ� EnterCriticalSection() �Լ��� ��ٷ� �����Ѵ�,
		// ������ ���� �ڿ��� ����ϰ� �ִ� �����尡 �ִٸ� EnterCriticalSection() �Լ��� �������� ���ϰ� ������� ��� ���°� �ȴ�
		3. EnterCriticalSection(&cs);

		// ���� �ڿ� ����

		// ���� �ڿ� ����� ��ġ�� LeaveCriticalSection() �Լ��� ȣ���Ѵ�,
		// �̶� EnterCriticalSection() �Լ����� ��� ���� �����尡 �ִٸ� �ϳ��� ���õǾ� �����
		4. LeaveCriticalSection(&cs);
		...
	}

	DWORD WINAPI MyThread2(LPVOID arg)
	{
		...
		3. EnterCriticalSection(&cs);
		// ���� �ڿ� ����
		4. LeaveCriticalSection(&cs);
		...
	}

	int main(int argc, char* argv[])
	{
		...
		// �Ӱ� ������ ����ϱ� ���� InitializeCriticalSection() �Լ��� ȣ���� �ʱ�ȭ �Ѵ�
		2. InitializeCriticalSection(&cs);

		// �����带 �� �� �̻� ������ �۾��� �����Ѵ�
		// ������ ��� �����尡 ������ ������ ��ٸ���

		// �Ӱ� ������ ����ϴ� ��� �����尡 �����ϸ� DeleteCriticalSection() �Լ��� ȣ���� �����Ѵ�
		5. DeleteCriticalSection(&cs);
		...
	}
*/

// �� ������� �ڱ⸸�� CPU �������� ���� ������ �����Ѵ�
// ���� ���� �����Ͱ� ���� �������� �ʰ� ���� ���Ǵٰ� ��������� �̻��� ���� ���͹����� ��Ȳ�̴�

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


// �̹����� �Ӱ� ������ ����� g_count ������ �� �����常 �����ϰ� �Ѵ�
// for ���� ��ü�� ��ȣ�� ���� ������ �� �� ������ ������ �����Ϸ��� g_count ������ �����ϴ� �ڵ� �� �ุ �Ӱ� �������� ������

// ���� ����� �ùٸ� ���� 0�� ����Ѵ�
// ����ȭ�� ���� ������� ������ ����� ��������� �ð��� �ټ� �ɸ�������
// �� ������ �ش����� ��Ȳ���� ����ġ�� ������ ������ ������ ����ȭ�� �ϰ� �־ ���� ���ϰ� �ε巯����

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
	// �Ӱ� ���� �ʱ�ȭ
	InitializeCriticalSection(&cs);

	// ������ �� �� ����
	HANDLE hThread[2];
	hThread[0] = CreateThread(NULL, 0, MyThread1, NULL, 0, NULL);
	hThread[1] = CreateThread(NULL, 0, MyThread2, NULL, 0, NULL);

	// ������ �� �� ���� ���
	WaitForMultipleObjects(2, hThread, TRUE, INFINITE);

	// �Ӱ� ���� ����
	DeleteCriticalSection(&cs);

	printf("g_count = %d\n", g_count);

	return 0;
}