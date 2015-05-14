/* client.c */

#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <strings.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <glob.h>


#define BUFFER_SIZE 1024

char fileName[] = "hello.out";


int main()
{
  /* create TCP client socket (endpoint) */
  int sock = socket( PF_INET, SOCK_STREAM, 0 );

  if ( sock < 0 )
    {
      perror( "socket() failed" );
      exit( EXIT_FAILURE );
    }

  struct hostent * hp = gethostbyname( "127.0.0.1" );
  if ( hp == NULL )
    {
      perror( "gethostbyname() failed" );
      exit( EXIT_FAILURE );
    }

  struct sockaddr_in server;
  server.sin_family = PF_INET;
  memcpy( (void *)&server.sin_addr, (void *)hp->h_addr,
          hp->h_length );
  unsigned short port = 8765;
  server.sin_port = htons( port );

  printf( "server address is %s\n", inet_ntoa( server.sin_addr ) );

  if ( connect( sock, (struct sockaddr *)&server,
                sizeof( server ) ) < 0 )
    {
      perror( "connect() failed" );
      exit( EXIT_FAILURE );
    }



  //char * msg = "hello world";


    
  char buffer[1024];

  
  getcwd(buffer, 1024);
  
  char inPath[1024+1];
  inPath[1024] = '\0';
  strcpy(inPath, buffer);
  strcat(inPath, "/");
  strcat(inPath, fileName);
  

  int inFD = open(inPath, O_RDONLY , 0644);
  if(inFD == -1){
    fprintf(stderr, "in input open() failed, inPath: %s\n",inPath);
    perror("input open() failed");
    exit(1);
  }


  struct stat sb;

  if (stat(fileName, &sb) == -1) {
    perror("stat");
  }
  else {
    printf("File size:   %lld bytes\n",
           (long long) sb.st_size);
  }
  
  int numBytes = (int) sb.st_size;

  char intBuff[1000];

  snprintf(intBuff,1000,"%d",numBytes);
  

  char command[1024];
  strcpy(command, "STORE ");
  strcat(command, fileName);
  strcat(command, " ");
  strcat(command, intBuff);
  strcat(command, "\n");
  
  printf("command: %s\n",command);




  int n = write( sock, command, sizeof(command));
  fflush( NULL );
  if ( n < sizeof(command)){
    perror( "write() failed" );
    exit( EXIT_FAILURE );
  }
  

  char recvbuffer[ BUFFER_SIZE ];
  
  /*
  printf("b4 read\n");
  //doesnt ack
  
  n = read( sock, recvbuffer, BUFFER_SIZE );  // BLOCK
  if ( n < 0 )
    {
      perror( "read() failed" );
      exit( EXIT_FAILURE );
    }
  else
    {
      recvbuffer[n] = '\0';
      printf( "Received message from server: %s\n", recvbuffer );
    }


  printf("past read\n");
  */
  int copiedBytes = 0;

  int numRead = read(inFD, buffer, 1024);
  //printf("numRead = %d\n",numRead);                                               

                                                                     
  while (numBytes > copiedBytes){
    int n = write( sock, buffer, numRead );
    fflush( NULL );
    if ( n < numRead ){
      perror( "write() failed" );
      exit( EXIT_FAILURE );
    }
    
    printf("%s\n",buffer);

    copiedBytes+=numRead;
    numRead = read(inFD, buffer, 1024);
    if(numRead == 0){
      break;
    }
    

  }
  
  printf("numBytes: %d\n",numBytes);
  printf("numsend: %d\n",copiedBytes);


  n = read( sock, recvbuffer, BUFFER_SIZE );  // BLOCK
  if ( n < 0 )
    {
      perror( "read() failed" );
      exit( EXIT_FAILURE );
    }
  else
    {
      recvbuffer[n] = '\0';
      printf( "Received message from server: %s\n", recvbuffer );
    }






  close( sock );

  return EXIT_SUCCESS;
}
