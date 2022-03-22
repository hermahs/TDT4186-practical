#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <error.h>
#include <stdlib.h>
#include "http.h"
#include "files.h"
#include "main.h"

#define min(a, b) ((a)<(b)?(a):(b))

thread_local char recv_buffer[BUFFER_SIZE], send_buffer[BUFFER_SIZE];

const char* HTML_CONTENT_TYPE = "text/html; charset=utf-8";
const char* JPEG_CONTENT_TYPE = "image/jpeg";

int handle_get_request() {
    char* path = get_path(recv_buffer); // We don't own the path memory
    if (path[0]=='/' && path[1]=='\0') // Replace / with the default index.html
        path = "/index.html";

    char* file_content = NULL;
    int file_len = get_file_from_path(&file_content, settings.origin_path, path);

    const char* content_type = HTML_CONTENT_TYPE;
    if (has_extension(path, ".jpg") || has_extension(path, ".jpeg"))
        content_type = JPEG_CONTENT_TYPE;

    int send;
    if (file_len >= 0) {
        send = snprintf(send_buffer, sizeof(send_buffer),
                        "HTTP/1.0 200 OK"CRLF
                        "Content-Length: %ld"CRLF
                        "Content-Type: %s"CRLF
                        CRLF,
                        file_len, content_type);
    } else {
        // The file didn't exist try 404 page instead
        file_len = get_file_from_path(&file_content, settings.origin_path, "/404.html");

        // Even the 404.html file doesn't exist, just send some text
        if (file_len < 0) {
            file_content = strdup("Not found");
            file_len = strlen(file_content);
        }

        send = snprintf(send_buffer, sizeof(send_buffer),
                        "HTTP/1.0 404 Not Found"CRLF
                        "Content-Length: %ld"CRLF
                        "Content-Type: %s"CRLF
                        CRLF,
                        file_len, HTML_CONTENT_TYPE);
    }

    if (send+file_len > sizeof(send_buffer)) {
        error(0, 0, "warning: Not enough room for http output!\n");
        return min(send, sizeof(send_buffer)-1);
    }

    printf("file len: %d", file_len);
    memcpy(send_buffer+send, file_content, file_len);
    free(file_content);

    return send+file_len;
}
