//this is the main falcon file

//#include "falcon.h"
#include "lexer.h"
#include "drawing.h"
#include "database.h"
#include "elementlist.h"
#include "hatchery.h"

int main()
{
	
	element_group root("root",0,0);

//set up the fake board
	/*root.add(new element_pad("P1",0,0,10,20));
	root.add(new element_pad("P2",20,0,10,20));
	root.add(new element_pad("P3",40,0,10,20));
	root.add(new element_pad("P4",60,0,10,20));
	root.add(new element_pad("P5",80,0,10,20));*/

	element_group *g=new element_group("FT232RL",0,0);
	create_2flat(g, 28, 25, 6, 13, 131);	//.05 mm(this is our grid forever)

	//element_group *g=new element_group("ATOM",0,0);
	//create_ga(g, 21, 21, 10, 20);

	root.add(g);

	//element_group *h=new element_group("FT232RL",0,0);
	//create_2flat(h, 28, 25, 6, 13, 131);	//.05 mm(this is our grid forever)

	//root.add(h);

	de_setrootelement(&root);
	de_initwindow();
	
	new elementlist(&root);


	lx_boot();		//start the lexer

	glutMainLoop();
	return 0;
}
