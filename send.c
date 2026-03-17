#include "common.h"
#include "send.h"
#include "file_utils.h"

void send_file_content(int connection_socket, char* filepath, char* buffer) {
    int n;
    uint32_t content_bytes;
    FILE* fp;

    // Open the file
    fp = open_file(filepath, "rb");
    if (fp == NULL) return;

    // Send content size (in bytes)
    fseek(fp, 0, SEEK_END);
    content_bytes = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    n = send(connection_socket, &content_bytes, sizeof(content_bytes), 0);
    if (n == -1) fatal_error("[ERROR SEND CONTENT LENGTH]");

    // Send file content
    while ((n = fread(buffer, 1, BUF_SIZE, fp)) > 0) {
        send(connection_socket, buffer, n, 0);
    }

    // Close the file
    fclose(fp);
}