

// TCP 서버 - 클라이언트의 핵심 동작을 개념적으로 정리하면 다음과 같다

// 1. 서버는 먼저 실행하여 클라이언트가 접속하기를 기다린다(listen)
// 2. 클라이언트는 서버에 접속(connect) 하여 데이터를 보낸다(send)
// 3. 서버는 클라이언트 접속을 수용하고(accept) 클라이언트가 보낸 데이터를 받아서(recv) 처리한다
// 4. 서버는 처리한 데이터를 클라이언트에 보낸다(send)
// 5. 클라이언트는 서버가 보낸 데이터를 받아서(recv) 처리한다
// 6. 데이터를 주고받는 과정을 모두 마치면 접속을 끊는다(closesocket)

//    TCP 서버                        TCP 클라이언트
//       I                                  I
//       V                                  I
//     listen 							    I
//       I                                  I
//       V                                  V
//     accept <<--- l----------l <<----- connect
//       I          l          l            I      
// --->> I		    l          l            I <<----
// l     V          l          l            V      l
// l    recv <<---- l 네트워크  l <<-----  send     l
// l     I          l          l            I      l
// l     V          l          l            V      l
// l    send ---->> l          l  ----->> recv     l
// l     I          l----------l            I      l
// ----- I			 					    I ------
//       V                                  V
//    closesocket                      closesocket

// 이와 같은 방식은 (웹 서버 - 클라이언트), (텔넷 서버 - 클라이언트), (FTP 서버 - 클라이언트) 등에도 동일하게 적용된다

// ------------------------------------------------------------------------------------------------------------------------------------------------

// TCP 서버 - 클라이언트 동작 원리

// TCP 서버와 TCP 클라이언트가 연결되서 통신을 수행하는 과정을 요약하면 다음과 같다

// 1. 서버는 소켓을 생성한 후 클라이언트가 접속하기를 기다린다 
//    이때 서버가 사용하는 소켓은 특정 포트 번호(예를 들면 9000번)와 결합되어 있어서 이 포트 번호로 접속하는 클라이언트만 수용할 수 있다

// 2. 클라이언트가 서버에 접속한다 
//    이때 TCP 프로토콜 수준에서 연결 설정을 위한 패킷 교환이 일어난다(TCP 연결 시에는 세 개의 패킷을 주고받으며 각각 SYN, SYN/ACK, ACK 라 부른다)

// 3. TCP 프로토콜 수준의 연결 절차가 끝나면 서버는 접속한 클라이언트와 통신할 수 있는 새로운 소켓을 생성한다 
//    이 소켓을 이용해 서버는 클라이언트와 데이터를 주고 받는다 기존 소켓은 새로운 클라이언트 접속을 수용하는 용도로 계속 사용한다

// 4. 두 클라이언트가 접속한 후의 상태를 나타낸 것이다
//    서버에는 소켓이 총 세 개 존재하며, 이 중 두 소켓을 접속한 클라이언트와 통신하는 용도로 사용한다

// ------------------------------------------------------------------------------------------------------------------------------------------------

// TCP 서버 - 클라이언트 실습

// 서버 : 클라이언트가 보낸 데이터를 받아서(recv) 이를 문자열로 간주해 무조건 화면에 출력한다(printf). 
//        그리고 받은 데이터를 변경 없이 다시 클라이언트에 보낸다(send). 
//        받은 데이터를 그대로 다시 보낸다는 뜻으로 에코 서버(echo server)라 부른다

// 클라이언트 : 사용자가 키보드로 입력한(fgets) 문자열을 서버에 보낸다(send)
//             서버가 받은 데이터를 그대로 돌려보내면
//             클라이언트는 이를 받아서(recv) 화면에 출력한다(printf)
//             에코 서버와 통신한다는 의미로 에코 클라이언트(echo client)라 부른다

// ------------------------------------------------------------------------------------------------------------------------------------------------

// TCP 클라이언트를 정상적인 방법으로 종료하면 TCP 클라이언트가 사용하던 포트는 일정시간(5분 이내) TIME_WAIT 상태에 있다가 사라진다

// TCP 클라이언트를 비정상적인 방법으로 종료하면(강제 종료) TCP 클라이언트가 사용하던 포트는 TIME_WAIT 상태를 거치지 않고 사라진다

// ------------------------------------------------------------------------------------------------------------------------------------------------

// TCP 서버 함수

// 일반적으로 TCP 서버는 다음과 같은 순서로 소켓 함수를 호출한다

// 1. socker() 함수로 소켓을 생성함으로써 사용할 프로토콜을 결정한다
// 2. bind() 함수로 지역 IP 주소와 지역 포트 번호를 결정한다
// 3. listen() 함수로 TCP를 LISTENING 상태로 변경한다
// 4. accept() 함수로 자신에게 접속한 클라이언트와 통신할 수 있는 새로운 소켓을 생성한다 (이때 원격 IP 주소와 원격 포트 번호가 결정된다)
// 5. send(), recv() 등의 데이터 전송 함수로 클라이언트와 통신을 수행한 후, closesocket() 함수로 소켓을 닫는다
// 6. 새로운 클라이언트 접속이 들어올 때마다 4번 ~ 5번 과정을 반복한다

// TCP 서버                                    -----------------------
//                                             V                     I                         
// socket() -> bind() -> listen() -> accept() --> recv() --> send() --> closesocket()
//                                      I          ^         I
//							            V          I         V
//                                  l----------------------------l
//                                  l          네트워크           l
//                                  l----------------------------l
//                                      I          ^         I
//                                      V          I         V
// socket() ----------------------> connect() --> send() --> recv() --> closesocket()
//                                             ^                     I
// TCP 클라이언트                               -----------------------

// TCP 클라이언트 함수
// 일반적으로 TCP 클라이언트는 다음과 같은 순서로 소켓 함수를 호출한다

// 1. socket() 함수로 소켓을 생성함으로써 사용할 프로토콜을 결정한다
// 2. connect() 함수로 서버에 접속한다 (이때 원격 IP 주소와 원격 포트 번호는 물론, 지역 IP 주소와 지역 포트 번호도 결정된다)
// 3. send(), recv() 등의 데이터 전송 함수로 서버와 통신한 후, closesocket() 함수로 소켓을 닫는다

// TCP 데이터 전송 함수

// 데이터 전송 함수는 크게 데이터를 보내는 함수와 데이터를 받는 함수로 구분할 수 있다
// 가장 기본이 되는 함수는 send()와 recv()며, 그 외에 sendto() / recvfrom()과 WSASend*() / WSARecv*() 형태의 확장 함수가 존재한다

// TCP 소켓과 연관된 데이터 구조체에는 지역/원격 주소 정보 외에 데이터 송수신 버퍼가 있다
// 송신 버퍼(send buffer)는 데이터를 전송하기 전에 임시로 저장해두는 영역이고
// 수신 버퍼(receive buffer)는 받은 데이터를 응용 프로그램이 처리하기 전까지 임시로 저장해두는 영역이다

// 송신 버퍼와 수신 버퍼를 통틀어 소켓 버퍼(socker buffer)라 부른다
// 여기서 send() 와 recv() 함수는 소켓 버퍼에 접근할 수 있게 만든 함수라고 보면 된다

// 데이터 전송 함수를 사용할 때는 통신 프로토콜의 특성을 잘 알고 있어야 한다
// TCP 프로토콜은 응용 프로그램이 보낸 데이터의 경계를 구분하지 않는다는 특징이 있다
// 예를 들어 클라이언트가 100, 200, 300 바이트 데이터를 차례로 보낼 경우 서버가 데이터의 경계를 구분하지 못하고 250, 350 바이트 데이터를 읽을 수 있다
// 따라서 TCP 서버 - 클라이언트를 작성할 때는 데이터 경계 구분을 위한 상호 약속이 필요하며, 이를 응용 프로그램 수준에서 처리해야 한다