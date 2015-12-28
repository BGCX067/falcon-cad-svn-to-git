//Falcon by George Hotz(geohot)

//This is the C based glut runner
//All the messages are sent here

//This is the main program loop 


#include <stdio.h>

#include "drawing.h"
#include "database.h"
#include "elementlist.h"
#include "lexer.h"
#include "cmd.h"
#include "router.h"

#include <set>
#include <algorithm>
using namespace std;

extern vector <blob *> blobs;

extern vector<element *> selection;		//should this be an extern

extern set<routeline> routelines;

//moving
bool moving;
int rx=0, ry=0;

//last keyboard command coords
int kx=0, ky=0;

//selection
bool mousedown;
int lx1,ly1;	//,lx2,ly2;	//selection box bounds
bool lineselect=true;

class vertex
{
public:
	vertex(int lx, int ly)
	{
		x=lx;
		y=ly;
	}
	int x,y;
};

vector<vertex> vertices;

int lx, ly;		//last mouse click coords
int cx, cy;		//current coords

element_group *rootelement;

extern vector<elementlist *> elementlists;

double de_x, de_y;
int de_w, de_h;
double de_zoomlevel;

#define ZOOM_SPEED 1.3

//int oldx, oldy;

element_net * findselectednet()
{
	element_net *mynet=0;
	vector<element *>::iterator elem;
	elem=selection.begin();
	while(elem!=selection.end())
	{
		mynet=dynamic_cast<element_net *>(*elem);
		if(mynet>0) break;
		elem++;
	}

	return mynet;
}

void de_updatewindow()
{
	//printf("Drawing at %f, %f with %d, %d and zoom of %f\n", de_x, de_y, de_w, de_h, de_zoomlevel);
	//printf("Screen is %f, %f, %f, %f\n", (de_x-(de_w/2.))/de_zoomlevel, (de_x+(de_w/2.))/de_zoomlevel, (de_y-(de_h/2.))/de_zoomlevel, (de_y+(de_h/2.))/de_zoomlevel);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D((de_x-(de_w/2.))/de_zoomlevel, (de_x+(de_w/2.))/de_zoomlevel, (de_y-(de_h/2.))/de_zoomlevel, (de_y+(de_h/2.))/de_zoomlevel);
}

void de_redraw()
{
//draw all elementlists
	//cout << "drawing " << elementlists.size() << " elements" << endl;
	vector<elementlist *>::iterator elem;		//foreach
	elem=elementlists.begin();
	while(elem!=elementlists.end())
	{
		(*elem)->draw();
		elem++;
	}
//now draw the main window
	glutSetWindow(de_window);
	de_updatewindow();
	glClear(GL_COLOR_BUFFER_BIT);

	

	//draw blobs
	vector<blob *>::iterator belem;
	belem=blobs.begin();
	while(belem!=blobs.end())
	{
		(*belem)->draw();
		belem++;
	}

	//draw routing lines
	set<routeline>::iterator relem;
	relem=routelines.begin();
	while(relem!=routelines.end())
	{
		draw_routeline((*relem).c, (*relem).lt);
		relem++;
	}

	rootelement->draw(0,0);	//draw everything under the root element at the origin

//on top
	if(mousedown==true)		//draw white box thing
	{
		glutUseLayer(GLUT_OVERLAY);
		glColor3f(1.0,1.0,1.0);
		if(lineselect==false)
		{
			glBegin(GL_LINE_LOOP);
				glVertex2i(lx1,ly1);
				glVertex2i(cx,ly1);
				glVertex2i(cx,cy);
				glVertex2i(lx1,cy);
			glEnd();
			//cout << "box at " << lx1 << ", " << ly1 << "  -  " << cx << ", " << cy << endl;
		}
		else
		{
			vector<element *> a;
			if(vertices.size()>0)
				rootelement->collision(&a,  cx, cy, vertices[vertices.size()-1].x, vertices[vertices.size()-1].y);
			else
				rootelement->collision(&a, cx, cy, cx, cy);

			vertices.push_back(*(new vertex(cx, cy)));

			if(a.size()==1)
			{
				if(find(selection.begin(), selection.end(), a[0])==selection.end())
				{
					cout << "collided with " << a[0]->name << endl;
					selection.push_back(a[0]);
				}
			}
			else if(a.size()>1) cout << "error: multiple select not allowed" << endl;

			glBegin(GL_LINE_STRIP);
			//glBegin(GL_POINTS);
				vector<vertex>::iterator elem;		//foreach
				elem=vertices.begin();
				while(elem!=vertices.end())
				{
					glVertex2i((*elem).x,(*elem).y);
					elem++;
				}
			glEnd();
		}
		glutUseLayer(GLUT_NORMAL);
	}

	glFlush();

	glutSwapBuffers();
}

void de_setrootelement(element_group *lroot)
{
	rootelement=lroot;
}

void de_kbhandler(unsigned char key, int x, int y)
{
	//should be better modularized
	cout << "You pressed: " << (int)key << endl;

	x=x-(de_w/2);
	y=(de_h/2)-y;

	double fx=(x+de_x)/de_zoomlevel;
	double fy=(y+de_y)/de_zoomlevel;

//toggle line vs box selection mode(line selection mode preserves order
	if(key=='l' && mousedown==false)
	{
		//lineselect=~lineselect;		//wtf?
		if(lineselect==true) lineselect=false;
		else lineselect=true;
		cout << "lineselect is " << lineselect << endl;
	}

//pad command
	if(key=='p' && mousedown==false)
	{
		rootelement->add(new element_pad("P6",(int)fx,(int)fy,10,20));	
	}

//move
	if(key=='m' && mousedown==false) 
	{
		moving=true;
		rx=0;
		ry=0;
		kx=(int)fx;
		ky=(int)fy;
	}

//group
	if(key=='g')
	{
		if(selection.size()>0)
		{
			element_group *mygroup=new element_group("G",0,0);		//shouldn't be on stack'
			//for_each(selection.begin(), selection.end(), mygroup->add);			
				vector<element *>::iterator elem;
				elem=selection.begin();
				while(elem!=selection.end())
				{
					mygroup->add(*elem);
					elem++;
				}

			rootelement->add(mygroup);
		}
	}

//add to net
	if(key=='n')
	{
		if(selection.size()>0)
		{
			element_net *mynet=findselectednet();
			if(mynet==0) mynet=new element_net("N",0,0);
				vector<element *>::iterator elem;
				elem=selection.begin();
				while(elem!=selection.end())
				{
					if(dynamic_cast<element_pad *>(*elem))
					{
						(*elem)->net=mynet;
						mynet->add(*elem);
					}
					elem++;
				}
			rootelement->add(mynet);
		}
	}

//draw trace
	if(key=='z')
	{
		element_net *mynet=findselectednet();
		//found net we are currently routing on
		if(mynet>0)
		{
			cout << "routing on " << mynet->name << endl;
		}
	}

//deselect
	if(key==27)		//escape
	{
		moving=false;
		rx=0;
		ry=0;
		selection.clear();
	}

//delete
	if(key==127)
	{
		cmd_delete();
	}

	de_redraw();

}

void de_motionhandler(int x, int y)
{

	x=x-(de_w/2);
	y=(de_h/2)-y;

	double fx=(x+de_x)/de_zoomlevel;
	double fy=(y+de_y)/de_zoomlevel;
	//printf("at %f, %f on coord plane, last click at %d, %d\n", fx,fy,lx,ly);

	cx=(int)fx;
	cy=(int)fy;

	rx=cx-lx;
	ry=cy-ly;

	de_redraw();
}

void de_mousehandler(int button, int state, int x, int y)
{

	//y=-y;

	x=x-(de_w/2);
	y=(de_h/2)-y;

	double fx=(x+de_x)/de_zoomlevel;
	double fy=(y+de_y)/de_zoomlevel;

	printf("at %f, %f on coord plane\n", fx,fy);

	//0 is left click
	//4 is zoom out
	//5 is zoom in


//**Adjust the zoomlevel**
	if(button==5&&state==0) 
	{
		de_zoomlevel*=ZOOM_SPEED;
	}
	else if(button==4&&state==0)
	{ 
		de_zoomlevel/=ZOOM_SPEED;

	} //min zoom is one pad per pixel

//**Update the zoomlevel**
	de_x-=(((x+de_x)/de_zoomlevel)-fx)*de_zoomlevel;
	de_y-=(((y+de_y)/de_zoomlevel)-fy)*de_zoomlevel;

//**Selection engine**
	if(button==0&&state==0)		//mousedown
	{
		lx1=(int)fx;
		ly1=(int)fy;
		mousedown=true;
		glutEstablishOverlay();
		vertices.clear();
		if( lineselect==true && !(glutGetModifiers()&GLUT_ACTIVE_CTRL) )
			selection.clear();

		if(moving==true)		//save move
		{
			moving=false;
			vector<element *>::iterator elem;
			elem=selection.begin();
			while(elem!=selection.end())
			{
				(*elem)->move(rx,ry);
				elem++;
			}
			//selection.clear();
		}
	}
	else if(button==0&&state==1)	//mouseup
	{
		lx=(int)fx;
		ly=(int)fy;

		mousedown=false;
		glutRemoveOverlay();

		if(lineselect==false)
		{
			//printf("Bounding box: %d, %d   %d, %d\n", lx1, ly1, lx2, ly2);
			if( !(glutGetModifiers()&GLUT_ACTIVE_CTRL) )
				selection.clear();
			rootelement->collision(&selection, lx1,ly1,(int)fx,(int)fy);
		}
	}

	glutPostRedisplay();

}

void de_changesize(int w, int h)
{
	de_w=w;
	de_h=h;
	glViewport(0,0,de_w, de_h);
	glutPostRedisplay();
}

void de_idle()
{
	lx_service();
}

void de_initwindow()		//constructor creates the window
{
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

	glEnable(GL_POLYGON_SMOOTH);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_DONT_CARE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	glutInitWindowSize(640,640);
	de_x=0; de_y=0;		//centered at startup
	de_w=640; de_h=640;
	de_zoomlevel=1;

	de_window=glutCreateWindow("Falcon");

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glutDisplayFunc(de_redraw);		//it's c, are you happy yet?
	glutMouseFunc(de_mousehandler);
	glutPassiveMotionFunc(de_motionhandler);
	glutMotionFunc(de_motionhandler);
	glutKeyboardFunc(de_kbhandler);
	glutReshapeFunc(de_changesize);

	glutIdleFunc(de_idle);
}
