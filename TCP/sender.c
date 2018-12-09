#include "tcp.h"


ssize_t tcp_send(int sockfd, const void *buf, size_t len, int flags){
	return sr_send(sockfd, buf, len,flags);
}
ssize_t tcp_recv(int sockfd, void *buf, size_t len, int flags){
	return sr_recv(sockfd, buf, len,flags);;	
}


int main(int argc, char *argv[]){
	int sockfd;          /* socket file descriptor of the client            */
	int numRead;
	socklen_t socklen;	 /* length of the socket structure sockaddr         */
	char buf[DATALEN * N];   /* buffer to send packets                       */
	struct hostent *he;	 /* structure for resolving names into IP addresses */
	FILE *inputFile;     /* input file pointer                              */
	struct sockaddr_in server;

	socklen = sizeof(struct sockaddr);

	/*----- Checking arguments -----*/
	if (argc != 4){
		fprintf(stderr, "usage: sender <hostname> <port> <filename>\n");
		exit(-1);
	}
	
	/*----- Opening the input file -----*/
	if ((inputFile = fopen(argv[3], "rb")) == NULL){
		perror("fopen");
		exit(-1);
	}

	/*----- Resolving hostname to the respective IP address -----*/
	if ((he = gethostbyname(argv[1])) == NULL){
		perror("gethostbyname");
		exit(-1);
	}
	
	/*----- Opening the socket -----*/
	if ((sockfd = tcp_socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1){
		perror("tcp_socket");
		exit(-1);
	}

	/*--- Setting the server's parameters -----*/
	memset(&server, 0, sizeof(struct sockaddr_in));
	server.sin_family = AF_INET;
	server.sin_addr   = *(struct in_addr *)he->h_addr;
	printf("%s\n",(char *) he->h_addr);
	server.sin_port   = htons(atoi(argv[2]));
	
	/*----- Connecting to the server -----*/
	if (connect(sockfd, (struct sockaddr *)&server, socklen) == -1){
		perror("gbn_connect");
		exit(-1);
	}

	/*----- Reading from the file and sending it through the socket -----*/
	while ((numRead = fread(buf, 1, DATALEN * N, inputFile)) > 0){
		if (tcp_send(sockfd, buf, numRead, 0) == -1){
			perror("gbn_send");
			exit(-1);
		}
	}

	/*----- Closing the socket -----*/
	if (tcp_close(sockfd) == -1){
		perror("tcp_close");
		exit(-1);
	}
	
	/*----- Closing the file -----*/
	if (fclose(inputFile) == EOF){
		perror("fclose");
		exit(-1);
	}
	
	return(0);
}

