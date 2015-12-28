#ifndef __LEXER_H
#define __LEXER_H

#include <windows.h>

void lx_boot();
DWORD WINAPI lx_run(void *);

void lx_service();		//must be within the glut thread context

#endif
