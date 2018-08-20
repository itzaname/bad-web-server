//
// Created by itzaname on 5/1/18.
//

#ifndef CSCIFINAL_REQUEST_H
#define CSCIFINAL_REQUEST_H

#include <map>
#include <string>
#include <vector>

#include "../tcplib/Conn.h"
#include "Url.h"

#define MAX_HEADER_SIZE 4000

class Request {
public:
    Request(char*, std::vector<char>*, std::vector<char>*);
    ~Request();

    std::string GetHeader(std::string);
    std::string GetHeaderString();
    std::string Method();

    void SetHeader(std::string, std::string);
    void SetStatus(int, std::string);
    void Write(std::string);
    void Write(char*, size_t);

    int GetStatus();
    std::string GetStatusString();

    Url* URL();
    std::vector<char>* Body();
private:
    bool parseRequest(char*);
    bool parseDataHeader(const std::string&);
    bool addRawHeader(const std::string&);

    std::vector<char> *requestBody;
    std::vector<char> *responseBody;

    // Request info
    Url *url;
    int status;
    std::string statusString;
    std::string method;
    std::map<std::string, std::string> requestHeaders;
    std::map<std::string, std::string> responseHeaders;
};


#endif //CSCIFINAL_REQUEST_H
