

// GUI 응용 프로그램은 다양한 API 로 구현된 편리하고 화려한 사용자 인터페이스를 제공한다
// GUI 응용 프로그램의 핵심 특징은 "메시지 구동 구조" 로 동작한다는 점이다
// 여기서 "메시지"는 윈도우 운영체제가 응용 프로그램의 외부 또는 내부에 변화가 발생했음을 해당 응용 프로그램에 알리는 데 사용하는 개념이다

// 외부에서 메시지를 발생시키는 이벤트가 발생하면, 운영체제가 관리하는 "시스템 메시지 큐" 에 정보가 저장된다
// 각 GUI 응용 프로그램은 운영체제로부터 "응용 프로그램 메시지 큐" 를 할당받는데
// 운영체제는 시스템 메시지 큐에 저장된 메시지를 적절한 응용 프로그램의 메시지 큐에 보낸다
// 각 프로그램은 자신의 메시지 큐를 감시하다가 메시지가 발생해서 큐에 들어오면 하나씩 꺼내 처리하고 메시지가 없을 때는 대기한다

// 메시지 구동 구조를 좀 더 세부적으로 나타내면
// 메시지 구동 구조에서는 응용 프로그램이 받는 메시지에 따라 코드의 실행 순서가 달라지며, 해당 메시지에 어떻게 반응하는가에 따라 동작이 달라진다
// 메시지를 받았을 때 동작을 결정하는 코드를 흔히 "메시지 핸들러" 라 부른다
// 프로그래머는 키보드 메시지 핸들러, 마우스 메시지 행들러, 메뉴 메시지 핸들러 같은 다양한 메시지 핸들러를 작성한다
// 메시지 핸들러의 집합을 윈도우 프로시저 라 부르며, 메시지 처리 코드를 담고 있는 사용자 정의 함수라고 보면 된다