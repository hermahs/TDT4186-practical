#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <threads.h>
#include "util.h"

#define BUFFER_SIZE (1024*1024)
#define ADDR "127.0.0.1"

#define error(...) do {              \
    fprintf (stderr, __VA_ARGS__);   \
    exit(EXIT_FAILURE);              \
} while(false)

#define CRLF "\r\n"

thread_local char recv_buffer[BUFFER_SIZE], send_buffer[BUFFER_SIZE];
settings_s settings;


int main(int argc, char *argv[]) {

    int server_socket, client_socket, read_size;

    if (argc != 5)
        error("not the correct amount of args\n");
    
	settings.origin_path = argv[1];
    
	if(!check_if_path_exist(settings.origin_path))
        error("path does not exist\n");

	settings.port = strtol(argv[2], NULL, 10);
	settings.num_threads = strtol(argv[3], NULL, 10);
	settings.buffer_slots = strtol(argv[4], NULL, 10);

    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) < -1)
        error("could not create socket\n");

    // Allow other processes to bind to the socket, to allow quick restarts
    int optval = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));

    struct sockaddr_in server_addr, client_addr;

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ADDR);
    server_addr.sin_port = htons(settings.port);
    
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
        error("bind error\n");

    if (listen(server_socket, 5) < 0)
        error("listen error");

    while (1) {
        int client_addr_len = sizeof(struct sockaddr_in);
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, (socklen_t*)&client_addr_len);

        if (client_socket < 0)
            error("nani?! accept failed!!\n");

        while ((read_size = recv(client_socket, recv_buffer, 6000, 0)) > 0) {
        	
			int send = create_send_data_to_client();
			
            if (send >= sizeof(send_buffer))
                fprintf(stderr, "Send buffer not large enough for response! Truncated!\n");

            if (write(client_socket, send_buffer, send) < 0)
                fprintf(stderr, "send error");

            close(client_socket);
        }
    }

    close(server_socket);

    return EXIT_SUCCESS;
}
