//
// Created by itzaname on 5/1/18.
//

#include <mhash.h>
#include "tcplib.h"

Conn::Conn(int conn, sockaddr_in addr) {
    this->conn = conn;
    this->addr = addr;
    DPRINT("[Conn] Connection opened.\n");
}

Conn::~Conn() {
    Close();
}

// Read will read data from the connection and cancel if it exceeds the allotted time.
size_t Conn::Read(char *buffer, size_t size) {
    // Init read file descriptor
    fd_set readfd;
    FD_ZERO(&readfd);
    FD_SET(conn, &readfd);

    // Set our timeout to whats defined
    timeval timeout;
    timeout.tv_sec = CONN_READ_TIMEOUT;
    timeout.tv_usec = NULL;

    // Perform read if we get data within the timeout
    if (select(conn + 1, &readfd, NULL, NULL, &timeout) == 1)
        return read(conn, buffer, size);

    // We exceeded the timeout
    return NULL;
}

// Write will write data to the connection and cancel if it exceeds the time limit
size_t Conn::Write(const char *buffer, size_t size) {
    // Init write file descriptor
    fd_set writefd;
    FD_ZERO(&writefd);
    FD_SET(conn, &writefd);

    // Set our timeout to whats defined
    timeval timeout;
    timeout.tv_sec = CONN_WRITE_TIMEOUT;
    timeout.tv_usec = NULL;

    // Perform write if available in time frame
    if (select(conn + 1, NULL, &writefd, NULL, &timeout) == 1)
        return send(conn, buffer, size, MSG_NOSIGNAL);

    // We exceeded the timeout
    return NULL;
}

size_t Conn::Write(std::string buffer) {
    return Write(buffer.c_str(), buffer.size());
};

void Conn::Close() {
    close(conn);
    DPRINT("[Conn] Connection closed.\n");
}