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

typedef unsigned char BYTE;

//global variable
PagingSystem* paging;

//Thread functions go in this section
//-------------- THREAD ----------------





//--------------------------------------



//sets up the server and gets everything ready to start up, also sets up the paging system
void init_server(){
	paging = new PagingSystem(NUM_FRAMES, SIZE_FRAMES, FRAMES_PER_FILE);

	paging->print_stats();

	std::vector<BYTE> file_input;
	file_input.push_back((unsigned char) *("A"));
	file_input.push_back((unsigned char) *("B"));
	file_input.push_back((unsigned char) *("C"));
	file_input.push_back((unsigned char) *("\n"));
	file_input.push_back((unsigned char) *("N"));
	file_input.push_back((unsigned char) *("-"));
	file_input.push_back((unsigned char) *("\n"));
	file_input.push_back((unsigned char) *("j"));

	int rc = paging->store(std::string("test-store.txt"), file_input);
	printf("store returned: %d\n", rc);
	std::cout<<"stored file"<<std::endl;

	std::vector<std::string> all_files = paging->dir();
	for(unsigned int i = 0; i < all_files.size(); i++){
		std::cout<<" - "<< all_files[i]<<std::endl;
	}	

	paging->delete_file("test-delete.txt");

	all_files = paging->dir();
	for(unsigned int i = 0; i < all_files.size(); i++){
		std::cout<<" - "<< all_files[i]<<std::endl;
	}

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
