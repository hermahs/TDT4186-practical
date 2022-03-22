#ifndef HTTP_H_
#define HTTP_H_

/** Reads the contents of recv_buffer, parsing the GET request,
 * and fills the send_buffer with an HTTP response for that file.
 * Looks up files relaive to settings.origin_path
 *
 * Returns the number of bytes of text written to the send_buffer (excluding 0-terminator)
 */
int handle_get_request();

#endif // HTTP_H_
