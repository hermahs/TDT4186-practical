#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>

int main(int argc, char *argv[]) {

    int socket_desc, client_socket, read_size;
    char buffer[6000];

    printf("hello?\n");

    if ((socket_desc = socket(AF_INET, SOCK_STREAM, 0)) < -1) {
        printf("could not create socket\n");
        return -1;
    }

    struct sockaddr_in server_addr, client_addr;

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(7202);

    if (bind(socket_desc, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        printf("bind error\n");
        return -1;
    }

    if (listen(socket_desc, 5) < 0) {
        printf("listen error");
        return -1;
    }

    printf("so its working\n");

    int c = sizeof(struct sockaddr_in);

    while (1) {
        client_socket = accept(socket_desc, (struct sockaddr *)&client_addr, (socklen_t*)&c);
        if (client_socket < 0) {
            printf("nani?! accept failed!!\n");
            return -1;
        }

        while ((read_size = recv(client_socket, buffer, 6000, 0)) > 0) {
            printf(buffer);
            char* split_buffer = strtok(buffer, "\n");
            write(client_socket, split_buffer[0], strlen(split_buffer[0]));
        }



        close(client_socket);
    }

    close(socket_desc);

    return 0;
}