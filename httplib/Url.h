//
// Created by itzaname on 5/1/18.
//

#ifndef CSCIFINAL_URL_H
#define CSCIFINAL_URL_H

#include <map>
#include <string>

class Url {
public:
    Url(std::string);
    std::string QueryValue(std::string);
    std::string Host();
    std::string Path();
    std::string Protocol();
private:
    std::string url, host, path, protocol;
    bool parse();
    bool parseQuery(std::string);
    std::map<std::string, std::string> query;
};


#endif //CSCIFINAL_URL_H
