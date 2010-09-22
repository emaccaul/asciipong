/******************************************************************************
 * File   : pong.c
 * Author : Emmanuel MacCaull
 * Description : Code for the ascii version of pong. (ascii == text only)
 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "keyboard.h"
#include "timer.h"
#include "titlescreen.h"
#include "command_options.h"

typedef struct {
  int xPos, xdir;
  int yPos, ydir;
} Pong;

#ifndef PLAYER_NAME_LEN
# define PLAYER_NAME_LEN  32
#endif

typedef struct {
  int xPos;   /* x position (horizontal) */
  int yPos;   /* y position (vertical) */
  char name[PLAYER_NAME_LEN];  /* Player's Name */
  int wins;   /* number of times player has won */
  char buttons[2];   /* stores directions of player travel */
} PlayerObj;

typedef enum {
  LEFT = -1,
  RIGHT = 1
} DIRECTION;

#define WIDTH   32
#define HEIGHT  27

/*               Y pos   X pos */
static char grid[HEIGHT][WIDTH+1];

typedef enum { FALSE=0, TRUE } BOOL;
BOOL reset_now = FALSE;
BOOL playerLost = FALSE;

/* current round */
int  cur_round = 1;

#define PONG    'o'   /* pong ball */
#define PADDLE  '_'
#define WALL    '|'
#define VSPACE  ' '   /* void area */

typedef enum {
  ERR_NO_ERROR=0,
  ERR_OUT_OF_BOUNDS
} ERROR_CODE;

void populateGrid()
{
  int i, j;
  for (i=0; i < HEIGHT; i++) {
    for (j=0; j < WIDTH; j++) {
      if (j == 0 || j == WIDTH-1)
        grid[i][j] = WALL;
      else
        grid[i][j] = VSPACE;
    }
    grid[i][WIDTH] = '\0';
  } 
}

ERROR_CODE addPlayer(PlayerObj * player)
{
  if ((player->xPos > 0 && player->xPos < WIDTH-1) &&
      (player->yPos > 0 && player->yPos < HEIGHT-1)) {
    grid[player->yPos][player->xPos] = PADDLE;
    return ERR_NO_ERROR;
  }
  return ERR_OUT_OF_BOUNDS;
}

void movePlayer(PlayerObj * player, DIRECTION dir)
{
  int newPos = player->xPos + dir;
  if (newPos <= 0)
    newPos = 1;
  else if (newPos >= WIDTH-1)
    newPos = WIDTH-2;

  grid[player->yPos][player->xPos] = VSPACE;
  grid[player->yPos][newPos] = PADDLE;
  player->xPos = newPos;
}

void moveAI(PlayerObj * pc, Pong * pong)
{
  int diff = (pc->xPos - pong->xPos);

  if (pc->yPos < HEIGHT/2-7 && pong->ydir == 1) return;
  else if (pc->yPos > HEIGHT/2+7 && pong->ydir == -1) return;

  if (diff > 0)
    movePlayer(pc, LEFT);
  else if (diff < 0)
    movePlayer(pc, RIGHT);
}

void reset(PlayerObj * p1, PlayerObj * pc, Pong * pong)
{
  PlayerObj * handle = NULL;
  int pcPos = rand() % (WIDTH-2) + 1;

  p1->buttons[0] = p1->buttons[1] =
    pc->buttons[0] = pc->buttons[1] = 0;
  populateGrid();

  p1->xPos = WIDTH/2;
  p1->yPos = HEIGHT - 4;

  pc->xPos = pcPos;
  pc->yPos = 3;

  pong->xPos = pcPos;
  pong->yPos = 4;
  pong->xdir = 1;
  pong->ydir = 1;

  if (addPlayer(p1) != ERR_NO_ERROR)
    handle = p1;
  else if (addPlayer(pc) != ERR_NO_ERROR)
    handle = pc;
  
  if (handle) {
    printf("Error setting %s's position\n", handle->name);
    waitForUserInput();
  }
}

void newRound()
{
  clear_screen();
  printf("Round Over\n");
  waitForUserInput();
  reset_now = TRUE;
  cur_round++;
}

void movePong(Pong * pong, PlayerObj * p1, PlayerObj * p2)
{
  grid[pong->yPos][pong->xPos] = VSPACE;

  pong->xPos = (pong->xPos + 1*pong->xdir);
  pong->yPos = (pong->yPos + 1*pong->ydir);

  if ((pong->yPos == p1->yPos && pong->xPos >= p1->xPos-1 && pong->xPos <= p1->xPos+1) ||
      (pong->yPos == p2->yPos && pong->xPos >= p2->xPos-1 && pong->xPos <= p2->xPos+1))
  {
    pong->yPos -= 1*pong->ydir;
    pong->ydir *= -1;
  } else {
    if (pong->xPos >= WIDTH-1 || pong->xPos <= 0) {
      pong->xPos -= 1*pong->xdir;
      pong->xdir *= -1;
    }
    if (pong->yPos >= HEIGHT-1 || pong->yPos <= 0) {
      if (pong->yPos >= HEIGHT/2)
        playerLost = TRUE;
      newRound();
    }
  }
  
  grid[pong->yPos][pong->xPos] = PONG;
}

void drawScene(PlayerObj * p1, PlayerObj * p2)
{
  int i;
  clear_screen();
  printf("\tClassic Pong V0.0.1\n");
  for (i=0; i < HEIGHT; i++)
    printf("\t%s\n", grid[i]);
  printf("\t%s wins: %d | %s wins: %d | %s%d\n"
      , p1->name, p1->wins
      , p2->name, p2->wins
      , "Round ", cur_round);
}

void takePlayerInput(PlayerObj * you, PlayerObj * player2)
{
  static int p1Reset = 0;
  static int p2Reset = 0;
  const char p1[2] = { '.', '/' };
  const char p2[2] = { 'z', 'x' };

  if (p1Reset > 1) {
    you->buttons[0] = you->buttons[1] = 0;
    p1Reset = 0;
  }

  if (player2 && p2Reset > 1) {
    player2->buttons[0] = player2->buttons[1] = 0;
    p2Reset = 0;
  }

  if (kbhit()) {
    int c = getch();
    if (c == p1[0]) {
      you->buttons[0] = 1;
      you->buttons[1] = 0;
      p1Reset++;
     } else if (c == p1[1]) {
      you->buttons[1] = 1;
      you->buttons[0] = 0;
      p1Reset++;
    }
    if (player2) {
      if (c == p2[0]) {
        player2->buttons[0] = 1;
        player2->buttons[1] = 0;
        p2Reset++;
      } else if (c == p2[1]) {
        player2->buttons[1] = 1;
        player2->buttons[0] = 0;
        p2Reset++;
      }
    }
  }

  if (you->buttons[0])
    movePlayer(you, LEFT);
  else if (you->buttons[1])
    movePlayer(you, RIGHT);

  if (player2) {
    if (player2->buttons[0])
      movePlayer(player2, LEFT);
    else if (player2->buttons[1])
      movePlayer(player2, RIGHT);
  }
}

void showTitleScreen()
{
  int i = rand() % NTITLES;
  clear_screen();

  printf("%s\n", titlescreen[i]);
}

int main(int ac, char ** av)
{
  int gamesToWin, nrounds = 3, i;
  PlayerObj you = { WIDTH-11, HEIGHT-4, "Player 1", 0, { 0, 0 } };
  PlayerObj pc = { 2, 3, "CPU", 0, { 0, 0 } };
  Pong pongBall = { 2, 4, 1, 1 };
  Options *opt = NULL;

  srand( time(NULL) );

#ifdef WIN32
  system("color 0A");
#endif

  /* Read number of rounds off the command line. */
  if (ac > 1) {
    nrounds = atoi(av[1]);
    if (nrounds == 0)
      nrounds = 3;
  } else {
    showTitleScreen();
    opt = getMenuOptions();
    if (opt->exit_game)
      goto EXIT;
    nrounds = opt->nrounds;
    strcpy(you.name, opt->playername1);
    if (opt->vsCPU == 0)
      strcpy(pc.name, opt->playername2);
  }

  gamesToWin = nrounds/2 + 1;   /* determines the number of rounds
                                   to win in order to win the series */
  reset(&you, &pc, &pongBall);

  i = 0;
  /* This is main game loop */
  while (1) {
    /* keep score and set up next round */
    if (reset_now) {
      reset(&you, &pc, &pongBall);
      reset_now = FALSE;
      if (playerLost)
        pc.wins += 1;
      else
        you.wins += 1;
      playerLost = FALSE;
    }
   
    /* end the series when there is a clear winner */
    if (cur_round == nrounds+1 ||
        you.wins == gamesToWin ||
        pc.wins == gamesToWin)
      break;
    
    /* display and character movement */
    drawScene(&you, &pc);
    if (!opt || opt->vsCPU) {
      moveAI(&pc, &pongBall);
      takePlayerInput(&you, NULL);
    } else
      takePlayerInput(&you, &pc);

    movePong(&pongBall, &pc, &you);
    delay(11);  /* this delay should be variable */
  }

  free(opt);

  printf("\nGame Over\n");
  if (you.wins > pc.wins)
    printf("You win, %s!\n", you.name);
  else         /* note that there can't be a tie game */
    printf("%s was smitten by %s\n", you.name, pc.name);

  printf("\n--- Results ---\n");
  printf("Played until best %d of %d rounds\n", gamesToWin, nrounds);
  printf("%s :\twins : %d\n", you.name, you.wins);
  printf("%s :\twins : %d\n", pc.name, pc.wins);

#ifdef WIN32
  system("pause");
 EXIT:
  system("cls");
  system("color 07");
#else
  waitForUserInput();
 EXIT:
  system("clear");
#endif

  return 0;
}
