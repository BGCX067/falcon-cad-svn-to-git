#include "database.h"
#include "cmd.h"
#include "router.h"

extern element_group *rootelement;
extern vector<element *> selection;	

//helper functions

int stoi(string s)
{
  istringstream myStream(s);
  int i;
  myStream>>i;
  return i;
}

void selectwithname(string thisname)	//support dot notaion the future
{
	vector<element *>::iterator elem;
	elem=rootelement->group.begin();
	while(elem!=rootelement->group.end())
	{
		if((*elem)->name==thisname) selection.push_back(*elem);
		elem++;
	}
}

void printinfo(element *me)
{
	cout << "full name:  " << me->name;
	element *bp=me->parent;
	while(bp!=0)
	{
		cout << "<-" << bp->name;
		bp=bp->parent;
	}
	cout << endl;
	cout << "  location: " << me->x << ", " << me->y << endl;
	cout << "  type:     ";
	element_pad *me_pad;
	element_group *me_group;
	if( ( me_pad=dynamic_cast<element_pad *>(me) ) != 0)
	{
		cout << "pad" << endl;
		cout << "    size:   " << me_pad->w << ", " << me_pad->h << endl;
	}
	else if( ( me_group=dynamic_cast<element_group *>(me) ) != 0)
	{
		cout << "group" << endl;
		cout << "    childs: ";
		vector<element *>::iterator elem;
		elem=me_group->group.begin();
		while(elem!=me_group->group.end())
		{
			cout << (*elem)->name << " ";
			elem++;
		}
		cout << endl;
	}
	else cout << "unknown" << endl;

}

//commands that don't need anything
void cmd_help()
{
	cout << "fuck you" << endl;
}

//commands that operate on selection
void cmd_info()
{
	vector<element *>::iterator elem;
	elem=selection.begin();
	while(elem!=selection.end())
	{
		printinfo(*elem);
		elem++;
	}
}

void cmd_delete()
{
	vector<element *>::iterator elem;
	elem=selection.begin();
	while(elem!=selection.end())
	{
		if(*elem!=rootelement)
		{
			(*elem)->parent->remove(*elem);	//tell the parent your kid is dead
			delete (*elem);	//delete the element itself
		}
		else
			cout << "You can't delete the root element\n" << endl;
		elem++;
	}
	selection.clear();
	cout << "deleted" << endl;
}

void cmd_rename(vector<string> argv)
{
	if(argv.size()-1 != selection.size())
	{
		cout << "size mismatch" << endl;
	}
	vector<element *>::iterator elem;
	elem=selection.begin();
	int count=1;
	while(elem!=selection.end())
	{
		(*elem)->name=argv[count];
		count++;
		elem++;
	}
}

void cmd_route()
{
	findBlobs();
	findLines();
}

//commands
void cmd_pad(vector<string> argv)
{
	if(argv.size()<6)
		cout << "pad <name> <x> <y> <w> <h>" << endl;
	else
		rootelement->add( new element_pad( argv[1],stoi(argv[2]),stoi(argv[3]),stoi(argv[4]),stoi(argv[5]) ) );	
}

void cmd_select(vector<string> argv)
{
	if(argv.size()<2)
		cout << "select <elements...>" << endl;
	else		
	{
		selection.clear();		//do we want this?
		for_each(argv.begin()+1, argv.end(), selectwithname);
	}
}



/*void cmd_list(vector<string> argv)
{
	if(argv.size()<2)
		cout << "list <element_group>" << endl;
	else
	{

	}
}*/
