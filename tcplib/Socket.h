//
// Created by itzaname on 5/1/18.
//

#ifndef CSCIFINAL_SOCKET_H
#define CSCIFINAL_SOCKET_H

#include <cstdint>
#include <string>
#include "Conn.h"

typedef void (*handlerfn)(Conn*);

class Socket {
public:
    Socket();
    ~Socket();
    Socket(std::string, uint16_t);
    Socket(std::string, uint16_t, handlerfn);
    Conn* Accept();
    void Handle();
    void Close();
protected:
    uint16_t port;
    std::string bindaddr;
    bool open();
private:
    int sock;
    bool quit;

    handlerfn handler;
};


#endif //CSCIFINAL_SOCKET_H
