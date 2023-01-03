#ifndef _ANSI_H
#define _ANSI_H

#ifdef _WIN32
#include <windows.h>
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING  0x0004
#endif
static void setANSI() {
  HANDLE stdoutHandle = GetStdHandle(STD_OUTPUT_HANDLE);
  DWORD outMode;
  GetConsoleMode(stdoutHandle, &outMode);
  outMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
  SetConsoleMode(stdoutHandle, outMode);
}
#else
void setANSI() { }
#endif

#endif