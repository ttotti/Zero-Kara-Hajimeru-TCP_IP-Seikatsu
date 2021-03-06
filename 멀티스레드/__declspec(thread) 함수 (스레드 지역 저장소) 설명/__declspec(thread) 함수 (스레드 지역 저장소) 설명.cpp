

// __declspec(thread) (스레드 지역 저장소) 설명

// 한 프로세스 안에 존재하는 모든 스레드는 코드, 데이터(전역 변수, 정적 변수), 힙, 환경 변수를 공유한다
// 반면 스택(함수 전달 인자, 지역 변수)은 스레드별로 할당되며, 스레드가 실행 중에 사용하는 CPU 레지스터 값도 운영체제가 스레드별로 독립적으로 유지/관리해준다

// 이런 특징을 잘 알고 있지 않으면 멀티스레드 환경에서 오동작하는 코드를 만들기 십상이다
// C/C++ 언어로 응용 프로그램을 개발할 때는 함수 단위로 기능을 분할하기 마련인데,
// 멀티스레드에서 안심하고 호출하려면 지역 변수만 사용해서 구현하는 것이 좋다
// 즉 스택은 스레드별로 존재하므로 별도의 스레드 동기화 기법을 사용해 보호하지 않아도 안전하기 때문이다

// 전역 변수나 정적 변수의 사용 목적이 데이터 공유라면 적절한 스레드 동기화 기법을 사용해 보호하면 되지만,
// 그 외에는 멀티스레드 환경에서 문제를 일으킬 가능성만 키울 뿐이다

// 구현상의 이유로 전역 변수나 정적 변수를 함수에서 사용하되 데이터 공유가 목적이 아니라면
// 스레드 지역 저장소를 사용하면 된다

// 스레드 지역 저장소(TLS : Thread Local Storage)는 스레드별로 전역 변수나 정적 변수등을 위한 저장 공간이 필요할 때 윈도우 운영체제가 할당해주는 메모리 영역이다
// 스레드 지역 저장소는 윈도우 API함수(TlsAlloc(), TlsSetValue(), TlsGetValue(), TlsFree()) 수준에서 지원되지만,
// 비주얼 C++의 확장 기능을 통해 더 편리하게 사용할 수 있다

// 다음 코드를 보면 TestFunc() 함수가 전역 변수, 정적 변수, 지역 변수를 사용하고 있다

// 둘 이상의 스레드가 TestFunc() 함수를 동시에 호출해서 사용하면
// 전역 변수와 정적 변수를 윈치 않게 공유하게 되어 문제가 발생할 수 있다
// 이때 전역 변수와 정적 변수 앞에 __declspec(thread) 지시자를 붙이면 된다

int glo_var;  // 전역 변수

void TestFunc(void)
{
	static int sta_var; // 정적 변수
	int loc_var;        // 지역 변수

	glo_var = 10;
	sta_var = 20;
	loc_var = 30;
}

//     l
//     l 코드 변경
//     V

// 이제 전역 변수와 정적 변수는 프로세스 내에 존재하는 스레드 개수만큼 존재하게 되어 
// 각 스레드가 자신만의 전역 변수와 정적 변수를 갖기 때문에 불필요한 메모리 영역 공유가 생기지 않는다
// 참고로, 지역 변수는 스레드별로 스택 영역에 따로 할당되므로 스레드 지역 저장소를 사용할 필요가 없다

__declspec(thread) int glo_var;  // 전역 변수

void TestFunc(void)
{
	__declspec(thread) static int sta_var; // 정적 변수
	int loc_var; // 지역 변수

	glo_var = 10;
	sta_var = 20;
	loc_var = 30;
}

