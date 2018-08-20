#include <iostream>
#include <fstream>

#include "httplib/Http.h"

std::map<std::string, requestfn> routes;

void google(Request *r) {
    r->SetHeader("Location", "https://google.com");
    r->SetStatus(301, "Moved Permanently");
    r->Write("Redirect....");
}

void hello(Request *r) {
    r->Write("Hello, " + r->URL()->QueryValue("name") + "!");
}

void index(Request *r) {
    r->Write("Hi!");
}

void test(Request *r) {
    r->Write("Testing!!");
}

void dir(Request *r) {
    std::ifstream file(r->URL()->Path().erase(0,1));
    if (file.good()) {
        try {
            r->Write(std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>()));
        } catch (std::ios_base::failure e) {
            r->SetStatus(404, "NOT FOUND");
            r->Write("404 Not Found");
        }
        return;
    }

    r->SetStatus(404, "NOT FOUND");
    r->Write("404 Not Found");
}

int main() {
    routes["/"] = index;
    routes["/test"] = test;
    routes["/hello"] = hello;
    routes["/google"] = google;

    try {
        Http server("0.0.0.0", 8080, [](Request *r) {
            if (routes.count(r->URL()->Path()))
                routes[r->URL()->Path()](r);
            else
                dir(r);
        });
    }
    catch (char const* e) {
        std::cout << e << std::endl;
    }

    return 0;
}