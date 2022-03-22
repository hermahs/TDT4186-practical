#include "main.h"
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <threads.h>
#include <errno.h>
#include <error.h>
#include "http.h"
#include "files.h"

#define ADDR "127.0.0.1"

settings_s settings;

int main(int argc, char *argv[]) {

    int server_socket, client_socket, read_size;

    if (argc != 5)
        error(-1, 0, "Usage: %s <www-path> <port> <#threads> <#bufferslots>", argv[0]);
    
	settings.origin_path = argv[1];
	if(!check_if_path_readable(settings.origin_path))
        error(-1, 0, "www-path '%s' does not exist", argv[1]);

	settings.port = strtol(argv[2], NULL, 10);
	settings.num_threads = strtol(argv[3], NULL, 10);
	settings.buffer_slots = strtol(argv[4], NULL, 10);

    if ((server_socket = socket(AF_INET6, SOCK_STREAM, 0)) < 0)
        error(-1, errno, "could not create socket");

    // Allow other processes to bind to the socket, to allow quick restarts
    int optval = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));

    struct sockaddr_in6 server_addr = {}, client_addr = {};
    server_addr.sin6_family = AF_INET6;
    server_addr.sin6_addr = in6addr_any; //= IN6ADDR_ANY_INITinet_addr(ADDR);
    server_addr.sin6_port = htons(settings.port);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
        error(-1, errno, "bind error");

    if (listen(server_socket, 5) < 0)
        error(-1, errno, "listen error");

    while (1) {
        int client_addr_len = sizeof(struct sockaddr_in);
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, (socklen_t*)&client_addr_len);

        if (client_socket < 0)
            error(0, errno, "accept failed");

        if (recv(client_socket, recv_buffer, sizeof(recv_buffer), 0) < 0) {
            error(0, errno, "recv error");
        } else {
            int send = handle_get_request();
            if (write(client_socket, send_buffer, send) < 0)
                error(0, errno, "send error");
        }
        close(client_socket);
    }
    close(server_socket);

    return EXIT_SUCCESS;
}
