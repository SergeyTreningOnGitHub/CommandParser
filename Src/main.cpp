/*!
    \file main.cpp
*/

#include "command_parser.h"
#include <cassert>
#include <iostream>

using namespace std;



int main(int argc, char* argv[]) {	
	assert(argc == 2);
	uint32_t N = (uint32_t)(stoi(string(argv[1])));
	assert(N > 0);
	CommandParser cmd_parser(N, cin, cout);
	cmd_parser.EnableLog();
	while (!cmd_parser.IsEndStream()) {
		cmd_parser.HandleCommand();
	}
	
	return 0;
}