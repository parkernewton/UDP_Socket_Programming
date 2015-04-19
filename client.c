// client.c 
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

int main(int argc, char *argv[])
// Main driver function 
// Establishes a UDP socket on client-side, locates the remote server host, 
// 		and sends a text file over the network. 
// Expects the following program arguments: 
// 		1) name of server host 
// 		2) name of file to transfer 

{
	// s stores socket, f_size is size of file to transfer
	// sockaddr_in struct stores info about the remote endpoint to which we bind a socket
	// stores information about the remote server 
	// file pointer to file to transfer 
	// f_name stores name of file to transfer, buffer stores contents of file to transfer 
	int s, f_size = 0; 		 
	struct sockaddr_in addr; 		
	struct hostent *srvr; 		
	FILE *fp; 		
	char *f_name, *buffer; 		

	if(argc < 3){
		printf("\nError: not enough args to program.\n"); 
		return -1; 
	}

	// Establish a UDP socket 
	if((s = socket(AF_INET, SOCK_DGRAM, 0)) == -1){
		printf("\nError: cannot create a socket.\n"); 
		return -1; 
	}

	// Locate the server host 
	if((srvr = gethostbyname(argv[1])) == NULL){
		printf("\nError: could not verify host.\n");  
		close(s);
		return -1; 
	} 

	// Write 0's to the members of the sockaddr_in struct
	bzero((char *)&addr, sizeof(struct sockaddr_in)); 
 
	// AF_INET denotes address family for our socket as IP 
	addr.sin_family = AF_INET; 

	// Specify the IP address of the server 
	bcopy((char *)srvr->h_addr, (char *)&addr.sin_addr, srvr->h_length); 

	// Specify the server port 
	addr.sin_port = htons(SERVER_PORT); 

	f_name = argv[2]; 
	if((fp = fopen(f_name, "r")) == NULL){
		printf("\nError: unable to open file for reading.\n");   
		close(s);
		return -1; 
	}

	// Calculate the size of the file in bytes
	fseek(fp, 0, SEEK_END); 
	f_size = ftell(fp); 
	rewind(fp); 
	f_size /= sizeof(char); 

	// Allocate appropriate memory for buffer to which we will read contents of file 
	if((buffer = (char *)malloc(sizeof(char) * f_size)) == NULL){
		printf("\nError: unable to allocate memory for file buffer.\n"); 
		fclose(fp);  
		close(s);
		return -1; 
	} 
	if(!fread(buffer, sizeof(char), f_size, fp)){
		printf("\nError: 0 char's read from file.\n"); 
		free(buffer); 
		fclose(fp);  
		close(s);
		return -1; 
	}

	// Send first the file name, and then the contents of the file 
	// We add 1 to arg 3 of sendto() to account for the terminating null byte 
	if(sendto(s, f_name, strlen(f_name) + 1, 0, (struct sockaddr *)&addr, sizeof(addr)) != (strlen(f_name) + 1)){
		printf("\nError: sendto of file name failed.\n"); 
		free(buffer); 
		fclose(fp);  
		close(s);
		return -1; 
	}
	if(sendto(s, buffer, strlen(buffer) + 1, 0, (struct sockaddr *)&addr, sizeof(addr)) != strlen(buffer) + 1){
		printf("\nError: sendto of file body failed.\n"); 
		free(buffer); 
		fclose(fp);  
		close(s);
		return -1;
	}

	free(buffer); 
	fclose(fp);  
	close(s); 	
	return 0; 
}
