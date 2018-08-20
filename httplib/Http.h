//
// Created by itzaname on 5/1/18.
//

#ifndef CSCIFINAL_HTTP_H
#define CSCIFINAL_HTTP_H

#include "../tcplib/tcplib.h"
#include "Request.h"

typedef void (*requestfn)(Request*);

class Http : public Socket {
public:
    Http(std::string, uint16_t, requestfn);
private:
    void dispatcher();
    requestfn handler;
    bool run;
};


#endif //CSCIFINAL_HTTP_H
