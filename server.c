// server.c 
// Parker Newton 
// 4-16-15 
// COEN 146L - T 2:15 pm 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>

#define SERVER_PORT 10014
#define BUFFER_SIZE 1000

int main(int argc, char *argv[])
// Main driver function 
// Establishes a UDP socket on server-side, binds the socket to the local host's IP address, 
//		and receives the contents of a text file to be written to a new text file. 
{
	// s stores the socket 
	// buffer stores the received text to write to the output file 
	// file_name stores the received text to name the output file 
	// addr is an instance of a sockaddr_in struct, which stores information about the remote endpoint to which we bind the socket 
	// fp points to the output file to which we write the received text 
	// addr_size stores the size of the sockaddr_in struct, which we pass to the recvfrom() fucntion 
	int s; 
	char buffer[BUFFER_SIZE]; 
	char file_name[BUFFER_SIZE]; 
	struct sockaddr_in addr;
	FILE *fp; 
	socklen_t addr_size = sizeof(addr); 

	// Establish the UDP socket 
	if((s = socket(AF_INET, SOCK_DGRAM, 0)) == -1){
		printf("\nError: cannot create a socket.\n"); 
		return -1; 
	} 

	// Write zeros to the members of the sockaddr_in struct 
	bzero((char *)&addr, sizeof(struct sockaddr_in));

	// AF_INET denotes address family for our socket as IP 
	addr.sin_family = AF_INET;

	// We accept any IP address to bind the socket to 
	addr.sin_addr.s_addr = INADDR_ANY; 

	// Specify the server port 
	addr.sin_port = htons(SERVER_PORT);

	// Bind the socket to the local IP address of the server
	if(bind(s, (struct sockaddr *)&addr, sizeof(addr)) == -1){
		printf("\nError: server bind failed. %s\n", strerror(errno));   
		close(s); 
		return -1; 
	} 

	// Receive first, the file name, and second, the contents of the text file to write 
	recvfrom(s, file_name, BUFFER_SIZE, 0, (struct sockaddr *)&addr, &addr_size); 
	recvfrom(s, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&addr, &addr_size); 

	// Create a new file named file_name in the local directory, and write the received text to it
	if((fp = fopen(file_name, "w")) == NULL){
		printf("\nError opening file.\n");  
		close(s); 
		return -1; 
	}
	if(fwrite(buffer, sizeof(char), strlen(buffer), fp) != strlen(buffer)){
		printf("\nError writing to text file.\n"); 
		fclose(fp); 
		close(s); 
		return -1; 
	}

	fclose(fp); 
	close(s); 
	return 0; 
}
