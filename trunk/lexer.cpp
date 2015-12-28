#include "lexer.h"
#include "cmd.h"
#include "drawing.h"

#include <algorithm>
#include <string>
#include <iostream>
#include <vector>
using namespace std;

vector < vector < string > > lx_stack;

//only one lexer

void lx_tokenize(string& str, vector<string>& tokens, const string& delimiters = " ")
{
	tokens.clear();
    // Skip delimiters at beginning.
    string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    string::size_type pos     = str.find_first_of(delimiters, lastPos);

    while (string::npos != pos || string::npos != lastPos)
    {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
}

void lx_parse(vector<string> words)		//should handle all non ambiguous
{
	//k this is crap
	if(words[0]=="pad") cmd_pad(words);
	if(words[0]=="select") cmd_select(words);
	if(words[0]=="delete") cmd_delete();
	if(words[0]=="help") cmd_help();
	if(words[0]=="info") cmd_info();
	if(words[0]=="rename") cmd_rename(words);
	if(words[0]=="route") cmd_route();
}


void lx_service()
{
	int a;
	if((a=lx_stack.size())>0)		//theres a bug hereish
	{
		for_each(lx_stack.begin(), lx_stack.end(), lx_parse);
		//cout << "I had fun at Disneyworld" << endl; 
		de_redraw();
		lx_stack.clear();
	}
}

//this is the threaded part of the lexer

static DWORD lexer_thread;

void lx_boot()
{
	lx_stack.clear();
	CreateThread(NULL, 0, lx_run, 0, 0, &lexer_thread);
}

DWORD WINAPI lx_run(void *crap)		//i am a unique
{
	string line;
	vector<string> words;
	while(getline(cin, line)) 
	{
		lx_tokenize(line, words);
		//cout << "parsed " << words.size() << " words" << endl;
		if(words.size() > 0)
		{
			lx_stack.push_back(words);
		}
	}
}
