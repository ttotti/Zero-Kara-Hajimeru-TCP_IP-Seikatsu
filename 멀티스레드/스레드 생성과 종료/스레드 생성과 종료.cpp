#include <Windows.h>
#include <stdio.h>

// ���� ����� �� ������, ù ��° ������, �� ��° ������ ������ ����� ����Ǵµ�
// �̶� �ٸ� ���α׷��� �� �� �����ϰ� ���ƿ��� ���� ������ �ٲ� �� �ִ�
// �� ���μ��� ���� ������ ���� ������ �ý��ۿ� �����ϴ� �ٸ� �������� ������ �����Ƿ� �̿� ���� ��ȭ�� �����


// ������ �Լ��� 32��Ʈ���� ū ���� �����ϱ� ���� ����ü�� �����Ѵ�
struct Point3D
{
	int x, y, z;
};

DWORD WINAPI MyThread(LPVOID arg)
{  
	// ���� ������ ���鼭 1�ʸ��� ������ ID�� x, y, z�� ����Ѵ�
	Point3D* pt = (Point3D*)arg;
	
	while (1)
	{
		printf("Running MyThread() %d : %d, %d, %d\n", GetCurrentThreadId(), pt->x, pt->y, pt->z);
		Sleep(1000);
	}

	// ������ �Լ��� ���������ν� �����尡 �����Ѵ�, ���� ������ ���� ������ ������ �� �ڵ�� ������� �ʴ´�
	return 0;
}

int main(int argc, char* argv[])
{
	// ������ �� ���� �����ϵ�, ������ �߻��ϸ� 1(����)�� �����ϸ鼭
	// ���� ���α׷��� �����Ѵ�.
	// CreateThread() �Լ��� �����ϴ� �ڵ��� ���߿� �����带 ������ ������ �ʿ䰡 ������ ������ ���� �ݴ� ���� ����
	// CloseHandle() �Լ��� ȣ���� �ڵ��� �ݴ´ٰ� �ؼ� �����尡 ����Ǵ� ���� �ƴ϶�� ���� ��������

	// ù ��° ������ ����
	Point3D pt1 = { 10,20,30 };

	HANDLE hThread1 = CreateThread(NULL, 0, MyThread, &pt1, 0, NULL);
	if (hThread1 == NULL)
		return 1;

	CloseHandle(hThread1);

	// �� ��° ������ ����
	Point3D pt2 = { 40,50,60 };

	HANDLE hThread2 = CreateThread(NULL, 0, MyThread, &pt2, 0, NULL);
	if (hThread2 == NULL)
		return 1;

	CloseHandle(hThread2);

	// �� ������� ���� ������ ���鼭 1�ʸ��� ������ ID�� ����Ѵ�
	// �� �κ��� �����ϸ� �� �����尡 0�� �����ϸ鼭 ���� ���α׷�, �� ���μ����� �����Ѵ�
	// ���μ����� �����ϸ� ���μ��� ���ο��� ���� ���� ��� ������� �ڵ����� �����Ѵ�
	while (1)
	{
		// GetCurrentThreadId() : ������ �ĺ���(ID) �� �����´�
		printf("Running main() %d\n", GetCurrentThreadId());
		Sleep(1000);
	}

	return 0;
}


// �߰�

// C/C++ ���̺귯�� �Լ��� ����ϴ� ���� ���α׷������� CreateThread(), ExitThread() API �Լ����ٴ�
// _beginthreadex(), _endthreadex() ���̺귯�� �Լ��� ����ϴ� ���� �ٶ����ϴ�

// _beginthreadex(), _endthreadex() �Լ��� CreateThread(), ExitThread()�Լ��� ���� ������ �ϸ�,
// ������ ���������� �̵� API�� ȣ���Ѵ�, ���̰� �ִٸ� C/C++ ���̺귯���� ��Ƽ������ ȯ�濡�� �������� �����ϵ��� �ΰ����� �۾��� �Ѵٴ� ���̴�

// ��� ���� ������ ����, API �Լ��� ������ Ÿ�Ը� �ణ �ٸ� �� ������ �ǹ̰� �����Ƿ� ���� �ڵ带 ���� ������ �ʿ䰡 ����

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