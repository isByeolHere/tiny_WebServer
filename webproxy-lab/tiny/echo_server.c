#include "csapp.h"

void echo(int connfd);

int main(int argc, char **argv)
{
  int listenfd, connfd;
  socklen_t clientlen;
  struct sockaddr_storage clientaddr; /* Enough space for any address */
  char client_hostname[MAXLINE], client_port[MAXLINE];

    if (argc != 2) {
      fprintf(stderr, "usage: %s <port>\n", argv[0]);
      exit(0);
    }

    listenfd = Open_listenfd(argv[1]);
    while (1) {
      clientlen = sizeof(struct sockaddr_storage);
      connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
      Getnameinfo((SA *) &clientaddr, clientlen, client_hostname, MAXLINE,
                        client_port, MAXLINE, 0);
      printf("Connected to (%s, %s)\n", client_hostname, client_port);
      echo(connfd);
      Close(connfd);
    }
    exit(0);
}

void echo(int connfd)
{
    size_t n;
    char buf[MAXLINE];
    rio_t rio;

    Rio_readinitb(&rio, connfd);
    while((n = Rio_readlineb(&rio, buf, MAXLINE)) != 0) {
      printf("server received %d bytes\n", (int)n);
      Rio_writen(connfd, buf, n);
    }
}




// // echo_server.c

// #include <stdio.h>      // 표준 입출력 (printf, perror)
// #include <stdlib.h>     // 표준 라이브러리 (exit, atoi)
// #include <string.h>     // 문자열 처리 (bzero)
// #include <unistd.h>     // 유닉스 표준 함수 (read, write, close)
// #include <arpa/inet.h>  // 인터넷 주소 변환 (htons, inet_addr 등 포함될 수 있음)
// #include <sys/socket.h> // 소켓 관련 핵심 함수 및 구조체 (socket, bind, listen, accept)
// #include <netinet/in.h> // 인터넷 주소 패밀리 구조체 (struct sockaddr_in)

// #define BUFFER_SIZE 1024 // 데이터를 읽고 쓸 버퍼 크기

// // 오류 처리 함수 (메시지 출력 후 종료)
// void error_handling(char *message) {
//     perror(message); // 오류 메시지 출력 (시스템 오류 원인 포함)
//     exit(1);        // 프로그램 비정상 종료
// }

// int main(int argc, char *argv[]) {
//     int serv_sock;          // 서버 리스닝 소켓의 파일 디스크립터
//     int clnt_sock;          // 클라이언트 연결 소켓의 파일 디스크립터
//     char message[BUFFER_SIZE]; // 클라이언트 메시지를 저장할 버퍼
//     int str_len;            // 읽은 바이트 수

//     struct sockaddr_in serv_addr; // 서버 주소 정보를 담을 구조체
//     struct sockaddr_in clnt_addr; // 클라이언트 주소 정보를 담을 구조체
//     socklen_t clnt_addr_size;    // 클라이언트 주소 구조체의 크기

//     // 1. 명령줄 인자로 포트 번호가 주어졌는지 확인. 번호가 없으면 아래 명령어 실행.
//     if (argc != 2) {
//         printf("Usage: %s <port>\n", argv[0]);
//         exit(1);
//     }

//     // 2. 소켓 생성 (TCP 소켓)
//     //    AF_INET: IPv4 인터넷 프로토콜
//     //    SOCK_STREAM: 연결 지향형 소켓 (TCP)
//     //    0: 프로토콜 기본값 사용 (TCP)
//     serv_sock = socket(PF_INET, SOCK_STREAM, 0);
//     if (serv_sock == -1) {
//         error_handling("socket() error");
//     }

//     // 3. 서버 주소 구조체 설정
//     memset(&serv_addr, 0, sizeof(serv_addr)); // 구조체를 0으로 초기화
//     serv_addr.sin_family = AF_INET;                 // 주소 체계: IPv4
//     serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); // 서버 IP 주소 설정
//                                                     // htonl: 호스트 바이트 순서 -> 네트워크 바이트 순서 (Long)
//                                                     // INADDR_ANY: 사용 가능한 모든 로컬 IP 주소로 들어오는 연결을 받음 (예: 0.0.0.0)
//     serv_addr.sin_port = htons(atoi(argv[1]));   // 서버 포트 번호 설정
//                                                     // atoi: 문자열 포트번호 -> 정수
//                                                     // htons: 호스트 바이트 순서 -> 네트워크 바이트 순서 (Short)

//     // 4. 소켓에 주소 정보 할당 (바인딩)
//     if (bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
//         error_handling("bind() error");
//     }

//     // 5. 연결 요청 대기열 생성 (클라이언트 연결 요청 대기 상태로 전환)
//     //    5: 연결 요청 대기열(큐)의 크기. 동시에 5개의 연결 요청을 큐에 저장 가능
//     if (listen(serv_sock, 5) == -1) {
//         error_handling("listen() error");
//     }

//     printf("Server listening on port %s...\n", argv[1]);

//     // 6. 클라이언트 연결 수락 루프 (무한 반복)
//     while (1) {
//         clnt_addr_size = sizeof(clnt_addr); // accept 함수 호출 전 크기 초기화 중요!

//         // 클라이언트의 연결 요청 수락. 연결이 올 때까지 대기(blocking)
//         // 연결되면 클라이언트와 통신할 새로운 소켓(clnt_sock)이 생성됨
//         // clnt_addr 구조체에는 연결된 클라이언트의 주소 정보가 채워짐
//         clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
//         if (clnt_sock == -1) {
//             // accept 실패 시 루프 계속 (서버가 죽으면 안됨)
//             perror("accept() error");
//             continue;
//         }

//         // inet_ntoa: 네트워크 바이트 순서의 IP 주소 -> 문자열로 변환
//         // ntohs: 네트워크 바이트 순서의 포트 -> 호스트 바이트 순서로 변환
//         printf("Connection accepted from %s:%d\n",
//                inet_ntoa(clnt_addr.sin_addr), ntohs(clnt_addr.sin_port));

//         // 7. 데이터 수신 및 송신 (에코) 루프
//         //    클라이언트가 연결을 끊거나 오류 발생 시까지 반복
//         while ((str_len = read(clnt_sock, message, BUFFER_SIZE)) != 0) {
//             if (str_len == -1) {
//                 // read 오류 발생
//                 perror("read() error");
//                 break; // 에코 루프 탈출
//             }

//             // 읽은 데이터(message)를 그대로 클라이언트에게 다시 전송 (에코)
//             // 주의: write는 요청한 만큼 다 못 보낼 수도 있지만, 여기선 단순화
//             if (write(clnt_sock, message, str_len) == -1) {
//                 perror("write() error");
//                 break; // 에코 루프 탈출
//             }
//             // 실제로는 write의 반환값을 확인하며 모든 데이터를 보낼 때까지 반복해야 할 수 있음
//         }

//         // 8. 클라이언트 연결 종료
//         //    read()가 0을 반환하면 클라이언트가 연결을 정상 종료한 것
//         close(clnt_sock);
//         printf("Connection closed from %s:%d\n",
//                inet_ntoa(clnt_addr.sin_addr), ntohs(clnt_addr.sin_port));
//     } // end of while(1)

//     // (이 코드는 무한 루프 때문에 실제로는 도달하지 않지만) 서버 리스닝 소켓 닫기
//     close(serv_sock);
//     return 0;
// }