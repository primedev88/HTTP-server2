//
// Created by anuj on 13/6/25.
//
#include "server.h"
#include "logger.h"

int main() {
    Logger::init("../logs/access.log");
    Server server(8080, "../www");
    server.start();
    return 0;
}