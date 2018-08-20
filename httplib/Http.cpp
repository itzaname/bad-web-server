//
// Created by itzaname on 5/1/18.
//

#include <thread>
#include <iostream>
#include "Http.h"
#include "Handler.h"

Http::Http(std::string bindaddr, uint16_t port, requestfn handler) {
    this->bindaddr = bindaddr;
    this->port = port;
    this->handler = handler;
    this->run = true;

    if (!open())
        throw "Failed to bind socket.";

    dispatcher();
}


void Http::dispatcher() {
    while (run) {
        auto conn = Accept();
        if (conn == NULL)
            continue;

        std::thread handlerThread([](Conn* conn, requestfn handler) {
            // We'll create the handler in a new thread so that we don't have to worry about closing the request
            Handler httpHandler(conn, handler);
            httpHandler.Handle();
        }, conn, handler);
        handlerThread.detach();
    }
}