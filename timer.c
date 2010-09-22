/* this was originally intended to be a timing library of sorts */

#include "timer.h"

#ifdef WIN32

# include <windows.h>

void delay(int msec)
{
  Sleep(msec);
}

#else
# include <unistd.h>

void delay(int msec)
{
  usleep(msec * 1000);
}

#endif
