/*
 * =====================================================================================
 *
 *       Filename:  game.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016년 04월 23일 22시 53분 28초
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// 가위바위보 정보
#define ROCK 0
#define SCISSORS 1
#define PAPER 2

char STR_ROCK[] = "ROCK";
char STR_SCISSORS[] = "SCISSORS";
char STR_PAPER[] = "PAPER";

// 가위바위보 진리표
int RESULT_TALBR[3][3] = {{0, 1, -1}, {-1, 0, 1}, {1, -1, 0}};

/*
 * 0~2 랜덤값을 리턴 함
 */
int game_rand(void)
{
  long seed;
  seed = time(NULL);
  srand(seed);

  return rand() % 3;
}

/*
 * 가위바위보 결과 점수를 리턴함
 * @input : 사용자가 입력한 값
 * @ran : 랜덤값
 */
int game_result(int input, int ran)
{
  if (input != ROCK && input != SCISSORS && input != PAPER)
    return -1;

  if (ran != ROCK && ran != SCISSORS && ran != PAPER)
    return -1;

  return RESULT_TALBR[input][ran];
}

/*
 * 가위바위보 값을 출력함
 */
char *game_to_string(int v)
{
  if (v == ROCK)
    return STR_ROCK;
  if (v == SCISSORS)
    return STR_SCISSORS;
  if (v == PAPER)
    return STR_PAPER;
}




