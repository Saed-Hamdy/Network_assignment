#include <sys/wait.h>
#include "tcp.h"
#include "fileHandler.h"


void client_handler(struct sockaddr_in* serverAddr, struct sockaddr_in* clientAddr, int newSockfd) ;
ssize_t tcp_send(int sockfd, const void *buf, size_t len, int flags){
	//return sr_send(sockfd, buf, len,flags);
	return sw_send(sockfd, buf, len,flags);
	//return gbn_send(sockfd, buf, len,flags);

}
ssize_t tcp_recv(int sockfd, void *buf, size_t len, int flags){
	//return sr_recv(sockfd, buf, len,flags);
	return sw_recv(sockfd, buf, len,flags);
	//return gbn_recv(sockfd, buf, len,flags);	
}

struct server_input {
    int port, max_window_size;
    float seed, loss_prob;
};

struct server_input* read_server_file(char *file_name){
    struct FileInfo* finfo = file_read(file_name);
    struct server_input* ser_in = malloc(sizeof(struct server_input));
    // parse file content
    char* pointer = finfo->content;
    char buffer[50];
    // set serve port
    int i=0;
    while(!isspace((char)*pointer)){
        buffer[i++] = *pointer;
        pointer++;
    }
    buffer[i] = 0;
    ser_in->port = atoi(buffer);
    // printf("%d\n", ser_in->port);

    // set max window size
    i=0;
    pointer++;
    while(!isspace((char)*pointer)){
        buffer[i++] = *pointer;
        pointer++;
    }
    buffer[i] = 0;
    ser_in->max_window_size = atoi(buffer);
    // printf("%d\n", ser_in->max_window_size);

    // set random seed
    i=0;
    pointer++;
    while(!isspace((char)*pointer)){
        buffer[i++] = *pointer;
        pointer++;
    }
    buffer[i] = 0;
    ser_in->seed = atof(buffer);
    // printf("%f\n", ser_in->seed);

    // set max window size
    i=0;
    pointer++;
    while(!isspace((char)*pointer) && *pointer != '\0'){
        buffer[i++] = *pointer;
        pointer++;
    }
    buffer[i] = 0;
    ser_in->loss_prob = atof(buffer);
    // printf("%f\n", ser_in->loss_prob);

    return ser_in;
}


int main(int argc, char *argv[])
{
	int sockfd;
	int newSockfd;
	int numRead;
	struct sockaddr_in server;
	struct sockaddr_in client;
	FILE *inputFile;     /* input file pointer                              */
	socklen_t socklen;
	char buf[DATALEN * N];   /* buffer to send packets                       */
	
	/*----- Checking arguments -----*/
	// if (argc != 3){
	// 	fprintf(stderr, "usage: receiver <port> <filename>\n");
	// 	exit(-1);
	// }
	struct server_input* input = read_server_file("server.in");
	srand(input->seed);
	/*----- Opening the output file -----*/
	// char * file_name ="output.txt";
	// // file_name=argv[2];
	// if ((outputFile = fopen(file_name, "wb")) == NULL){
	// 	perror("fopen");
	// 	exit(-1);
	// }

	/*----- Opening the socket -----*/
	if ((sockfd = tcp_socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1){
		perror("tcp_socket");
		exit(-1);
	}
	
	/*--- Setting the server's parameters -----*/
	memset(&server, 0, sizeof(struct sockaddr_in));
	server.sin_family      = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	printf("%s\n",INADDR_ANY );
	// server.sin_port        = htons(atoi(argv[1]));
	printf("%d\n", (input->port));
	server.sin_port        = htons((input->port));

	s.max_window_size=input-> max_window_size;
	printf("s.max_window_size =%d\n", s.max_window_size);
	s.loss_prob = input->loss_prob ;
	
	/*----- Binding to the designated port -----*/
	if (tcp_bind(sockfd, (struct sockaddr *)&server, sizeof(struct sockaddr_in)) == -1){
        perror("tcp_bind");
		exit(-1);
	}
	
	/*----- Listening to new connections (not impemented ToDo) -----*/
	if (tcp_listen(sockfd, 1) == -1){
		perror("tcp_listen");
		exit(-1);
	}

	/*----- Waiting for the client to connect -----*/
	socklen = sizeof(struct sockaddr_in);
		//printf("recive   %d..............................................................\n",s.type );
	// newSockfd = tcp_accept(sockfd, (struct sockaddr *)&client, &socklen);
	// if (newSockfd == -1){
	// 	perror("tcp_accept");
	// 	exit(-1);
	// }

	// printf("%d ->>>> %d \n",sockfd,newSockfd );
	
	// ----- Reading from the socket and dumping it to the file -----
	// if ((numRead = tcp_recv(newSockfd, buf, DATALEN*MAX_WINDOW_SIZE, 0)) == -1){
	// 		perror("gbn_recv");
	// 		exit(-1);
	// }else{
	// 	printf("File Name = %s..................................................\n", buf);
	// 	// exit(0);
	// }

	// if ((inputFile = fopen(buf, "rb")) == NULL){
	// 	perror("fopen");
	// 	exit(-1);
	// }
	// printf("open.............................\n");

	// //----- Reading from the file and sending it through the socket -----
	// while ((numRead = fread(buf, 1, DATALEN * N, inputFile)) > 0){
	// 	printf("read..........................\n");
	// 	if (tcp_send(sockfd, buf, numRead, 0) == -1){
	// 		perror("gbn_send");
	// 		exit(-1);
	// 	}
	// }

	// //----- Closing the socket -----
	// if (tcp_close(sockfd) == -1){
	// 	perror("tcp_close");
	// 	exit(-1);
	// }

	// //----- Closing the file -----
	// if (fclose(inputFile) == EOF){
	// 	perror("fclose");
	// 	exit(-1);
	// }


	 while(true){
            printf("Waiting for request..\n");
            newSockfd = tcp_accept(sockfd, (struct sockaddr *)&client, &socklen);
			if (newSockfd == -1){
				perror("tcp_accept");
				exit(-1);
			}
                        // create new child
            pid_t pid = fork();
            if(pid == 0){
                // child process
                client_handler(&server, &client,newSockfd);
                // kill child process upon completion
                kill(getpid(), SIGKILL);

            }else if (pid > 0){
            	printf("Parent: child created..\n");
            	wait(&pid);
            	if (WIFSIGNALED(pid) != 0)
            		printf("CHILED DIED BY SIGNAL");
            	else if(WIFEXITED(pid) != 0)
            		printf("chiled ended successfully\n");
            	else
            		printf("child process ended Not NORMAL \n");
            }else{
                printf("Fork failed !\n");
            }
        } // end of while
			
	return (0);
}

void client_handler(struct sockaddr_in* serverAddr, struct sockaddr_in* clientAddr,int newSockfd){

	// int newSockfd;
	int numRead;
	struct sockaddr_in server;
	struct sockaddr_in client;
	FILE *inputFile;     /* input file pointer                              */
	socklen_t socklen;
	char buf[DATALEN * N];
	printf("in chiles .........................\n");

	 // if ((newSockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
  //       printf("Child socket creation failed!\n");
  //   else {
  //   	printf("nw soket ..............%d\n",newSockfd );
  //       if(bind(newSockfd, (struct sockaddr *) serverAddr, sizeof(struct sockaddr_in)) < 0)
  //           perror("Error, child binding failed");

		if ((numRead = tcp_recv(newSockfd, buf, DATALEN*MAX_WINDOW_SIZE, 0)) == -1){
				perror("gbn_recv");
				exit(-1);
		}else{
			printf("File Name = %s..................................................\n", buf);
			// exit(0);
		}

		if ((inputFile = fopen(buf, "rb")) == NULL){
			perror("fopen");
			exit(-1);
		}
		printf("open.............................\n");

		//----- Reading from the file and sending it through the socket -----
		while ((numRead = fread(buf, 1, DATALEN * N, inputFile)) > 0){
			printf("read..........................\n");
			if (tcp_send(newSockfd, buf, numRead, 0) == -1){
				perror("gbn_send");
				exit(-1);
			}
		}

		//----- Closing the socket -----
		if (tcp_close(newSockfd) == -1){
			perror("tcp_close");
			exit(-1);
		}

		//----- Closing the file -----
		if (fclose(inputFile) == EOF){
			perror("fclose");
			exit(-1);
		}
	// }

}
