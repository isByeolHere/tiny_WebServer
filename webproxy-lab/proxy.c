#include <stdio.h>
#include "csapp.h"

/* Recommended max cache and object sizes */
#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400

/* You won't lose style points for including this long line in your code */
static const char *user_agent_hdr =
    "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 "
    "Firefox/10.0.3\r\n";

void proxy_handler(int connfd);

int main(int argc, char **argv) {
    int listenfd, connfd;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;
    char client_hostname[MAXLINE], client_port[MAXLINE];

    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(1);
    }

    listenfd = Open_listenfd(argv[1]);  // 1. Listen socket 생성
    while (1) {
        clientlen = sizeof(clientaddr);
        connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);  // 2. 연결 수락
        Getnameinfo((SA *)&clientaddr, clientlen,
                    client_hostname, MAXLINE,
                    client_port, MAXLINE, 0);
        printf("Accepted connection from (%s, %s)\n", client_hostname, client_port);

        proxy_handler(connfd);  // 3. 요청 처리 함수 호출
        Close(connfd);          // 4. 연결 종료
    }
    return 0;
}

void proxy_handler(int connfd) {
    char buf[MAXLINE];
    rio_t rio;

    Rio_readinitb(&rio, connfd);  // 1. rio 버퍼 초기화
    Rio_readlineb(&rio, buf, MAXLINE);  // 2. 요청 한 줄 읽기
    printf("Request line: %s", buf);    // 3. 출력해보기 (ex: GET http://... HTTP/1.1)

    // TODO: 다음 단계에선 Host 파싱하고 서버에 전달할 예정!
}
