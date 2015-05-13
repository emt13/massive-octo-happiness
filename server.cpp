/* server.c */
//Names: Evan Thompson, John Cusick, Tausif Ahmed   

/*NOTES:
  -12 byte buffer: handles 11 with the enter, 10 with enter and EOS
  -filenames have no spaces
  -weird thing: a lot of output when client exited out (not properly closed)
    --exit out of terminal rather than control C
 */


/*TODO
-send function
-string to char* function + \n
-int to char* function + \n  
-STORE
-READ
-DELETE
*/




#include <sys/types.h>
#include <sys/socket.h>
#include <sstream>

#include <netinet/in.h>
#include <iostream>
#include <unistd.h>
#include <string.h>
#include <cstdlib>
#include <arpa/inet.h>
#include <pthread.h>
#include "pagingSys.h"
#include "fileSys.h"


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
pthread_mutex_t addMutex = PTHREAD_MUTEX_INITIALIZER;  //for new threads

/* global mutex variable */
pthread_mutex_t storeMutex = PTHREAD_MUTEX_INITIALIZER;  //for new clients


//holds all Thread IDs
pthread_t * tids; 

//the paging?
PagingSystem* paging;

int sock;

//file system
FileSystem* filesys;

//================================================================
//========================FUNCTIONS===============================
//================================================================



void sendClient(int socket, const char * str, int len){
  int numSent = send( socket, str, len, 0 );
  fflush( NULL );
  if ( numSent != len ){
    perror( "send() failed" );
  }
}




int numDig(unsigned int x){
  int length = 1;
  while ( x /= 10 ){
    length++;
  }
  return length;
}






//Thread functions go in this section                                                               
//-------------- THREAD ----------------                                                            


/*
  Parse server queries into argv/argc data type
  Return Value: 0 = success, 1 = unknown command
*/

//could add test to make sure don't read in MAX_ARG arguments, but unneccesary
int parseQuery(char *comm, struct queryType * query){
  //to be deleted in client_thread
  query->argc = 0;
  
  printf("COMM: %s\n",comm);

  char * tmp = strtok(comm, " ");
  int counter = 0;
  
  printf("tmp: %s\n",tmp);
  
  if(tmp == NULL){
    printf("TEMP IS NULL\n");
    return 1; //should not be the case but for safety
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
  else if(strcmp(tmp,"DIR\n") == 0){
    query->type = 4;
  }
  else{
    //must be a packet with file-info!!!!!!!!
    query->type = 5;
    //first fix the buffer
    if(comm[strlen(tmp)] != '\0'){ //idk to be safe?
      comm[strlen(tmp)] = ' ';
    }
    return 1;
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
  
  return 0; //success
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

  printf("DONE WITH SERVER INIT\n");


  //server init                                                                                     


}



//start listening for connections and then starts up threads to handle the commands issued          
void start_listening(){
  //STORE, DELETE, READ, DIR                                                                        

}









//big problem with allocation and deallocation here


//WARNING: ANY COMMAND WORDS IN FILE CONTENTS ARE STILL FILE CONTENTS

void * client_thread(void * arg){
  struct clientArg * clientInfo = (struct clientArg *) arg;
  int n;
  char * buffer = new char[BUFFER_SIZE+1];
  unsigned long tid = (unsigned long) pthread_self();
  bool command = 1;  //default to a command
  std::vector<std::string> file_names;

  //int numRecv;
  int numBytes = 0;
  char * bigBuff;
  char *tmpBig;
  int currBytes = 0;
  char *fName;

  struct queryType * query = new struct queryType; 
  query->argv = new char *[MAX_ARG];
  
  
  do{
    printf( "[thread %lu]: Blocked on recv()\n", tid );
    
    /* can also use read() and write()..... */
    
    if(command == 1){
      n = recv( *(clientInfo->newsock), buffer, BUFFER_SIZE, 0 );
    }
    else{
      n = recv( *(clientInfo->newsock), tmpBig, numBytes, 0 );
    }
    
    if ( n < 0 ){
      perror( "recv() failed" );
    }
    else if ( n == 0 ){
      printf( "[thread %lu]: Rcvd 0 from recv(); closing socket\n", tid );
    }
    else{

      int rc = 1;
      if(command == 1){  //parse the command
	buffer[n] = '\0';  /* assuming text.... */
	printf( "[thread %lu]: Rcvd message from %s: %s\n",tid,inet_ntoa( (struct in_addr)clientInfo->client->sin_addr) ,buffer );
	
	rc = parseQuery(buffer,query);
      }

      if(rc == 1 && command == 1){
	//no command and expecting command
	printf( "[thread %lu]: ERROR: no such command\n", tid );
      }
      if(rc == 1 && command == 0){  //2nd part of store
	//was expecting file contents
	printf( "[thread %lu]: file contents: %s\n", tid,tmpBig);
	if(currBytes == 0){
	  strncpy(bigBuff, tmpBig, numBytes);
	}
	else{
	  strncat(bigBuff, tmpBig,numBytes-currBytes);
	}
	currBytes+=n;  //n set by recv, numBytes received
	
	
	if(currBytes == numBytes){  //ready to store, go ahead and lock
	  pthread_mutex_lock( &storeMutex );    /*   P(mutex)  */
	  //CRITICAL SECTION: storing
	  std::vector<BYTE> storeInput;
	  
	  for(int i = 0; i < numBytes; i++){
	    storeInput.push_back((BYTE)bigBuff[i]);
	  }
	  
	  /*
	    rc: -1 if it already exists
	    total # written if it doesn't
	   */
	  filesys->addFile(fName);
	  int rc = paging->store(fName, storeInput);       
	  //END CRITICAL SECTION                                                                          
	  pthread_mutex_unlock( &storeMutex );  /*   V(mutex)  */
	  
	  free(bigBuff);
	  free(tmpBig);
	  free(fName);
	  
	  command = 1;
	  
	  if(rc == -1){  //file already existed,  SORRY CHAP
	    printf("FILE ALREADY EXISTED\n");
	    int sendN = send( *(clientInfo->newsock), "ERROR: FILE EXISTS\n", 19, 0 );
	    fflush( NULL );
	    if ( sendN != 19){
	      perror( "send() failed" );
	    }
	  }
	  else{
	    sendClient(*(clientInfo->newsock),"ACK\n", 4);
	  }
	}
      }
      if(rc == 0){ //got a command
	//debug print
	printf("argc: %d\n",query->argc);
	printf("type: %d\n",query->type);
	for(int i = 0; i < query->argc; i++){
	  printf("argv[%d]: %s\n", i, query->argv[i]);
	}



	if(query->type == 1){  //store
	  
	  numBytes = atoi(query->argv[2]);
	  command = 0;
	  bigBuff = (char *) calloc(numBytes, sizeof(char));
	  tmpBig = (char *) calloc(numBytes, sizeof(char));
	  
	  fName = (char *) calloc(strlen(query->argv[1]), sizeof(char));
	  strcpy(fName, query->argv[1]);

	}


	else if(query->type == 2){  //read
	  pthread_mutex_lock( &RDMutex );    /*   P(mutex)  */
	  //CRITICAL SECTION: read & delete
	  
	  int offset = atoi(query->argv[2]);
	  int numRead = atoi(query->argv[3]);
	  
	  int *flag = new int; 
	  /*
	    0:success
	    1:file DNE
	    2:byte range invalid
	    ...
	   */

	  int currRead = 0;
	  int stopVal = 0;

	  int startVal = 0;
	  
	  
	  std::vector<BYTE> output;


	  //if file queued for deletion, spew ERROR: FILE QUEUED FOR DELETION\n to client

	  
	  bool send = 1;

	  //use a counter here if you need to do +1 or something (maybe no counter maybe just if(currRead==0) dont add 1, else +1 -------- for startVal
	  while(currRead != numRead){
	    std::vector<BYTE> tmpVec;
	    
	    startVal = offset+currRead;
	    if((startVal%SIZE_FRAMES)+(numRead - currRead) > SIZE_FRAMES){  //go to end of frame
	      stopVal = SIZE_FRAMES;  //end of frame
	    }
	    else{  //this is our last frame
	      stopVal = numRead-currRead;
	    }
	    
	    printf("START VAL: %d\n",startVal);
	    printf("STOP VAL: %d\n",stopVal);

	    tmpVec = paging->read_page(query->argv[1], startVal, stopVal, flag);
	    
	    if(*flag == 1){
	      sendClient(*(clientInfo->newsock),"ERROR: NO SUCH FILE\n",20);
	      send = 0;
	      break;
	    }
	    else if(*flag == 2){
	      sendClient(*(clientInfo->newsock),"ERROR: INVALID BYTE RANGE\n",26);
	      send = 0;
	      break;
	    }
	    
	    for(int i = 0; i < tmpVec.size(); i++){
	      output.push_back(tmpVec[i]);
	    } 
	    
	    currRead+=tmpVec.size();

	    std::cout<<"====================================================================="<<std::endl;
	    std::cout<<"====================================================================="<<std::endl;
	    std::cout<<"====================================================================="<<std::endl;
	    std::cout<<"====================================================================="<<std::endl;
	    std::cout<<"====================================================================="<<std::endl;
	    std::cout<<"====================================================================="<<std::endl;
	    std::cout<<"====================================================================="<<std::endl;
	    std::cout<<"====================================================================="<<std::endl;
	    std::cout<<"====================================================================="<<std::endl;
	    std::cout<<"====================================================================="<<std::endl;
	    std::cout<<"====================================================================="<<std::endl;
	    std::cout<<"====================================================================="<<std::endl;
	    std::cout<<"====================================================================="<<std::endl;
	    std::cout<<"====================================================================="<<std::endl;

	   
	    printf( "[thread %lu]: Sent: ACK %lu\n", tid , tmpVec.size());
	    
	    //TODO: send bytes to client..........

	    printf( "[thread %lu]: Transferred %lu Bytes from offset %d\n", tid , tmpVec.size(), startVal);
	    
	  }

	  if(send == 1){
	    printf("SIZE OF OUTPUT: %lu\n", output.size());
	    printf("numREAD: %d\n", numRead);
	    
	    char * clientOut = (char * )calloc(output.size(), sizeof(char));
	    for(int i = 0; i  < output.size(); i++){
	      clientOut[i] = (char) output[i];
	      std::cout<<"output["<<i<<"] = "<<output[i]<<std::endl;
	    }
	    
	    
	    
	    //convert number to char*
	    std::stringstream strs;
	    strs << output.size();
	    std::string temp_str = strs.str();
	    const char* numOut = (char*) temp_str.c_str();
	    
	    //int numDigits = numDig(file_names.size());  //find number of digits

	    
	    char firstOut[strlen(numOut) + 5];  //5: ack\n space
	    
	    strcpy(firstOut,"ACK ");
	    strcat(firstOut, numOut);
	    strcat(firstOut,"\n");

	    //ACK
	    sendClient( *(clientInfo->newsock), firstOut, strlen(firstOut));
	    
	    
	    //file contents
	    sendClient( *(clientInfo->newsock), clientOut, strlen(clientOut));
	    


	  }
	  

	  delete flag;
	  //END CRITICAL SECTION                                                                          
	  pthread_mutex_unlock( &RDMutex );  /*   V(mutex)  */

	}


	else if(query->type == 3){  //delete command, some synchronization required
	  printf("start delete\n");
	  
	  filesys->print();
	  
	  printf("end debug print\n");
	  query->argv[1][strlen(query->argv[1])-1] = '\0';  //get rid of \n
	  printf("delete2\n");
	  printf("argv[1]: %s\n",query->argv[1]);

	  int rc = filesys->removeFile(query->argv[1]);
	  printf("delete3\n");
	  if(rc == 1){
	    printf("FILE NOT FOUND: %s\n",query->argv[1]);
	    
	  }
	  else{
	    //how well does it work with bum filename?
	    paging->delete_file(std::string(query->argv[1]));
	  }
	}


	else if(query->type == 4){  //dir command, no synchronization required
	  file_names = paging->dir();
	  //no need for 0 files check, this line is generalized
	  //Note: no files just returns 0\n

	  if(file_names.size()==0){
	    n = send( *(clientInfo->newsock), "0\n", 2, 0 );
	    fflush( NULL );
	    if ( n != 2){
	      perror( "send() failed" );
	    }

	  }
	  else{
	    //convert number to char*
	    std::stringstream strs;
	    strs << file_names.size();
	    std::string temp_str = strs.str();
	    const char* numFiles = (char*) temp_str.c_str();


	    
	    
	    int numDigits = numDig(file_names.size());  //find number of digits
	    
	    std::cout<<"numFiles: "<<numFiles<<std::endl;
	    std::cout<<"numDigits: "<<numDigits<<std::endl;
	    
	    char outTmp[numDigits+1];
	    strcpy(outTmp, numFiles);
	    outTmp[numDigits] = '\n';
	    
	    n = send( *(clientInfo->newsock), outTmp, numDigits + 1, 0 );
	    fflush( NULL );
	    if ( n != numDigits + 1){
	      perror( "send() failed" );
	    }
	    
	    for(unsigned int i = 0; i < file_names.size(); i++){
	      std::cout<<file_names[i]<<std::endl;
	      std::string str = file_names[i];
	      const char* charStr = (char*) str.c_str();
	      char out[strlen(charStr)+1];
	      strcpy(out, charStr);
	      out[strlen(charStr)]='\n';

	      n = send( *(clientInfo->newsock), out, strlen(charStr) + 1, 0 );
	      fflush( NULL );
	      if ( (unsigned int)n != strlen(charStr) + 1){
		perror( "send() failed" );
	      }


	    }
	  }
	  std::cout<<"Done with DIR"<<std::endl;
	  /*

	        //ACK

      
	  n = send( *(clientInfo->newsock), "ACK\n", 4, 0 );
	  fflush( NULL );
	  if ( n != 4 ){
	    perror( "send() failed" );
	  }
	  
*/

	}


	
      }
      //FIX: IF MORE THAN ONE COMMAND IN A QUERY, SPLIT ON \n AND LOOP THROUGH

	
      /* send ack message back to the client */

      
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



/*
void * exitThread(void *){
  while(1){
    std::string exitStr;
    std::cin>>exitStr;
    if(exitStr.compare("quit")==0 || exitStr.compare("exit")==0){
      break;
    }
  }
  close( sock );
  delete paging;
  delete [] tids;
  return EXIT_SUCCESS;
}
*/




int main()
{
  std::cout<<"Hello World!"<<std::endl;
  
  init_server();

  //keep track of pids
  
  //thread ids and bools for if pthread_join has been called on them
  tids = new pthread_t[1000]; 
  
  printf("crash is in FileSystem\n");
  
  filesys = new FileSystem;  //must be after init_server (init adds files to server)

  printf("testa\n");

  /* Create the listener socket as TCP (SOCK_STREAM) socket */
  sock = socket( PF_INET, SOCK_STREAM, 0 );

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
  printf( "Listening on port %d\n", port );

  
  //int pid;
  int rc;
  //char buffer[ BUFFER_SIZE ];



  printf("testb\n");




  while ( 1 ){
    struct sockaddr_in * client = new struct sockaddr_in;
    int fromlen = sizeof( client );
    
    
    
    printf( "PARENT: Blocked on accept()\n" );
    int *newsock = new int; 
    *newsock = accept( sock, (struct sockaddr *)client,
		       (socklen_t*)&fromlen );
    printf( "Received incoming connection from %d.%d.%d.%d\n",
	    int(client->sin_addr.s_addr&0xFF),
	    int((client->sin_addr.s_addr&0xFF00)>>8),
	    int((client->sin_addr.s_addr&0xFF0000)>>16),
	    int((client->sin_addr.s_addr&0xFF000000)>>24));
    
    //set up argument
    struct clientArg * clientInfo = new clientArg;
    clientInfo->client = client;
    clientInfo->newsock = newsock;
    
    /* handle socket in child process */
    
    //CRITICAL SECTION: global tCounter.. dont want to overright any tids  
    pthread_mutex_lock( &addMutex );    /*   P(mutex)  */
    
    rc = pthread_create( &tids[tCounter], NULL, client_thread, (void*)clientInfo);
    if ( rc != 0 ){
      fprintf( stderr, "pthread_create() failed (%d)\n", rc );
    }
    tCounter++;
    
    //END CRITICAL SECTION
    pthread_mutex_unlock( &addMutex );  /*   V(mutex)  */
    
  }
  
  close( sock );

  delete filesys;

  delete paging;
  
  delete [] tids;
  
  return EXIT_SUCCESS;
}
