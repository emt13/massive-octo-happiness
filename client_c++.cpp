/* client.cpp  */

//Names: Evan Thompson, John Cusick, Tausif Ahmed

/*NOTES:
 *	Test client implemented in C++
 *
 * */

#include <netinet/in.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <vector>
#include <fstream>

#define PORT_NUM 8765

void loadFile(std::vector<std::string> *commands) {
	std::string file_name = "", line = "";
	std::cout << "Please provide a local test file: ";
	std::cin >> file_name;
	std::ifstream file;
	file.open(file_name);
	while (std::getline(file, line)) {
		commands -> push_back(line);
	}
	file.close();
}


int main() {

	/*
	 *	- Connect to server via PORT_NUM
	 *	- Read through vector and pass each command to server
	 *
	 * */
	std::vector<std::string> commands;
	loadFile(&commands);
	
	for (int i = 0; i < commands.size(); ++i) {
		std::cout << commands[i] << std::endl;
	}

	return EXIT_SUCCESS;
}
