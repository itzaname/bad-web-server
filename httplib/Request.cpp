//
// Created by itzaname on 5/1/18.
//

#include <sstream>
#include <cstring>
#include <algorithm>
#include "httplib.h"
#include "Handler.h"

Request::Request(char *headers, std::vector<char> *requestBody, std::vector<char> *responseBody) {
    this->requestBody = requestBody;
    this->responseBody = responseBody;
    this->status = 200;
    this->statusString = "OK";

    DPRINT("[Request] New request started.\n");

    if (!parseRequest(headers))
        throw "Failed to parse request";
}

Request::~Request() {
    delete url;
    DPRINT("[Request] Request has finished.\n");
}

Url* Request::URL() {
    return url;
}

std::vector<char>* Request::Body() {
    return requestBody;
}

std::string Request::Method() {
    return method;
}

int Request::GetStatus() {
    return status;
}

std::string Request::GetStatusString() {
    return statusString;
}

std::string Request::GetHeader(std::string key) {
    std::transform(key.begin(), key.end(), key.begin(), ::tolower);

    return requestHeaders[key];
}

std::string Request::GetHeaderString() {
    std::string headerString = "";
    for (auto const& h : responseHeaders) {
        headerString += h.first + ": " + h.second + "\r\n";
    }

    return headerString;
}

void Request::SetHeader(std::string key, std::string value) {
    std::transform(key.begin(), key.end(), key.begin(), ::tolower);
    std::transform(value.begin(), value.end(), value.begin(), ::tolower);

    responseHeaders[key] = value;
}

void Request::SetStatus(int status, std::string msg) {
    this->status = status;
    this->statusString = msg;
}

void Request::Write(std::string data) {
    responseBody->insert(responseBody->end(), data.begin(), data.end());
}

void Request::Write(char* data, size_t size) {
    responseBody->insert(responseBody->end(), &data[0], &data[size]);
}

bool Request::parseRequest(char *headers) {
    uint32_t readPos = 0;

    // Read the request info line
    for (; readPos < HANDLER_MAX_HEADER_SIZE - 1; ++readPos) {
        if (readPos > 0 && !strncmp(&headers[readPos], "\r\n", 2)) {
            std::string header(&headers[0], &headers[readPos]);
            readPos += 2;
            if (!parseDataHeader(header))
                return false;
            break;
        }
    }

    // Read out the rest of the request headers
    uint32_t lastHeader = readPos;
    for (; readPos < HANDLER_MAX_HEADER_SIZE - 1; ++readPos) {
        if (!strncmp(&headers[readPos], "\r\n\r\n", 4)) {
            readPos += 4;
            break;
        }
        if (!strncmp(&headers[readPos], "\r\n", 2)) {
            std::string header(&headers[lastHeader], &headers[readPos]);
            readPos += 2;
            lastHeader = readPos;
            if (!addRawHeader(header))
                return false;
        }
    }

    return true;
}

bool Request::parseDataHeader(const std::string &header) {
    size_t firstSpace = header.find(' ');
    if (firstSpace == std::string::npos)
        return false;

    method = header.substr(0, firstSpace);

    size_t secondSpace = header.find(' ', firstSpace + 1);
    if (secondSpace == std::string::npos)
        return false;

    try {
        url = new Url(header.substr(firstSpace + 1, secondSpace - firstSpace));
    } catch (...) {
        // I could care less what the exception is tbh
        return false;
    }

    return true;
}

bool Request::addRawHeader(const std::string &header) {
    size_t separator = header.find(":");
    if (separator == std::string::npos)
        return false;

    std::string key = header.substr(0, separator);
    std::string value = header.substr(separator + 1);

    // I really didn't look into this far enough to see if this is going to crash using multi byte but I don't really care right now
    std::transform(key.begin(), key.end(), key.begin(), ::tolower);
    std::transform(value.begin(), value.end(), value.begin(), ::tolower);

    // Trim the space on value
    value.erase(0, value.find_first_not_of(' '));
    value.erase(value.find_last_not_of(' ') + 1);

    requestHeaders[key] = value;

    DPRINT("[Request] Header parsed. Key: %s -- Value: %s\n", key.c_str(), value.c_str());

    return true;
}