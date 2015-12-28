#ifndef __ELEMENTLIST_H
#define __ELEMENTLIST_H

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include "database.h"
#include "drawing.h"

#include <iostream>
#include <vector>
#include <typeinfo>
using namespace::std;

class elementlist
{
public:
	elementlist(element_group *);
	void draw();
	vector<element *> irep;
private:
	int window;
	element_group *linked;
	void draw_recursive(element *);
};

//static doesn't work

#endif
