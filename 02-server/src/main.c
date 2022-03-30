#include "main.h"
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <error.h>
#include "http.h"
#include "files.h"
#include "bbuffer.h"

#define ADDR "127.0.0.1"

_Thread_local char recv_buffer[BUFFER_SIZE], send_buffer[BUFFER_SIZE];
_Thread_local int thread_number;
settings_s settings;
BNDBUF* queue;

void* handle_thread(void* arg) {
    thread_number = (intptr_t) arg;

    while(1) {
        int client_socket = bb_get(queue); // Gives us the file descriptor of a new client

        // Set timeout on recv operations, to avoid blocking forever
        struct timeval tv;
        tv.tv_sec = 1; // 1 second
        tv.tv_usec = 0;
        setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);

        int recieved = 0;
        bool newline_seen = false;
        while(!newline_seen && recieved < sizeof(recv_buffer)) {
            int more = recv(client_socket, recv_buffer+recieved, sizeof(recv_buffer)-recieved, 0);
            if (more < 0) {
                error(0, errno, "recv error");
                goto fail;
            }

            for(int i = recieved; i < recieved+more && !newline_seen; i++)
                newline_seen = newline_seen || recv_buffer[i] == '\n';
        }

        int send = handle_get_request();
        if (write(client_socket, send_buffer, send) < 0)
            error(0, errno, "send error");

    fail:
        close(client_socket);
    }
    return NULL;
}

int main(int argc, char *argv[]) {

    int server_socket, client_socket;

    // =============== Arg parsing ===============
    if (argc != 5)
        error(-1, 0, "Usage: %s <www-path> <port> <#threads> <#bufferslots>", argv[0]);

    settings.origin_path = argv[1];
    if(!check_if_path_readable(settings.origin_path))
        error(-1, 0, "www-path '%s' does not exist", argv[1]);

    settings.port = strtol(argv[2], NULL, 10);
    settings.num_threads = strtol(argv[3], NULL, 10);
    settings.buffer_slots = strtol(argv[4], NULL, 10);

    // =============== Server socket creation and binding ===============
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

    // =============== Create thread pool to consume client queue  ===============
    printf("Creating ring buffer with %d slots\n", settings.buffer_slots);
    queue = bb_init(settings.buffer_slots);
    pthread_t* threads = malloc(sizeof(pthread_t) * settings.num_threads);
    if(threads == NULL)
        error(-1, errno, "not enough memory for threads");
    printf("Creating %d threads\n", settings.num_threads);
    for(int i = 0; i < settings.num_threads; i++) {
        int err = pthread_create(&threads[i], NULL, handle_thread, (void*)(intptr_t)i);
        if (err != 0)
            error(-1, err, "thread creation failed");
    }

    // =============== Server socket accept loop ===============
    while (1) {
        int client_addr_len = sizeof(struct sockaddr_in);
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, (socklen_t*)&client_addr_len);

        if (client_socket < 0)
            error(0, errno, "accept failed");
        else {
            bb_add(queue, client_socket);
        }
    }

    // =============== Cleanup ===============
    // TODO: Should really stop and join threads, but we don't really have a shutoff function either way
    close(server_socket);
    bb_del(queue);

    return EXIT_SUCCESS;
}
