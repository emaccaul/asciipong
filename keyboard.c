#include "keyboard.h"

#ifdef WIN32

#include <conio.h>

int kbhit(void)
{
  return _kbhit();
}

int getch(void)
{
  return _getch();
}

int getche(void)
{
  return _getche();
}

#else /* !WIN32 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>

/* Use this variable to remember original terminal attributes. */
static struct termios saved_attributes;

static void reset_input_mode(void)
{
    tcsetattr(STDIN_FILENO, TCSANOW, &saved_attributes);
}

static void set_input_mode(void)
{
    struct termios tattr;

    /* Make sure stdin is a terminal.  */
    if (!isatty(STDIN_FILENO)) {
        fprintf(stderr, "Not a terminal.\n");
        exit(EXIT_FAILURE);
    }

    /* Save the terminal attributes so we can restore them later.  */
    tcgetattr(STDIN_FILENO, &saved_attributes);
    atexit(reset_input_mode);

    /* Set the funny terminal modes.  */
    tcgetattr(STDIN_FILENO, &tattr);
    tattr.c_lflag &= ~(ICANON | ECHO); /* Clear ICANON and ECHO.   */
    tattr.c_cc[VMIN] = 1;
    tattr.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &tattr);
}

int kbhit(void)
{
  struct timeval tv;
  fd_set fds;
  tv.tv_sec = 0;
  tv.tv_usec = 0;
  FD_ZERO(&fds);
  FD_SET(STDIN_FILENO, &fds);
  if (select(STDIN_FILENO+1, &fds, NULL, NULL, &tv) == -1) {
    return 0;
  }
  return FD_ISSET(STDIN_FILENO, &fds);
}

int getch(void)
{
  static int initialized = 0;

  if (!initialized) {
    set_input_mode();
    initialized = 1;
  }

  char c = 0;
  if (read(STDIN_FILENO, &c, 1) == -1) {
    perror("read");
  }
  return c;
}

int getche(void)
{
  int c = getch();
  putc(c, stderr);
  return c;
}

#endif
