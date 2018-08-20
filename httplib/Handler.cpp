//
// Created by itzaname on 5/2/18.
//

#include <cstring>
#include <vector>
#include <sstream>

#include "Handler.h"

Handler::Handler(Conn *conn, requestfn handler) {
    this->conn = conn;
    this->handler = handler;
    this->excessBuffer = new char[HANDLER_BUFFER_SIZE];
    this->excessBufferSize = NULL;
}

Handler::~Handler() {
    delete[] excessBuffer;
    conn->Close();
}

bool Handler::Handle() {
    DPRINT("[Handler] Starting handler for connection...\n");

    while (handleRequest());

    DPRINT("[Handler] Completed.\n");

    return true;
}

bool Handler::writeError(int status, std::string msg) {
    std::ostringstream infoHeader;
    infoHeader << "HTTP/1.1 ";
    infoHeader << status;
    infoHeader << " ";
    infoHeader << msg;
    conn->Write(infoHeader.str() + "\r\n");
    conn->Write("Connection: Closed\r\n\r\n");
    conn->Write(msg);
}

bool Handler::handleRequest() {
    auto headerBuffer = new char[HANDLER_MAX_HEADER_SIZE];
    size_t readPos = 0;
    bool foundHeaders = false;

    // Copy in excess buffer data from last iteration
    if (excessBufferSize > 0) {
        memcpy(headerBuffer, excessBuffer, excessBufferSize);
        readPos = excessBufferSize;
        DPRINT("[Handler] Imported %d bytes from existing buffer.\n", excessBufferSize);
    }

    size_t readSize = conn->Read(&headerBuffer[readPos], HANDLER_MAX_HEADER_SIZE - readPos);
    if (!readSize || readSize > HANDLER_MAX_HEADER_SIZE) {
        delete []headerBuffer;
        DPRINT("[Handler] Bad request: Failed to read header from socket.\n");
        writeError(400, "BAD REQUEST");
        return false;
    }

    // Find size of headers
    for (; readPos < readSize - 1; ++readPos) {
        if (!strncmp(&headerBuffer[readPos], "\r\n\r\n", 4)) {
            readPos += 4;
            foundHeaders = true;
            break;
        }
    }

    // If we couldn't find the end of the headers within defined limits
    if (!foundHeaders) {
        delete []headerBuffer;
        DPRINT("[Handler] Bad request: Headers not found.\n");
        writeError(400, "BAD REQUEST");
        return false;
    }

    // Now we will handle the request
    std::vector<char> requestBody;
    std::vector<char> responseBody;
    bool continueConnection = false;

    // headerBuffer may contain extra data but it shouldn't matter that much
    try {
        Request request(headerBuffer, &requestBody, &responseBody);

        request.SetHeader("server", "Stuart's Server");

        // Handle keep alive
        if (request.GetHeader("connection") == "keep-alive") {
            request.SetHeader("Connection", "Keep-Alive");
            request.SetHeader("Keep-Alive", "timeout=5, max=9999");
            continueConnection = true;
        } else {
            request.SetHeader("Connection", "Closed");
        }

        // Read body into vector
        if (continueConnection) {
            // Keep alive
            int len = atoi(request.GetHeader("Content-Length").c_str());
            if (len > HANDLER_MAX_BODY_SIZE) {
                delete []headerBuffer;
                writeError(413, "PAYLOAD TOO LARGE");
                return false;
            }


            DPRINT("[Handler] Reading body with a length of %d\n", len);

            // If we have data left then keep reading
            if (len > 0) {
                int remaining = readSize - readPos;
                if (remaining > len)
                    remaining = len;

                // So now we copied in the remaining buffer
                requestBody.insert(requestBody.end(), &headerBuffer[readPos], &headerBuffer[readPos + remaining]);

                readPos = readSize;

                size_t leftRead = len - remaining;
                auto buffer = new char[leftRead];
                size_t totalRead = 0;

                // Loop until we have read the specified length
                while (leftRead > 0) {
                    size_t bufferRead = conn->Read(&buffer[totalRead], leftRead);
                    if (bufferRead == 0) {
                        delete []headerBuffer;
                        return false;
                    }

                    leftRead -= bufferRead;
                    totalRead += bufferRead;
                }

                requestBody.insert(requestBody.end(), &buffer[0], &buffer[totalRead]);

                delete[] buffer;
            }
        } else {
            DPRINT("[Handler] Reading body of unknown length\n");

            // Copy remaining header data
            requestBody.insert(requestBody.end(), &headerBuffer[readPos], &headerBuffer[readSize]);
            readPos = readSize;

            // If our buffer was full then continue reading
            if (readSize == HANDLER_MAX_HEADER_SIZE) {
                auto buffer = new char[HANDLER_BUFFER_SIZE];
                size_t bufferRead = 0;
                size_t totalRead = 0;
                while (totalRead < HANDLER_MAX_BODY_SIZE) {
                    bufferRead = conn->Read(buffer, HANDLER_BUFFER_SIZE);
                    requestBody.insert(requestBody.end(), &buffer[0], &buffer[bufferRead]);
                    totalRead += bufferRead;
                    if (bufferRead != HANDLER_BUFFER_SIZE)
                        break;
                }

                delete[] buffer;
            }

            DPRINT("[Handler] Read a total of %d bytes.\n", requestBody.size());
        }

        // Finally call the handler function
        handler(&request);

        // Write out our remaining request data
        std::ostringstream infoHeader;
        infoHeader << "HTTP/1.1 ";
        infoHeader << request.GetStatus();
        infoHeader << " ";
        infoHeader << request.GetStatusString();
        infoHeader << "\r\nContent-Length: ";
        infoHeader << responseBody.size();
        conn->Write(infoHeader.str() + "\r\n");
        conn->Write(request.GetHeaderString() + "\r\n");
        if (responseBody.size() > 0)
            conn->Write(&responseBody[0], responseBody.size());
    } catch(char const* e) {
        delete []headerBuffer;
        writeError(400, "BAD REQUEST");
        DPRINT("[Handler] Bad request: Failed to start request. %s\n", e);
        return false;
    }


    // Copy our leftover data into the excess buffer
    if (readPos < readSize) {
        excessBufferSize = readSize - readPos;
        // Shouldn't happen but who knows
        if (excessBufferSize < 0 || excessBufferSize < HANDLER_MAX_HEADER_SIZE) {
            DPRINT("[Handler] Leftover data in header buffer totalling %d bytes %d %d.\n", excessBufferSize, readPos, readSize);
            memcpy(excessBuffer, &headerBuffer[readPos], excessBufferSize);
        } else
            excessBufferSize = 0;
    }

    delete []headerBuffer;

    // Continue if connection was specified to stay alive
    return continueConnection;
}
