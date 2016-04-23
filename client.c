/*
 * fork()를 이용하여 1개의 부모와 5개의 자식 프로세스를 생성
 * 생성된 자식 프로세스는 각각 서버와 connect
 * connect 된 후 서버에 전송해주는 5개의 자식프로세스
 * 접속 정보를 객체로 받아 출력
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

#define MAX_BUFF 1024 //buf 크기
#define TOTALFORK 5 //클라이언트 수

struct client_info {
  char clientAddr[32];
  int clientPort;
};

void createClient(char *port, char *serverIP);

int main(int argc, char *argv[])
{
  // Argument
  if(argc < 3) {
    printf("Use %s ip_addr port\n", argv[0]);
    exit(0);
  }

  pid_t pids[TOTALFORK];
  int runProcess = 0;

  createClient(argv[2], argv[1]);

  return 0;
}

void createClient(char *port, char *serverIP)
{
  struct sockaddr_in servaddr;
  struct client_info *cliinfo;
  int strlen = sizeof(servaddr);
  cliinfo = malloc(TOTALFORK);
  int sockfd, nbyte, cNum;//cNum 연결 번호
  char buff[MAX_BUFF];

  sockfd = socket(PF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    perror("socket fail");
    exit(0);
  }

  // Init info
  memset(&servaddr, 0, strlen);
  servaddr.sin_family = AF_INET;
  inet_pton(AF_INET, serverIP, &servaddr.sin_addr);
  servaddr.sin_port = htons(atoi(port));

  // Connect
  if (connect(sockfd, (struct sockaddr *)&servaddr, strlen) < 0) {
    perror("Connect fail");
    exit(0);
  }

  // 서버로 메세지 보내기
  printf("Id : ");
  gets(buff);
  write(sockfd, buff, sizeof(buff));

  // 서버로 메세지 보내기
  do {
    printf("1 : ROCK \n");
    printf("2 : SCISSORS \n");
    printf("3 : PAPER \n");
    printf("Data : ");
    gets(buff);
  } while (atoi(buff) < 0 || 2 < atoi(buff));
  write(sockfd, buff, sizeof(buff));

  // 서버에서 데이터 받기
  read(sockfd, buff, sizeof(buff));
  printf("=== Game Result ===\n");
  printf("%s\n", buff);

  close(sockfd);
}
