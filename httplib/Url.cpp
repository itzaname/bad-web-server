//
// Created by itzaname on 5/1/18.
//

#include <iostream>
#include "Url.h"

Url::Url(std::string url) {
    this->url = url;
    if (!parse())
        throw "Invalid URL";
}

bool Url::parse() {
    // Parse the protocol
    size_t protoPos = url.find("://");
    if (protoPos != std::string::npos) {
        protocol = url.substr(0, protoPos);
        protoPos += 3;
    } else
        protoPos = 0;

    if (url.size() == protoPos)
        return true;

    // Parse the host
    size_t hostPos = url.find('/', protoPos);
    if (hostPos == std::string::npos)
        hostPos = url.find('?');
    if (hostPos == std::string::npos)
        hostPos = url.size();

    host = url.substr(protoPos, hostPos - protoPos);

    if (hostPos == url.size())
        return true;

    // Parse the path
    size_t pathPos = url.find('?');
    if (pathPos == std::string::npos)
        pathPos = url.size();

    path = url.substr(hostPos, pathPos - hostPos);
    path.erase(0, path.find_first_not_of(' '));
    path.erase(path.find_last_not_of(' ') + 1);

    if (pathPos == url.size())
        return true;

    // Parse the query string
    size_t queryPos = pathPos;
    while (queryPos != std::string::npos) {
        size_t nextPos = url.find('&', queryPos + 1);
        if (nextPos == std::string::npos)
            parseQuery(url.substr(queryPos + 1, url.size() - queryPos - 1));
        else
            parseQuery(url.substr(queryPos + 1, nextPos - queryPos - 1));

        queryPos = nextPos;
    }

    return true;
}

bool Url::parseQuery(std::string param) {
    size_t separator = param.find('=');
    if (separator == std::string::npos)
        return false;

    std::string key = param.substr(0, separator);
    std::string value = param.substr(separator + 1);
    value.erase(0, value.find_first_not_of(' '));
    value.erase(value.find_last_not_of(' ') + 1);

    if (key.size() == 0 || value.size() == 0)
        return false;

    query[key] = value;

    return true;
}

std::string Url::QueryValue(std::string key) {
    return query[key];
}

std::string Url::Host() {
    return host;
}

std::string Url::Path() {
    return path;
}

std::string Url::Protocol() {
    return protocol;
}

