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
#include <fcntl.h>
#include <sys/uio.h>

#define BUFFER_SIZE 1025

typedef unsigned char BYTE;

int main()
{
  /* create TCP client socket (endpoint) */
  int sock = socket( PF_INET, SOCK_STREAM, 0 );

  if ( sock < 0 )
    {
      perror( "socket() failed" );
      exit( EXIT_FAILURE );
    }

  struct hostent * hp = gethostbyname( "localhost" );
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
  
 // while(1){
    //char * msg = "READ sample.txt 1000 5000\n";
    char * msg = "STORE hw.out 15044\n";
    BYTE arr[15044];
    int rc = open("hw.out", O_RDONLY);
    int val = read(rc, arr, 15044);
    printf("val: %d\n", val);
    int n = write( sock, msg, strlen( msg ) );
    int v = write( sock, arr, 15044);
    fflush( NULL );
    if ( n < strlen( msg ) )
      {
	perror( "write() failed" );
	exit( EXIT_FAILURE );
      }
    
    /*char buffer[ BUFFER_SIZE ];
    n = read( sock, buffer, BUFFER_SIZE );  // BLOCK
    if ( n < 0 )
      {
	perror( "read() failed" );
	exit( EXIT_FAILURE );
      }
    else
      {
	buffer[n] = '\0';
	printf( "Received message from server: %s\n", buffer );
      }*/
 // }
  close( sock );

  return EXIT_SUCCESS;
}
