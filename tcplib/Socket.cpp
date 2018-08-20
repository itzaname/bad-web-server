//
// Created by itzaname on 5/1/18.
//

#include "tcplib.h"

#include <arpa/inet.h>
#include <mhash.h>


Socket::~Socket() {
    close(sock);
    DPRINT("[Socket] Socket closed.\n");
}

Socket::Socket() {
    this->bindaddr = "0.0.0.0";
    this->port = 8080;
}

Socket::Socket(std::string bindaddr , uint16_t port) {
    this->bindaddr = bindaddr;
    this->port = port;

    if (!open())
        throw "Failed to open socket.";
}

Socket::Socket(std::string bindaddr , uint16_t port, handlerfn handler) {
    this->bindaddr = bindaddr;
    this->port = port;
    this->handler = handler;

    if (!open())
        throw "Failed to open socket.";
}

Conn* Socket::Accept() {
    sockaddr_in clientAddress;
    socklen_t alen = sizeof(clientAddress);
    int connfd = accept(sock, (sockaddr *) &clientAddress, &alen);
    if (connfd <= 0) {
        DPRINT("[Socket] Failed to accept connection: Error %d\n", errno);
        return NULL;
    }

    return new Conn(connfd, clientAddress);
}

// Runs a blocking handler that utilizes the callback function
void Socket::Handle() {
    while (!quit) {
        Conn* conn = Accept();
        if (conn == NULL)
            continue;

        handler(conn);
    }
}

void Socket::Close() {
    close(sock);
}

bool Socket::open() {
    // Initialize a new socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        DPRINT("[Socket] Failed to create socket. Error 0x%X\n", errno);
        return false;
    }

    DPRINT("[Socket] Socket created.\n");

    // Convert string bindaddr to usable type
    in_addr_t addr = inet_addr(bindaddr.c_str());
    if (addr == INADDR_NONE) {
        // Invalid IPV4 notation
        DPRINT("[Socket] Invalid bind address: %s\n", bindaddr.c_str());
        return false;
    }

    sockaddr_in bindinfo;
    bindinfo.sin_family = AF_INET;
    bindinfo.sin_addr.s_addr = addr;
    bindinfo.sin_port = htons(port);

    // Bind our socket to the address and port specified
    if (bind(sock, reinterpret_cast<struct sockaddr*>(&bindinfo), sizeof(bindinfo)) < 0) {
        DPRINT("[Socket] Failed to bind to %s:%d.\n", bindaddr.c_str(), port);
        return false;
    }

    listen(sock, 5);

    return true;
}