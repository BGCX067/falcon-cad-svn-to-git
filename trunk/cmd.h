#ifndef __CMD_H
#define __CMD_H

#include <iostream>
#include <vector>
#include <sstream>
using namespace std;

void cmd_help();

void cmd_delete();
void cmd_info();

void cmd_rename(vector<string> argv);

void cmd_route();

void cmd_pad(vector<string> argv);
void cmd_select(vector<string> argv);

#endif
