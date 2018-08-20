//
// Created by itzaname on 5/2/18.
//

#ifndef CSCIFINAL_HANDLER_H
#define CSCIFINAL_HANDLER_H

#define HANDLER_BUFFER_SIZE 4000
#define HANDLER_MAX_HEADER_SIZE 4000
#define HANDLER_MAX_BODY_SIZE 10000000

// So that I don't forget what we are doing here
// - Reading requests and parsing everything
// - Slitting up requests (If needed)
// - ???

#include "Http.h"

class Handler {
public:
    Handler(Conn*, requestfn);
    ~Handler();
    bool Handle();
private:
    Conn *conn;
    requestfn handler;

    bool handleRequest();
    bool writeError(int, std::string);

    char* excessBuffer;
    size_t excessBufferSize;
};


#endif //CSCIFINAL_HANDLER_H
