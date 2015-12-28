#include "database.h"
#include <typeinfo>
#include <vector>
#include <algorithm>
using namespace std;

#include <math.h>

#include "router.h"

//current state
vector<element *> selection;

//extern moving stuff
extern bool moving;
extern int rx, ry;

//element

element::element()
{
	printf("attempting to create empty element, FAILURE\n");
}

element::element(string lname, int lx, int ly)		//root constructor
{
	x=lx;		//all things come to life here?
	y=ly;
	name=lname;
	parent=0;	//no parent when first created
	net=0;		//no net either
	cout << "created element " << name << " at " << x << ", " << y << endl;
}

element::~element()
{
	cout << "generic element just died" << endl;
}


void element::move(int mx, int my)
{
	x+=mx;
	y+=my;
}

void element::draw(int offset_x, int offset_y)
{
	cout << "I eat like a generic element" << endl;
	//printf("attempting to draw generic element, FAILURE\n");
	//if(type==ELEMENT_PAD) ((element_pad *)this)->draw(offset_x,offset_y);
	//if(type==ELEMENT_GROUP) ((element_group *)this)->draw(offset_x,offset_y);
	//if(type==ELEMENT_NET) ((element_net *)this)->draw(offset_x,offset_y);
	//cout << "Type: " << typeid(this).name() << endl;
}

int element::collision(vector<element *> *v, int lx1, int ly1, int lx2, int ly2)
{
	cout << "this is virtual, dipshit" << endl;
}

//element_group

element_group::element_group(string lname, int lx, int ly) : element(lname, lx, ly)
{
	//type=ELEMENT_GROUP;
}

void element_group::add(element *my)
{
	if(my->parent!=0) my->parent->remove(my);

	printf("adding element, current size is %d\n",group.size());
	group.push_back(my);
	my->parent=this;
	printf("new size is %d\n", group.size());
}

void element_group::remove(element *my)
{
	group.erase(std::remove(group.begin(), group.end(), my));
}

void element_group::draw(int offset_x, int offset_y)
{
	//cout << "drawing element group " << name << " of length " << group.size() << endl;
	vector<element *>::iterator elem;
	elem=group.begin();
	while(elem!=group.end())
	{
		//cout << "drawing " << group[i]->name << endl;
		//if((*elem)==0) { cout << "deleted element found" << endl; group.erase(elem); }	//must have been deleted(this is a hack)
		(*elem)->draw(offset_x+x,offset_y+y);
		elem++;
	}
}

int element_group::collision(vector<element *> *v, int lx1, int ly1, int lx2, int ly2)
{
	int ret=0;
	vector<element *>::iterator elem;
	elem=group.begin();
	while(elem!=group.end())
	{
		ret+=(*elem)->collision(v, lx1-x, ly1-y, lx2-x, ly2-y);
		elem++;
	}
	return ret;
}

//element_pad

element_pad::element_pad(string lname,int lx, int ly, int lw, int lh) : element(lname, lx, ly)
{
	w=lw;
	h=lh;
	round=false;
	//type=ELEMENT_PAD;
}

element_pad::element_pad(string lname,int lx, int ly, int lw, int lh, bool lround) : element(lname, lx, ly)
{
	w=lw;
	h=lh;
	round=lround;
	//type=ELEMENT_PAD;
}

int isSelected(element *me)		//helper function
{
	vector<element *>::iterator elem;
	elem=selection.begin();
	while(elem!=selection.end())
	{
		if((*elem)==me) return 1;
		elem++;
	}
	return 0;
}

int isSelectedRecurse(element *me, vector<element *> g, int depth)		//helper function
{
	depth++;
	vector<element *>::iterator elem;
	elem=g.begin();
	element_group *mygroup=0;
	int r=0;
	while(elem!=g.end())
	{
		if((*elem)==me) return depth;
		mygroup=dynamic_cast<element_group *>(*elem);
		if(mygroup!=0) 
		{
			r=isSelectedRecurse(me, mygroup->group, depth);
			if(r>0) return r;		//selected in group
		}
		elem++;
	}
	return 0;
}

/*#define NU_ANGLESTEP		M_PI/180.0

void DrawCircle(double xctr, double yctr, double radius)
{
	double vectorX,vectorY;			// vector to a point on circle from its center
	double angle;					// Angle in radians from circle start point.

	glBegin(GL_LINE_STRIP);		// Tell OpenGL to draw a series of lines:
	for(angle=0; angle < 2.0*M_PI + NU_ANGLESTEP; angle+= NU_ANGLESTEP)			
	{								// (>2PI so that circle is always closed)
		vectorX= xctr + radius * cos(angle);	// set line endpoint
		vectorY= yctr + radius * sin(angle);		
		glVertex2d(vectorX,vectorY);	// plot the line endpoint.
	}
	glEnd();						// finished drawing line segments.
	glFlush();						// Finish any pending drawing commands
}*/

void element_pad::draw(int offset_x, int offset_y)
{
	int sel=isSelectedRecurse(this, selection,0);

	if(sel>1)
		glColor3f(0.5,1.0,0.0);
	else if(sel==1)
		glColor3f(0.0,1.0,0.0);		//pads are green
	else
	{
		if(find(selection.begin(), selection.end(), this->net)==selection.end())
			glColor3f(0.0,0.5,0.0);		//pads are green
		else
			glColor3f(0.0,1.0,0.5);		//selected net
	}
	if(sel>0&&moving==true)
	{
		offset_x+=rx;
		offset_y+=ry;
	}
	//if(round==false)
	//{
		glBegin(GL_POLYGON);
			glVertex2i(offset_x+x,offset_y+y);
			glVertex2i(offset_x+x,offset_y+y+h);
			glVertex2i(offset_x+x+w,offset_y+y+h);
			glVertex2i(offset_x+x+w,offset_y+y);
		glEnd();
	//}
	//else
	//{
	//	DrawCircle(offset_x+x, offset_y+y, w/2);
	//}
}

int element_pad::collision(vector<element *> *v, int lx1, int ly1, int lx2, int ly2)
{
	//lx2 must be bigger
	if(lx1>lx2) { int tx=lx2; lx2=lx1; lx1=tx; }

	//ly2 must be bigger
	if(ly1>ly2) { int ty=ly2; ly2=ly1; ly1=ty; }

	if( (x<lx2) && (lx1<(x+w+1)) &&
	 (y<ly2) && (ly1<(y+h+1)) ) 
	{
		v->push_back(this);
		return 1;
	}
	return 0;
}

//element_net

element_net::element_net(string lname,int lx, int ly) : element(lname, lx, ly)
{
	//type=ELEMENT_NET;
}

element_net::~element_net()
{
	cout << "destructing element net" << endl;
	vector<element *>::iterator elem;
	elem=pads.begin();
	while(elem!=pads.end())
	{
		(*elem)->net=0;
		elem++;
	}
}

void element_net::draw(int offset_x, int offset_y)
//only draws the traces, assumes that the pads are already drawn
//all traces
{
	//cout << "i'm drawing a net, hahaha" << endl;
//draw ratsnets
	element_pad *drawme;
	glColor3f(1.0,1.0,0.0);
	glBegin(GL_LINE_STRIP);
		vector<element *>::iterator elem;
		elem=pads.begin();
		while(elem!=pads.end())
		{
			drawme=dynamic_cast<element_pad *>(*elem);
			if(drawme!=0)
				glVertex2i(drawme->x+(drawme->w/2),drawme->y+(drawme->h/2));
			elem++;
		}
	glEnd();
}

void element_net::add(element *my)		//simple
{
	pads.push_back(my);
}

int element_net::collision(vector<element *> *v, int lx1, int ly1, int lx2, int ly2)
{
	//collide with a net...
	return 0;
}
