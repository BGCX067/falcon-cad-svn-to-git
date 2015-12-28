#ifndef __ROUTER_H
#define __ROUTER_H

class blob		//this is a blob of elements
{
public:
	vector <element_pad *> elements;
	void updateBounds();
	void draw();
private:
	int x1,y1,x2,y2;
};

void findBlobs();

#define PROXIMITY_CONSTANT 8

#define ROUTELINE_X 0
#define ROUTELINE_Y 1

class routeline			//this is a blob of elements
{
public:
	routeline(int lc, int llt)
	{
		c=lc;
		lt=llt;
	}
	//void draw();
	bool operator<(const routeline a) const
	{
		if(lt==a.lt) return c<(a.c);
		else return lt<(a.lt);
	}
	int c,lt;	//coord line type
};

void draw_routeline(int c, int lt);

void findLines();

#endif
