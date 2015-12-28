#ifndef __DATABASE_H
#define __DATABASE_H

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

void de_drawpad(int x, int y, int w, int h);

//3 base element types

#include <iostream>
#include <string>
#include <vector>
using namespace::std;

#define ELEMENT_GROUP 1
#define ELEMENT_PAD 2
#define ELEMENT_NET 3		//replaced by group?

class element;
class element_group;	//prototyping for parent element
class element_net;

int isSelected(element *);

//element is the root class
class element
{
public:
	element();
	element(string, int, int);
	virtual ~element();
	virtual void draw(int, int);	//checks type and draws
	virtual int collision(vector<element *> *, int, int, int, int);
	string name;
	element_group *parent;
	element_net *net;		//move this to element_pad *?
	void move(int, int);
//protected:
	int x,y;
	int layer;
	//int type;	//this really shouldn't have to be here, and thanks to virtual functions, it isn't
};

//element_group is a group of elements, root is the base element
class element_group : public element
{
public:
	element_group(string, int, int);
	void add(element *);
	void remove(element *);
	void draw(int, int);	//iterate through elements and call their draw function
	int collision(vector<element *> *, int, int, int, int);
	int connected;	//all contained elements are electrically connected
//private:
	vector<element *> group;
};

//element_pad is a pad on the board
class element_pad : public element
{
public:
	element_pad(string, int, int, int, int);
	element_pad(string, int, int, int, int, bool);
	void draw(int, int);	//every element has a different draw function
	int collision(vector<element *> *, int, int, int, int);
//private:
	int w,h;
	bool round;
};

struct trace		//only exists in nets
{
	int x1,y1;
	int x2,y2;
};

//element_net connects multiple pads together
class element_net : public element
{
public:
	element_net(string, int, int);
	~element_net();
	void draw(int, int);
	void add(element *);
	int collision(vector<element *> *, int, int, int, int);
private:
	vector<element *> pads;	//really can only be pads
	vector<trace> traces;		//not pointers
};

#endif
