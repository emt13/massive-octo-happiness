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
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

#define PORT_NUM 8765
#define HOST "74.74.74.74"


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
	
	/*
	for (int i = 0; i < commands.size(); ++i) {
		std::cout << commands[i] << std::endl;
	}*/

	int sd, ret;
	struct sockaddr_in server;
	struct in_addr ipv4addr;
	struct hostent *hp;

	sd = socket(AF_INET, SOCK_STREAM, 0);
	server.sin_family = AF_INET;

	inet_pton(AF_INET, HOST, &ipv4addr);
       	hp = gethostbyaddr(&ipv4addr, sizeof(ipv4addr), AF_INET);

	bcopy(hp -> h_addr, &(server.sin_addr.s_addr), hp -> h_length);
	server.sin_port = htons(PORT_NUM);

	connect(sd, (const sockaddr *)&server, sizeof(server));
	for(int i = 0; i < commands.size(); ++i) {
		send(sd, (char *)commands[i].c_str(), strlen((char *)commands[i].c_str()), 0);
		/*if (i == commands.size() - 1) {
			i = 0;
		}*/
	}



	return EXIT_SUCCESS;
}
