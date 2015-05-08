//Names: Evan Thompson, John Cusick, Tausif Ahmed

#include <cstdlib>
#include <iostream>
#include "pagingSys.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define PORT_NUM 8765
#define NUM_FRAMES 32
#define SIZE_FRAMES 1024
#define FRAMES_PER_FILE 4

//global variable
PagingSystem* paging;

//Thread functions go in this section
//-------------- THREAD ----------------





//--------------------------------------



//sets up the server and gets everything ready to start up, also sets up the paging system
void init_server(){
	paging = new PagingSystem(NUM_FRAMES, SIZE_FRAMES, FRAMES_PER_FILE);

	paging->print_stats();

	paging->load_file(std::string("helloWorld.txt"));
}
//start listening for connections and then starts up threads to handle the commands issued
void start_listening(){
	//STORE, DELETE, READ, DIR	


}

int main(){

	init_server();
  
	start_listening();  

	free(paging);
  	std::cout<<"Hello World!"<<std::endl;
  	return EXIT_SUCCESS;
}
