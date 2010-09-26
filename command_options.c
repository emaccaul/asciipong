/**********************************************************************
 * command_options.c
 * description : user interface main menu for pong
 * note : some of this stuff is windows specific, namely conio.h and
 *        _getch()
 *********************************************************************/
#include <stdlib.h> 
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "keyboard.h"
#include "command_options.h"
#include "timer.h"

void clear_screen()
{
#ifdef WIN32
  system("cls");
#else
  system("clear");
#endif
}

/* waits for user to input <RETURN> or EOF */
void waitForUserInput()
{
  int ch;
  printf("Press Enter to continue ...");
  while ((ch = getchar()) != '\n' && ch != EOF)
    ;
}

static void getInputToEOL(char *buffer, int size)
{
  int c, i=0;
  while ((c=getchar()) != '\n' && c != EOF) {
    if (i < size && c != '\n') {
      buffer[i] = c;
      putchar(c);
      i++;
    }
  }

  buffer[i] = 0;
  putchar('\n');
}

static int getOption(int withClearScreen)
{
  if (withClearScreen)
    clear_screen();

  printf("Options:\n"
         "1) Start Game!\n"
         "2) Change your player name\n"
         "3) Add Second Player\n"
         "4) Select game length\n"
         "5) Help\n"
         "6) Quit\n"
         "> ");

  return getche();
}

static void setPlayerName(char *playername)
{
  clear_screen();
  printf("Enter your new name, %s: ", playername);
  getInputToEOL(playername, PLAYER_NAME_LEN);
  printf("Welcome to ascii pong, %s\n", playername);
  delay(2000);
}

static void addP1(Options *opt)
{
  setPlayerName(opt->playername1);
}

static void addP2(Options *opt)
{
  setPlayerName(opt->playername2);
  opt->vsCPU = 0;
}

static void setRounds(Options *opt)
{
  char buf[32];
  clear_screen();
  printf("To the best of how many rounds, %s? "
         ,(opt->playername1[0]) ? opt->playername1 : "Player 1");
  getInputToEOL(buf, sizeof(buf));
  opt->nrounds = atoi(buf);
  if (opt->nrounds == 0)
    opt->nrounds = 1;
  printf("Playing to best of %d rounds\n", opt->nrounds);
  delay(1000);
}

static void help()
{
  clear_screen();
  printf("Help Section:\n"
         "  User Controls:\n"
         "      first player : '.' moves you left, '/' moves you right\n"
         "      second player: 'z' moves you left, 'x' moves you right\n");
  waitForUserInput();
}

static void initOptions(Options *opt)
{
  opt->nrounds = 3;     /* default to best of 3 */
  opt->vsCPU   = 1;
  opt->exit_game = 0;
  opt->playername1[0] = 0;
  opt->playername2[0] = 0;
  strncpy(opt->playername1, "Player 1", PLAYER_NAME_LEN);
  opt->playername1[PLAYER_NAME_LEN-1] = 0;
  strncpy(opt->playername2, "Player 2", PLAYER_NAME_LEN);
  opt->playername2[PLAYER_NAME_LEN-1] = 0;
}

Options *getMenuOptions()
{
  int n, done=0, i=0;
  Options  *opt = (Options *)malloc(sizeof(Options));
  assert(opt != NULL);

  initOptions(opt);

  while (!done) {
    n = getOption(i);
         
    switch (n) {
    case '1':
      done = 1;
      break;
    case '2':
      addP1(opt);
      break;
    case '3':
      addP2(opt);
      break;
    case '4':
      setRounds(opt);
      break;
    case '5':
      help();
      break;
    case '6':
      opt->exit_game = 1;
      done = 1;
      break;
    case 0:
    default:
      clear_screen();
      printf("You entered an invalid command\n");
      delay(1000);
    }
    i = 1;
  }  

  return opt;
}
