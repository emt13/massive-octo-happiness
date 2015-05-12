/* server.c */

//Names: Evan Thompson, John Cusick, Tausif Ahmed   

/*NOTES:
  -12 byte buffer: handles 11 with the enter, 10 with enter and EOS
  -filenames have no spaces
  -weird thing: a lot of output when client exited out (not properly closed)
    --exit out of terminal rather than control C
 */


#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <iostream>
#include <unistd.h>
#include <string.h>
#include <cstdlib>
#include <arpa/inet.h>
#include <pthread.h>
#include "pagingSys.h"

//FIX: buffer is const size, any file-contents larger than 1024 will get cutoff
#define BUFFER_SIZE 1025

#define PORT_NUM 8765
#define NUM_FRAMES 32
#define SIZE_FRAMES 1024
#define FRAMES_PER_FILE 4

typedef unsigned char BYTE;

//global variable                                                                                   


#define MAX_ARG 100

struct clientArg{
  struct sockaddr_in * client;
  int *newsock; 
};

struct queryType{
  char ** argv;
  int argc;
  int type;
  /*
    types:
    1: STORE
    2: READ
    3: DELETE
    4: DIR
    5: STORE's file-contents
   */
};






//==============global variables=================

//global thread counter (for tid[])
//may encounter some critical sections here -- be wary
int tCounter = 0;

/* global mutex variable */
pthread_mutex_t addMutex = PTHREAD_MUTEX_INITIALIZER;

//holds all Thread IDs
pthread_t * tids; 

//the paging?
PagingSystem* paging;





//================================================================
//========================FUNCTIONS===============================
//================================================================



//Thread functions go in this section                                                               
//-------------- THREAD ----------------                                                            


/*
  Parse server queries into argv/argc data type
  Return Value: 1 = success, 0 = unknown command
*/

//could add test to make sure don't read in MAX_ARG arguments, but unneccesary
int parseQuery(char *comm, struct queryType * query){
  //to be deleted in client_thread
  query->argc = 0;
  char * tmp = strtok(comm, " ");
  int counter = 0;
  int returnVal = 0;

  if(tmp == NULL){
    printf("TEMP IS NULL\n");
    return 0; //should not be the case but for safety
  }

  //the \n was also a part of it (which is fine, because should be <comm> w/ a space
  if(strcmp(tmp,"STORE") == 0){
    query->type = 1;
  }
  else if(strcmp(tmp,"READ") == 0){
    query->type = 2;
  }
  else if(strcmp(tmp,"DELETE") == 0){
    query->type = 3;
  }
  else if(strcmp(tmp,"DIR") == 0){
    query->type = 4;
  }
  else{
    //must be a packet with file-info!!!!!!!!
    query->type = 5;
    //first fix the buffer
    comm[strlen(tmp)] = ' ';
    return 0;
  }

  while(tmp!=NULL){
    query->argv[counter] = tmp;
    query->argc++;
    tmp = strtok(NULL, " ");
    counter++;
    /*
    if(counter == 3){
      break;
    }
    */
  }
  
  return 1; //success
}


//Note: doesn't recognize STORE w/out arguments (due to \n)
//------------------------------------------------




//sets up the server and gets everything ready to start up, also sets up the paging system          
void init_server(){

  //paging system init                                                                              
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


  //server init                                                                                     



}



//start listening for connections and then starts up threads to handle the commands issued          
void start_listening(){
  //STORE, DELETE, READ, DIR                                                                        


}









//big problem with allocation and deallocation here


void * client_thread(void * arg){
  struct clientArg * clientInfo = (struct clientArg *) arg;
  int n;
  char * buffer = new char[BUFFER_SIZE+1];
  unsigned long tid = (unsigned long) pthread_self();
  bool command = 1;  //default to a command


  struct queryType * query = new struct queryType; 
  query->argv = new char *[MAX_ARG];
  
  
  do{
    printf( "[thread %lu]: Blocked on recv()\n", tid );
    
    /* can also use read() and write()..... */
    n = recv( *(clientInfo->newsock), buffer, BUFFER_SIZE, 0 );
    
    if ( n < 0 ){
      perror( "recv() failed" );
    }
    else if ( n == 0 ){
      printf( "[thread %lu]: Rcvd 0 from recv(); closing socket\n", tid );
    }
    else{
      buffer[n] = '\0';  /* assuming text.... */
      printf( "[thread %lu]: Rcvd message from %s: %s\n",tid,inet_ntoa( (struct in_addr)clientInfo->client->sin_addr) ,buffer );
      
      int rc = parseQuery(buffer,query);
      if(rc == 0 && command == 1){
	//no command and expecting command
	printf( "[thread %lu]: ERROR: no such command\n", tid );
      }
      if(rc == 0 && command == 0){
	//was expecting file contents
	printf( "[thread %lu]: file contents: %s\n", tid,buffer );
      }
      if(rc == 1){
	printf("argc: %d\n",query->argc);
	printf("type: %d\n",query->type);
	for(int i = 0; i < query->argc; i++){
	  printf("argv[%d]: %s\n", i, query->argv[i]);
	}
	
      }
      //FIX: IF MORE THAN ONE COMMAND IN A QUERY, SPLIT ON \n AND LOOP THROUGH

	
      /* send ack message back to the client */

      //MAKE THIS ACK\n
      n = send( *(clientInfo->newsock), "ACK", 3, 0 );
      fflush( NULL );
      if ( n != 3 ){
	perror( "send() failed" );
      }
    }
  }while ( n > 0 );
  /* this do..while loop exits when the recv() call
     returns 0, indicating the remote/client side has
     closed its socket */
  
  printf( "[thread %lu]: Bye!\n", tid );
  close( *(clientInfo->newsock) );
  
  delete [] buffer;
  
  delete [] query->argv;
  delete query;

  delete clientInfo->newsock;
  delete clientInfo->client;
  delete clientInfo;
  
  pthread_exit(NULL); /* thread terminates here! */
}







int main()
{
  std::cout<<"Hello World!"<<std::endl;
  
  init_server();

  //keep track of pids
  
  //thread ids and bools for if pthread_join has been called on them
  tids = new pthread_t[1000]; 



  /* Create the listener socket as TCP (SOCK_STREAM) socket */
  int sock = socket( PF_INET, SOCK_STREAM, 0 );

  if ( sock < 0 ){
    perror( "socket() failed" );
    exit( EXIT_FAILURE );
  }
  
  /* socket structures */
  struct sockaddr_in server;

  server.sin_family = PF_INET;
  server.sin_addr.s_addr = INADDR_ANY;

  unsigned short port = 8765;

  /* htons() is host-to-network-short for marshalling */
  /* Internet is "big endian"; Intel is "little endian" */
  server.sin_port = htons( port );
  int len = sizeof( server );

  if ( bind( sock, (struct sockaddr *)&server, len ) < 0 ){
    perror( "bind() failed" );
    exit( EXIT_FAILURE );
  }
  
  listen( sock, 5 );   /* 5 is the max number of waiting clients */
  printf( "PARENT: Listener bound to port %d\n", port );

  
  int pid;
  int rc;
  char buffer[ BUFFER_SIZE ];

  while ( 1 ){
    struct sockaddr_in * client = new struct sockaddr_in;
    int fromlen = sizeof( client );
    
    
    
    printf( "PARENT: Blocked on accept()\n" );
    int *newsock = new int; 
    *newsock = accept( sock, (struct sockaddr *)client,
		       (socklen_t*)&fromlen );
    printf( "PARENT: Accepted client connection\n" );
    
    //set up argument
    struct clientArg * clientInfo = new clientArg;
    clientInfo->client = client;
    clientInfo->newsock = newsock;
    
    /* handle socket in child process */
    
    pthread_mutex_lock( &addMutex );    /*   P(mutex)  */
    //CRITICAL SECTION: global tCounter.. dont want to overright any tids  
    
    int rc = pthread_create( &tids[tCounter], NULL, client_thread, (void*)clientInfo);
    if ( rc != 0 ){
      fprintf( stderr, "pthread_create() failed (%d)\n", rc );
    }
    tCounter++;
    
    //END CRITICAL SECTION
    
    pthread_mutex_unlock( &addMutex );  /*   V(mutex)  */
    
  }
  
  close( sock );



  free(paging);
  
  delete [] tids;
  
  return EXIT_SUCCESS;
}
