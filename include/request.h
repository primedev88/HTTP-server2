//
// Created by anuj on 13/6/25.
//

#pragma once
#include <string>
#include <unordered_map>

struct HttpRequest {
    std::string method;
    std::string path;
    std::unordered_map<std::string, std::string> headers;

    static HttpRequest parse(const std::string& raw);
};