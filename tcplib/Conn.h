//
// Created by itzaname on 5/1/18.
//

#ifndef CSCIFINAL_CONN_H
#define CSCIFINAL_CONN_H

#define CONN_READ_TIMEOUT 30
#define CONN_WRITE_TIMEOUT 30

#include <netinet/in.h>
#include <string>

class Conn {
public:
    Conn(int, sockaddr_in);
    ~Conn();
    size_t Read(char*, size_t);
    size_t Write(const char*, size_t);
    size_t Write(std::string);
    void Close();
private:
    int conn;
    sockaddr_in addr;
};


#endif //CSCIFINAL_CONN_H
