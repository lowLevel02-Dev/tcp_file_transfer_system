#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <endian.h>
#include <stdint.h>

typedef struct{
	uint32_t filename_len;
	uint64_t file_size;
}file_header_t;

int send_header(int sock_fd, file_header_t *header){
	uint32_t net_filename_len = htonl(header->filename_len);
	uint64_t net_file_size = htobe64(header->file_size);

	ssize_t bytes_write = write(sock_fd, &net_filename_len, sizeof(net_filename_len));
	if(bytes_write != sizeof(net_filename_len)){
		return -1;
	}

	bytes_write = write(sock_fd, &net_file_size, sizeof(net_file_size));
	if(bytes_write != sizeof(net_file_size)){
		return -1;
	}
	return 0;
}

int main(){
	int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(sock_fd < 0){
		perror("socket failed");
		exit(1);
	}
	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(8080);

	const char *ip_string = "192.168.7.1";

	if(inet_pton(AF_INET, ip_string , &server_addr.sin_addr) <= 0){
		printf("invalid address, connection failed");
		close(sock_fd);
		return -1;
	}
	if(connect(sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
		perror("connection failed");
		close(sock_fd);
		return -1;
	}

	printf("Successful connection established\n");

	file_header_t header;
	const char *filename = "test.txt";
	const char *file_data = "Hello from the file transfer system";
	header.filename_len = strlen(filename);
	header.file_size = strlen(file_data);

	if(send_header(sock_fd, &header) < 0){
		printf("Failed to send the header\n");
		close(sock_fd);
		return -1;
	}
	ssize_t bytes_written = write(sock_fd,filename,header.filename_len);
	if(bytes_written != (ssize_t)header.filename_len){
		printf("Failed to send the file\n");
		close(sock_fd);
		return -1;
	}
	bytes_written = write(sock_fd,file_data,header.file_size);
	if(bytes_written != (ssize_t)header.file_size){
		printf("failed to send the file");
		close(sock_fd);
		return -1;
	}
	close(sock_fd);
	return 0;
}
