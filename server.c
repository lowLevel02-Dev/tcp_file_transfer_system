#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <endian.h>
#include <stdint.h>
#include <sys/epoll.h>

typedef struct{
	uint32_t filename_len;
	uint64_t file_size;
}file_header_t;

int recv_header(int sock_fd, file_header_t *header){
	uint32_t net_filename_len;
	ssize_t bytes_read = read(sock_fd, &net_filename_len, sizeof(net_filename_len));
	if(bytes_read != sizeof(net_filename_len)){
		return -1;
	}
	header->filename_len = ntohl(net_filename_len);

	uint64_t net_file_size;
	bytes_read = read(sock_fd, &net_file_size,sizeof(net_file_size));
	if(bytes_read != sizeof(net_file_size)){
		return -1;
	}
	header->file_size = be64toh(net_file_size);
	return 0;
}

int main(){
	int server_fd = socket(AF_INET, SOCK_STREAM, 0);
	int opt = 1;
	if(server_fd < 0){
		perror("socket failed");
		exit(1);
	}
	setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	struct sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(8080);

	if(bind(server_fd , (struct sockaddr *)&address , sizeof(address)) < 0){
		perror("bind failed");
		exit(1);
	}

	if(listen(server_fd,5) < 0){
		perror("listen failed");
		exit(1);
	}

	printf("Server listening on port 8080...\n"); 

	int epoll_fd = epoll_create1(0);
	if(epoll_fd < 0){
		perror("epoll_create1 failed");
		exit(1);
	}

	struct epoll_event ev; 
	ev.events = EPOLLIN;
	ev.data.fd = server_fd;

	if(epoll_ct1(epoll_fd , EPOLL_CTL_ADD, server_fd, &ev) < 0){
		perror("epoll_ct1 failed");
		exit(1);
	}

	file_header_t header;
	if(recv_header(client_fd, & header) < 0){
		printf("failed to recieve the header\n");
		close(client_fd);
		close(server_fd);
		return 1;
	}
	printf("Recieved header: filename_len=%u, file_size=%lu\n", header.filename_len, header.file_size);

	char filename[256] = {0};
	ssize_t bytes_read = read(client_fd, filename, header.filename_len);
	if(bytes_read != (ssize_t)header.filename_len){
		printf("Failed to recieve the filename \n");
		close(client_fd);
		close(server_fd);
		return -1;
	}
	printf("Receiving file: %s\n",filename);

	char *file_data = calloc(header.file_size + 1 , 1); 
	bytes_read = read(client_fd,file_data,header.file_size);
	if(bytes_read != (ssize_t)header.file_size){
		free(file_data);
		printf("failed.\n");
		close(client_fd);
		close(server_fd);
		return -1;
	}
	printf("file content: %s",file_data);
	close(client_fd);
	close(server_fd);
	return 0;
}
