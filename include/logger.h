//
// Created by anuj on 13/6/25.
//

#pragma once
#include <string>

class Logger {
public:
    static void init(const std::string& filename);
    static void log(const std::string& message);
};
