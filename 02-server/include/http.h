#ifndef HTTP_H_
#define HTTP_H_

#include <threads.h>

#define CRLF "\r\n"
#define BUFFER_SIZE (1024*1024)
extern thread_local char recv_buffer[BUFFER_SIZE], send_buffer[BUFFER_SIZE];

/** Reads the contents of recv_buffer, parsing the GET request,
 * and fills the send_buffer with an HTTP response for that file.
 * Looks up files relaive to settings.origin_path
 *
 * Returns the number of bytes of text written to the send_buffer (excluding 0-terminator)
 */
int handle_get_request();

#endif // HTTP_H_
