//this is where chip templates are created

//pass these blank element groups
//i don't think these should be included in the element_group class

//maybe element_chip should be a subclass of element_group
//nah, for now, we'll just create them

#include "database.h"
#include "hatchery.h"

//2flat is SOIC, TSSOP, TSOP
//4flat is QFP
//ga is BGA, LGA

#include <sstream>

string itos(int s)
{
  ostringstream out;
  out << s;
  return out.str();
}

const char rowname[21][3]={"A","B","C","D","E","F","G","H","J","K","L","M","N","P","R","T","U","V","W","Y","AA"};


void create_2flat(element_group *my, int count, int pad_w, int pad_h, int pad_spacing, int row_spacing)
{
	int a;
	int cx=0,cy=0;
	for(a=1;a<=count;a++)
	{
		my->add( new element_pad(itos(a), cx, cy, pad_w, pad_h) );
		if(a<(count/2)) cy-=pad_spacing;
		else if(a>(count/2)) cy+=pad_spacing;
		else if(a==(count/2)) cx+=row_spacing;
	}
}

/*void create_4flat(element_group *my*/

void create_ga(element_group *my, int count_w, int count_h, int pad_d, int spacing)
{
	int w, h;
	for(h=0;h<count_h;h++)
	{
		string rowstring=rowname[h];
		for(w=0;w<count_w;w++)
		{
			//make round a choice
			my->add( new element_pad(rowstring+itos(w+1), w*spacing, -(h*spacing), pad_d, pad_d, true) );
		}
	}
}
