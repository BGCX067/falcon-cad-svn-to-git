#include <algorithm>

#include "elementlist.h"

vector<elementlist *> elementlists;		//all elementlists currently enumerated
extern vector<element *> selection;	

#define ELEMENTLIST_HEIGHT 640
#define ELEMENTLIST_WIDTH 160

#define FONT_PIXEL_HEIGHT 12

#define ROW_OFFSET 12

vector<element_group *> collapsed;

//glut helper function
void glutBitmapString(void *font, const char *message)		//boo
{
	while (*message) {
		glutBitmapCharacter(font, *message++);
		//glutStrokeCharacter(GLUT_STROKE_ROMAN,*message++);
	}
}

//handle the mouse
void el_mousehandler(int button, int state, int x, int y)
{
	//assume elementlists 0
	//cout << elementlists[0]->irep.size() << " elements vs " << y/FONT_PIXEL_HEIGHT << endl;

	cout << "el_mousehandler: " << button << ", " << state << endl;
	
	
	if(elementlists[0]->irep.size() > (y/FONT_PIXEL_HEIGHT))		//sanity is good
	{
		if(button==0&&state==1)	//left click up
		{
			cout << "click like adam sandler on " << elementlists[0]->irep[y/FONT_PIXEL_HEIGHT]->name << endl;
			if( !(glutGetModifiers()&GLUT_ACTIVE_CTRL) )
				selection.clear();
			selection.push_back(elementlists[0]->irep[y/FONT_PIXEL_HEIGHT]);
			de_redraw();	//and redraw the shit
		}
		
		if(button==2&&state==1)	//right click up
		{
			element_group *toc;
			if( toc=dynamic_cast<element_group *>(elementlists[0]->irep[y/FONT_PIXEL_HEIGHT]) )
			{
				if( find(collapsed.begin(), collapsed.end(), toc) == collapsed.end() )
					collapsed.push_back(toc);
				else
					collapsed.erase(std::remove(collapsed.begin(), collapsed.end(), toc));
					
				de_redraw();	//and redraw the shit
			}
		}
	}
}

elementlist::elementlist(element_group *linked_setup)
{
	printf("constructing element list\n");
	glutInitWindowSize(ELEMENTLIST_WIDTH,ELEMENTLIST_HEIGHT);
	window=glutCreateWindow("Element List");
	glutDisplayFunc(de_redraw);
	glutMouseFunc(el_mousehandler);
	glutKeyboardFunc(de_kbhandler);	//limit this
	printf("elementlists: %d\n", elementlists.size());
	elementlists.push_back(this);		//this is already a pointer i think
	printf("elementlists: %d\n", elementlists.size());
	linked=linked_setup;
}

//recursive crap
int xpos, ypos;

void elementlist::draw_recursive(element *todraw)		//not recursive yet
{
	irep.push_back(todraw);
//draw the local element
	ypos+=FONT_PIXEL_HEIGHT;
	if(isSelected(todraw))
	{
		glColor3f(1,.8,.8);
		glBegin(GL_POLYGON);
			glVertex2i(0,ypos);
			glVertex2i(ELEMENTLIST_WIDTH,ypos);
			glVertex2i(ELEMENTLIST_WIDTH,ypos-ROW_OFFSET);
			glVertex2i(0,ypos-ROW_OFFSET);
		glEnd();
	}

	glColor3f(0.0,0.0,0.0);		//words are black
	glRasterPos2f((GLfloat)xpos, (GLfloat)ypos);
	glutBitmapString(GLUT_BITMAP_HELVETICA_12, todraw->name.c_str());
	if(todraw->net!=0)
	{
		glutBitmapString(GLUT_BITMAP_HELVETICA_12, " : ");
		glutBitmapString(GLUT_BITMAP_HELVETICA_12, todraw->net->name.c_str());
	}

//now check if theres more in the group
	element_group *todrawgroup;
	if( (todrawgroup = dynamic_cast<element_group *>(todraw)) !=0 )
	{
		if( find(collapsed.begin(), collapsed.end(), todrawgroup) == collapsed.end() )
		{
			vector<element *>::iterator elem;		//foreach
			elem=todrawgroup->group.begin();
			xpos+=ROW_OFFSET;
			while(elem!=todrawgroup->group.end())
			{
				draw_recursive(*elem);
				elem++;
			}
			xpos-=ROW_OFFSET;
		}
	}
}

void elementlist::draw()
{
	glutSetWindow(window);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0,ELEMENTLIST_WIDTH,ELEMENTLIST_HEIGHT,0);

	glClearColor(1,1,1,0);
	glClear(GL_COLOR_BUFFER_BIT);

	glColor3f(0.0,0.0,0.0);

	xpos=0;
	ypos=0;

	irep.clear();
	draw_recursive(linked);

/*	glRasterPos2f((GLfloat)0, (GLfloat)FONT_PIXEL_HEIGHT*2);
	glutBitmapString(GLUT_BITMAP_HELVETICA_12, "at that");*/

	glFlush();
	glutSwapBuffers();
}
