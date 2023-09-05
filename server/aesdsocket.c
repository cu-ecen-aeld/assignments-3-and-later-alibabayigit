#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <syslog.h>
#include <fcntl.h>

#define RW_SIZE 1024
#define PORT "9000"
#define FILE_TO_RW "/var/tmp/aesdsocketdata"

int cfd;
int sfd;
int fd; 
void sig_handler(int signal_number){

	if(signal_number == SIGINT || signal_number == SIGTERM){
	
		syslog(LOG_DEBUG, "Caught signal, exiting");
		close(sfd);
		close(cfd);
		remove(FILE_TO_RW);
		exit(1);
	}
}

int main(int argc, char* argv[]){

	struct addrinfo hints;
	struct addrinfo *servinfo, *iterator;
	memset(&hints, 0, sizeof hints); 
	hints.ai_family = AF_UNSPEC;     
	hints.ai_socktype = SOCK_STREAM; 
	hints.ai_flags = AI_PASSIVE;     


	int ret;
	int yes=1;

	//struct sockaddr_storage client_addr;
	struct sockaddr_in client_addr;
	socklen_t addr_size;


	char s[INET6_ADDRSTRLEN];
	
	int num_bytes_read = 0;
	int num_bytes_sent = 0;
	
	signal(SIGINT, sig_handler);
	signal(SIGTERM, sig_handler);

	if(getaddrinfo(NULL, PORT, &hints, &servinfo) != 0) {
	    return -1;
	}
	
	for(iterator = servinfo; iterator != NULL; iterator = iterator->ai_next){
	
		sfd = socket(PF_INET, SOCK_STREAM, 0);
		if(sfd == -1){ /*error on socket() */
		     //TODO: log error
		     continue;
		}
	
		if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) == -1) {
		    return -1;
		} 
		
		ret = bind(sfd, servinfo->ai_addr, servinfo->ai_addrlen);
		if(ret == -1){ /* error on bind() */
		    // TODO: log error
		    close(sfd);
		    return -1;
		}
		
		break;
	}

	freeaddrinfo(servinfo);
	
	if(iterator == NULL){ // failed to bind
		return -1;
	}
	
	ret = listen(sfd, 20);
	if(ret == -1){ // error on listen()
	    // TODO: log error
	    return -1;
	}
	
	if(argc == 2 && strcmp(argv[1], "-d") == 0){
	
		daemon(0, 0);
	}

	while(1){
	
		addr_size = sizeof client_addr;
		cfd = accept(sfd, (struct sockaddr *)&client_addr, &addr_size);
		if(cfd == -1){ // error on accept()
		    // TODO: log error
		    return -1;
		    // continue;
		}

		inet_ntop(AF_INET, &(client_addr.sin_addr), s, sizeof s);
		syslog(LOG_DEBUG, "Accepted connection from %s", s);


		fd = open(FILE_TO_RW, O_RDWR | O_APPEND | O_CREAT, 0644);
		
		char read_buf[RW_SIZE];
		while((num_bytes_read = recv(cfd, read_buf, RW_SIZE, 0)) > 0){
			
			write(fd, read_buf, num_bytes_read);
			
			if(strchr(read_buf, '\n') != NULL){
				break;
			}
		
		}

		lseek(fd, 0, SEEK_SET);
		
		while((num_bytes_sent = read(fd, read_buf, RW_SIZE)) > 0){
		
			send(cfd, read_buf, num_bytes_sent, 0);
			
		}
		
		close(fd);
		close(cfd);
		
		syslog(LOG_DEBUG, "Closed connection from %s", s);
	}
	return 0;
}
