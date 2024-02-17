/* 
 * This loader is like a first version of the web page content loader, using very old APIs for TCP and Websockets.
 * The whole file is in charge for almost all the process
 */

// Standar headers
#include "stdio.h"
#include "string.h"
#include "stdlib.h"

// Net headers
#include <netdb.h>
#include "sys/socket.h"
#include "netinet/in.h"
#include <arpa/inet.h>


#define BUF_SIZE 4095
#define BUF_SIZE_INC BUF_SIZE+1

#ifndef NULL
#define NULL '\0'
#endif


int main(int argc, char** argv)
{
	// ERROR => The hostname is not valid
	if(argc < 2)
	{
		printf("No hostname has been provided.\n");
		return 1;
	}

	// REGION => Resolve the DNS
	
	struct hostent* hst = gethostbyname(argv[1]);

	// ERROR => The hostname cannot be resolved
	if(!hst)
	{
		printf("Resolving hostname has failed.\n");
		return 1;
	}


	// Get the IP bia inet_ntoa which deals with the conversion
	char* ip = inet_ntoa( *( (struct in_addr*) hst->h_addr_list[0]));

	int sockfd; // Socked file descriptor
	int port_n = 80;

	char header_buffer[512];
	struct sockaddr_in server_addr;

	// AF_INET stands for Addres Family Internet
	// SOCK_STREAM is TCP
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	// ERROR => Could not open the socket at port
	if(sockfd == -1)
	{
		printf("Could not open the socket at port: %d\n",port_n);
		return 1;
	}
	else
	{
		printf("Socket openned at port:%d\n",port_n);
	}

	// Connect
	memset((char*) &server_addr, 0, sizeof(server_addr)); //Set the space
							      
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port_n); // Right format for the port number

	inet_pton(AF_INET, ip, &(server_addr.sin_addr)); // Fill the address
	
	// ERROR => Failed to connect to the IP
	if(connect(sockfd, (struct sockaddr*) &server_addr, sizeof(server_addr))< 0)
	{
		printf("Failed to connect\n");
		return 1;
	}

	// Print the Header information
	snprintf(header_buffer, 512, "GET / HTTP/1.1\r\nHost: %s\r\n\r\n", argv[1]);
	printf("Header %s\n", header_buffer);

	int conn_length = send(sockfd, header_buffer, strlen(header_buffer), 0);

	if(conn_length)
	{
		printf("nr %d (%lu)\n", conn_length, strlen(header_buffer));
	}

	char* buffer;
	buffer = (char*) malloc(BUF_SIZE_INC * sizeof(char));
	memset(buffer, 0, BUF_SIZE_INC);

	ssize_t bytes_received = recv(sockfd, buffer, BUF_SIZE, 0);

	// Constantly check if there's something in the buffer
	while(bytes_received && bytes_received != 0)
	{
		buffer[bytes_received] = 0;
		printf("%s\n", buffer);
		memset(buffer, 0, BUF_SIZE_INC);
		bytes_received = recv(sockfd, buffer, BUF_SIZE, 0);
	}
}
