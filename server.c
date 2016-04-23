/*
 * client 접속 정보를 담기위해서 client_info 구조체 생성
 * Client 5개가 접속 될 때까지 대기 및 각 클라이언트간 메시지 전송
 * 접속이 완료되면 client_info를 각 클라이언트로 모두 전송 받음
 * 전송 받은 client 정보를 출력
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

#include "game.c"

#define MAX_BUFF 1024 //buff 크기
#define LISTENQ 100 //Listen Q 설정

#define MAX_USERS 1024

//client 정보를 저장하기 위한 구조체 (접속 IP, port)
struct client_info {
  char clientAddr[32];
  int clientPort;
};

struct user {
  int num;
  int win;
  int lost;
  int draw;
  char *name;
};

struct user *new_user(void)
{
  struct user *new = NULL;
  
  new = malloc(sizeof(struct user*));
  new->num = 0;
  new->win = 0;
  new->lost = 0;
  new->draw = 0;
  new->name = NULL;

  return new;
}

int user_regist(struct user *user, int num, char *name)
{
  if (user == NULL)
    return -1;

  user->num = num;
  user->name = malloc(sizeof(char *) * strlen(name));
  strcpy(user->name, name);

  return 0;
}


struct user *pic_end_user(struct user *arr[], int len)
{
  if (arr == NULL || len < 0)
    return NULL;

  int i = 0;
  for (i = 0; i < len && arr[i]->name != NULL; i++);

  return arr[i];
}

struct user *lookup_user(struct user *arr[], int size, char *str)
{
  if (arr == NULL || str == NULL)
    return NULL;

  int i = 0;
  for (i = 0; i < size; i++) {
    if (!strcmp(arr[i]->name, str))
      return arr[i];
  }

  return NULL;
}

int user_info_strcat(struct user *user, char *org)
{
  char tmp[32] = {'\0', };

  if (user == NULL || org == NULL)
    return -1;

  strcat(org, "[User Scores]\n");
  sprintf(tmp, "%d", user->win);
  strcat(org, tmp);
  strcat(org, " win\n");

  sprintf(tmp, "%d", user->lost);
  strcat(org, tmp);
  strcat(org, " lost\n");

  sprintf(tmp, "%d", user->draw);
  strcat(org, tmp);
  strcat(org, " draw\n");

 
  return 0;
}

int main(int argc, char *argv[]) 
{
  struct sockaddr_in servaddr, cliaddr;
  struct client_info cliinfo;
  int listen_sock = 0, accp_sock = 0;
  int addrlen = sizeof(servaddr);
  int nbyte; //전송 받은 메시지 byte 저장
  char buff[MAX_BUFF];
  int user_count = 0;

  int ret = 0;

  // Argument exception
  if (argc < 2) {
    printf("Server Port : %s \n", argv[0]);
    exit(0);
  }

  // Socket
  listen_sock = socket(PF_INET, SOCK_STREAM, 0);
  if (listen_sock < 0) {
    perror("socket Fail");
    exit(0);
  }

  // Init Server info
  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(atoi(argv[1]));

  // Call bind
  ret = bind(listen_sock, (struct sockaddr *)&servaddr, sizeof(servaddr));
  if (ret < 0) {
    perror("bind Fail");
    exit(0);
  }

  // Init user_list
  struct user **user_list = NULL;
  int i = 0;
  user_list = malloc(sizeof(struct user*) * MAX_USERS);
  for (i = 0; i < MAX_USERS; i++) {
    user_list[i] = new_user();
  }

  // Listen
  listen(listen_sock, LISTENQ);

  // Loop
  while (user_count < 5) {

    // Accept 클라이언트 연결 받음
    accp_sock = accept(listen_sock, (struct sockaddr *)&cliaddr, &addrlen);
    if(accp_sock < 0) {
      perror("accept fail");
      exit(0);
    }

    strcpy(cliinfo.clientAddr, inet_ntoa(cliaddr.sin_addr));
    cliinfo.clientPort = ntohs(cliaddr.sin_port);
    /* printf("[Client 연결] ID : %s, Port : %d\n", cliinfo.clientAddr, cliinfo.clientPort); */

    // 클라이언트로부터 아이디 받기
    memset(buff, '\0', MAX_BUFF);
    nbyte = read(accp_sock, buff, MAX_BUFF);
    if (nbyte < 0) {
      perror("read fail");
      exit(0);
    }
    buff[nbyte] = 0;

    // 기존 유저 찾기
    struct user *curr_user = NULL;
    curr_user = lookup_user(user_list, user_count, buff);
    // 새로운 유저 생성
    if (curr_user == NULL) {
      curr_user = pic_end_user(user_list, MAX_USERS);
      user_regist(curr_user, user_count, buff);

      user_count++;
    }

    // 접속 정보 출력
    printf("user : %d, id : %s\n", curr_user->num, curr_user->name);

    // 클라이언트로부터 받은 메세지
    memset(buff, '\0', MAX_BUFF);
    nbyte = read(accp_sock, buff, MAX_BUFF);
    if (nbyte < 0) {
      perror("read fail");
      exit(0);
    }
    buff[nbyte] = 0;
    printf("client Data str : %s\n", buff);

    int rand_val = game_rand();
    int user_val = atoi(buff);
    printf("RAND : %s \n", game_to_string(rand_val));
    printf("USER : %s \n", game_to_string(user_val));
    printf("RAND RESULT : %d \n", game_result(user_val, rand_val));

    // 클라이언트에 보낼 메세지
    memset(buff, '\0', MAX_BUFF);
    if (0 < game_result(user_val, rand_val)) {
      strcpy(buff, "[Win!] \n\n");
      curr_user->win++;
    } else if (game_result(user_val, rand_val) < 0) {
      strcpy(buff, "[Lost!] \n\n");
      curr_user->lost++;
    } else {
      strcpy(buff, "[Draw!] \n\n");
      curr_user->draw++;
    }

    strcat(buff, "Computer is : ");
    strcat(buff, game_to_string(rand_val));
    strcat(buff, "\n");
    strcat(buff, "You are : ");
    strcat(buff, game_to_string(user_val));
    strcat(buff, "\n");

    user_info_strcat(curr_user, buff);

    nbyte = write(accp_sock, buff, MAX_BUFF);
    if (nbyte < 0) {
      perror("set read fail");
      exit(0);
    }

  }

  close(listen_sock);
  return 0;
}

