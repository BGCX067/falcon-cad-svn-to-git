#ifndef __DRAWING_H
#define __DRAWING_H

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
//#include <GL/freeglut.h>

#include "database.h"

static int de_window;

void de_initwindow();
void de_redraw();
void de_kbhandler(unsigned char key, int x, int y);
void de_setrootelement(element_group *lroot);

#endif
