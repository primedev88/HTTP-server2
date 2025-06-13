//
// Created by anuj on 13/6/25.
//

#include "request.h"
#include <sstream>

HttpRequest HttpRequest::parse(const std::string& raw) {
    HttpRequest req;
    std::istringstream ss(raw);
    ss >> req.method >> req.path;

    std::string header;
    while (std::getline(ss, header) && header != "\r") {
        size_t colon = header.find(":");
        if (colon != std::string::npos) {
            std::string key = header.substr(0, colon);
            std::string value = header.substr(colon + 1);
            if (!value.empty() && value.back() == '\r')
                value.pop_back();
            req.headers[key] = value;
        }
    }
    return req;
}