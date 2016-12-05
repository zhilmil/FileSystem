#include <stdio.h>
#include <stdlib.h>

#include <netdb.h>
#include <netinet/in.h>

#include "libnetfiles.h"
struct hostent *server;

int netserverinit(char* hostname, int* filemode)
{	
   server = gethostbyname(hostname);
   
   if (server == NULL) {
      fprintf(stderr,"ERROR, no such host\n");
      return -1;
   }
   else
	return 0;
}

int netopen(const char* pathname, int flags)
{
	//creating socket
	int sockfd, portno,n;
	struct sockaddr_in serv_addr;
	portno = 1800;
	
	//creating socket point
	sockfd = socket(AF_INET, SOCK_STREAM,0);
	if (sockfd<0)
	{
		perror("Error opening socket");
		exit(1);
	}
	
	if(server == NULL)
	{
		perror("Error opening");
	}
	//getting addreses
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(portno);
	
	//connecting to server
	if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr))<0)
	{
		perror("connection error");
		exit(1);
	}
	
	// creating to be sent message
	char* message;
	char* mode = "exclusive,";
	message = malloc(strlen(pathname)+15);
	strcpy(message,mode);
	strcat(message,pathname);
	
	n = write(sockfd, message, strlen(message));
	
	if(n<0){
		perror("Error writing to socket");
		exit(1);
	}

	//reading server response
	char buffer[256];
	bzero(buffer,256);
	n = read(sockfd, buffer, 255);
	
	if(n<0)
	{
		perror("Error reading from socket");
		exit(1);
	}

	printf("%s\n",buffer);
	return 0;	
}



