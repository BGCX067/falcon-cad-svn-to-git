//i route circuit boards, yum yum

//1. blob detection
//2. ???
//3. profit

#include "database.h"
#include "router.h"

#include <iostream>
#include <vector>
#include <set>
using namespace std;

extern element_group *rootelement;

void blob::updateBounds()
{

	vector<element_pad *>::iterator elem;
	elem=elements.begin();

	x1=(*elem)->x;
	x2=(*elem)->x+(*elem)->w;

	y1=(*elem)->y;
	y2=(*elem)->y+(*elem)->h;
	elem++;

	while(elem!=elements.end())
	{
		if( x1>(*elem)->x ) x1=(*elem)->x;
		if( x2<((*elem)->x+(*elem)->w) ) x2=(*elem)->x+(*elem)->w;

		if( y1>(*elem)->y ) y1=(*elem)->y;
		if( y2<((*elem)->y+(*elem)->h) ) y2=(*elem)->y+(*elem)->h;

		elem++;
	}

	cout << "bounds are (" << x1 << ", " << y1 << ")-(" << x2 << ", " << y2 << ")" << endl;
}

void blob::draw()
{
	glColor3f(0.15,0.0,0.0);
	glBegin(GL_POLYGON);
		glVertex2i(x1-PROXIMITY_CONSTANT,y1-PROXIMITY_CONSTANT);
		glVertex2i(x2+PROXIMITY_CONSTANT,y1-PROXIMITY_CONSTANT);
		glVertex2i(x2+PROXIMITY_CONSTANT,y2+PROXIMITY_CONSTANT);
		glVertex2i(x1-PROXIMITY_CONSTANT,y2+PROXIMITY_CONSTANT);
	glEnd();

	glColor3f(0.3,0.0,0.0);
	glBegin(GL_POLYGON);
		glVertex2i(x1,y1);
		glVertex2i(x2,y1);
		glVertex2i(x2,y2);
		glVertex2i(x1,y2);
	glEnd();

}

vector <blob *> blobs;
vector <element_pad *> pad_list;

void findPads(element *start)
//build list of all pads in root element
{
	element_group *me_group;
	element_pad *me_pad;

	//cout << "traversing: " << start->name << endl;

	//if group, traverse all
	if( (me_group=dynamic_cast<element_group *>(start)) != 0)
	{
		//cout << "traversing group: " << me_group->name << endl;
		vector<element *>::iterator elem;
		elem=me_group->group.begin();
		while(elem!=me_group->group.end())
		{
			findPads(*elem);
			elem++;
		}
	}

	//if pad, add to list
	if( (me_pad=dynamic_cast<element_pad *>(start)) != 0)
	{
		//cout << "traversing pad: " << me_pad->name << endl;
		pad_list.push_back(me_pad);
	}
}

bool checkProximity(element_pad *first, element_pad *second)
//support only rectangle pads for now
//order isn't important
{
//check Y
	int bl,tl;

	if(first->y < second->y)
	{
		bl=first->y+first->h;
		tl=second->y;
	}
	else
	{
		bl=second->y+second->h;
		tl=first->y;
	}

	if(bl+PROXIMITY_CONSTANT < tl) return false;

//check X
	if(first->x < second->x)
	{
		bl=first->x+first->w;
		tl=second->x;
	}
	else
	{
		bl=second->x+second->w;
		tl=first->x;
	}

	if(bl+PROXIMITY_CONSTANT < tl) return false;

	//cout << "proximity between: " << first->name << " " << second->name << endl;
	return true;
}

void checkProximities(blob *topush, element_pad *firstpad)
{
	topush->elements.push_back(firstpad);

	vector<element_pad *> tocheck;

	vector<element_pad *>::iterator elem;
	elem=pad_list.begin();

	while(elem!=pad_list.end())
	{
		if(checkProximity(firstpad, *elem))
		{
			tocheck.push_back(*elem);
			pad_list.erase(elem);
		}
		else
		{
			elem++;
		}
	}

	elem=tocheck.begin();
	while(elem!=tocheck.end())
	{
		checkProximities(topush, *elem);		//does the push too
		elem++;
	}
}

void findBlobs()
{
//k i know this is a shitty algo( O(n^2) )
//1. build list of all pads
//2. add element k to blob n
//3. check all remaining for proximity
//4. go back to 2 until no elements remain

	blobs.clear();		//erase the old blobs
	pad_list.clear();	//rebuild pad list
	
	findPads(rootelement);
	cout << pad_list.size() << " pads on the board" << endl;

	while(pad_list.size()>0)
	{
		blob *thisblob = new blob;
		blobs.push_back(thisblob);

		element_pad *secondpad=pad_list.back();
		pad_list.pop_back();
		checkProximities(thisblob, secondpad);

		thisblob->updateBounds();
	}

	cout << "found " << blobs.size() << " blobs" << endl;
}

//lines

set<routeline> routelines;

int bx1=1000,bx2=-1000,by1=1000,by2=-1000;

void draw_routeline(int c, int lt)
{
	glColor3f(0.5,0.0,0.0);
	glBegin(GL_LINE_STRIP);
	if(lt==ROUTELINE_X)
	{
		glVertex2i(bx1-10,c);
		glVertex2i(bx2+10,c);
	}
	else if(lt==ROUTELINE_Y)
	{
		glVertex2i(c,by1-10);
		glVertex2i(c,by2+10);
	}
	glEnd();

}


void findLines()
{
	set<routeline> routelines_temp;

	pad_list.clear();
	routelines.clear();
	findPads(rootelement);
	cout << pad_list.size() << " pads on the board" << endl;
	
	vector<element_pad *>::iterator elem;
	elem=pad_list.begin();

	while(elem!=pad_list.end())
	{
		if(bx1>(*elem)->x) bx1=(*elem)->x;
		if(bx2<((*elem)->x+(*elem)->w)) bx2=((*elem)->x+(*elem)->w);
		if(by1>(*elem)->y) by1=(*elem)->y;
		if(by2<((*elem)->y+(*elem)->h)) by2=((*elem)->y+(*elem)->h);

		//routelines.insert( routeline((*elem)->x, ROUTELINE_Y) );
		//routelines.insert( routeline((*elem)->y, ROUTELINE_X) );

		routelines.insert( routeline((*elem)->x+((*elem)->w/2), ROUTELINE_Y) );
		routelines.insert( routeline((*elem)->y+((*elem)->h/2), ROUTELINE_X) );

		//routelines.insert( routeline((*elem)->x+((*elem)->w), ROUTELINE_Y) );
		//routelines.insert( routeline((*elem)->y+((*elem)->h), ROUTELINE_X) );

		elem++;
	}

	/*set<routeline>::iterator relem;
	relem=routelines_temp.begin();
	while(relem!=routelines_temp.end())
	{
		int oldc=(*relem).c;
		int oldlt=(*relem).lt;
		relem++;
		if(oldlt==(*relem).lt)
		{
			routelines.insert( routeline( ((*relem).c+oldc)/2, oldlt ) );
		}
	}*/

	cout << routelines.size() << " lines on the board" << endl;
}


