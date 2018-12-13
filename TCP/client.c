#include "tcp.h"
#include "fileHandler.h"


ssize_t tcp_send(int sockfd, const void *buf, size_t len, int flags){
	return sr_send(sockfd, buf, len,flags);
	//return sw_send(sockfd, buf, len,flags);
	//return gbn_send(sockfd, buf, len,flags);

}
ssize_t tcp_recv(int sockfd, void *buf, size_t len, int flags){
	//return sr_recv(sockfd, buf, len,flags);
	return sw_recv(sockfd, buf, len,flags);
	//return gbn_recv(sockfd, buf, len,flags);	
}

struct client_input {
    char server_ip[15], file_name[30];
    int server_port, client_port, window_size;
};

struct client_input* read_client_file(char *file_name){
    struct FileInfo* finfo = file_read(file_name);
    struct client_input * ci = malloc(sizeof(struct client_input));
    // parse file content
    char* pointer = finfo->content;
    char buffer[50];
    // set serve ip
    int i=0;
    while(!isspace((char)*pointer)){
        buffer[i++] = *pointer;
        pointer++;
    }
    buffer[i] = 0;
    strcpy(ci->server_ip, buffer);
    // printf("%s\n", ci->server_ip);
    // set serve port
    i=0;
    pointer++;
    while(!isspace((char)*pointer)){
        buffer[i++] = *pointer;
        pointer++;
    }
    buffer[i] = 0;
    ci->server_port = atoi(buffer);
    // printf("%d\n", ci->server_port);

    // set client port
    i=0;
    pointer++;
    while(!isspace((char)*pointer)){
        buffer[i++] = *pointer;
        pointer++;
    }
    buffer[i] = 0;
    ci->client_port = atoi(buffer);
    // printf("%d\n", ci->client_port);

    // set file name
    i=0;
    pointer++;
    while(!isspace((char)*pointer)){
        buffer[i++] = *pointer;
        pointer++;
    }
    buffer[i] = 0;
    strcpy(ci->file_name, buffer);
    // printf("%s\n", ci->file_name);

    // set window_size
    i=0;
    pointer++;
    while(!isspace((char)*pointer) && *pointer != '\0'){
        buffer[i++] = *pointer;
        pointer++;
    }
    buffer[i] = 0;
    ci->window_size = atoi(buffer);
    // printf("%d\n", ci->window_size);

    return ci;
}


int main(int argc, char *argv[]){
	int sockfd;          /* socket file descriptor of the client            */
	int numRead;
	socklen_t socklen;	 /* length of the socket structure sockaddr         */
	// char buf[DATALEN * N];    buffer to send packets                       
	struct hostent *he;	 /* structure for resolving names into IP addresses */
	struct sockaddr_in server;
	FILE *outputFile;

	socklen = sizeof(struct sockaddr);

	/*----- Checking arguments -----*/
	// if (argc != 4){
	// 	fprintf(stderr, "usage: sender <hostname> <port> <filename>\n");
	// 	exit(-1);
	// }
	struct client_input* input = read_client_file("client.in");
	/*----- Opening the input file -----*/
	// if ((inputFile = fopen(input->file_name, "rb")) == NULL){
	// 	perror("fopen");
	// 	exit(-1);
	// }

	/*----- Resolving hostname to the respective IP address -----*/
	if ((he = gethostbyname(input->server_ip)) == NULL){
		perror("gethostbyname");
		exit(-1);
	}
	
	/*----- Opening the socket -----*/
	if ((sockfd = tcp_socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1){
		perror("tcp_socket");
		exit(-1);
	}
	s.window_size=input->window_size;
	printf("size............%d\n",s.window_size );
	
	char buf[DATALEN*MAX_WINDOW_SIZE];	

	/*--- Setting the server's parameters -----*/
	memset(&server, 0, sizeof(struct sockaddr_in));
	server.sin_family = AF_INET;
	server.sin_addr   = *(struct in_addr *)he->h_addr;
	printf("%s\n",(char *) he->h_addr);
	
	server.sin_port   = htons(input->server_port);
	
	/*----- Connecting to the server -----*/
	if (connect(sockfd, (struct sockaddr *)&server, socklen) == -1){
		perror("gbn_connect");
		exit(-1);
	}
	printf("%s\n",input->file_name );
	char fname[200];
	sprintf(fname,"send/%s",input->file_name);
	if (tcp_send(sockfd, fname, strlen(fname), 0) == -1){
			perror("gbn_send");
			exit(-1);
	}
	
	sprintf(fname,"received/%s",input->file_name);

	if ((outputFile = fopen(fname, "wb")) == NULL){
		perror("fopen");
		exit(-1);
	}

	while(1){
		printf("recive   %d..............................................................\n",numRead );
		if ((numRead = tcp_recv(sockfd, buf, DATALEN*MAX_WINDOW_SIZE, 0)) == -1){
			perror("gbn_recv");
			exit(-1);
		}
		else if (numRead == 0)
			break;
		printf("recive   %d..............................................................\n",numRead );
		//printf("%s\n",buf );
		fwrite(buf, 1, numRead, outputFile);
	}

	// /*----- Reading from the file and sending it through the socket -----*/
	// while ((numRead = fread(buf, 1, DATALEN * N, inputFile)) > 0){
	// 	if (tcp_send(sockfd, buf, numRead, 0) == -1){
	// 		perror("gbn_send");
	// 		exit(-1);
	// 	}
	// }

	/*----- Closing the socket -----*/
	if (tcp_close(sockfd) == -1){
		perror("tcp_close");
		exit(-1);
	}
	
	/*----- Closing the file -----*/
	if (fclose(outputFile) == EOF){
		perror("fclose");
		exit(-1);
	}
	
	return(0);
}

