//
// Created by anuj on 13/6/25.
//

#pragma once
#include <string>
#include <netinet/in.h>
#include <thread>
#include <vector>
#include "threadpool.h"
#include "request.h"

class Server {
public:
    Server(int port, std::string  www_root);
    void start();

private:
    int port;
    std::string www_root;
    int server_fd;
    sockaddr_in address;
    ThreadPool pool;
    void handleClient(int client_fd);
    std::string getMimeType(const std::string& path);
    std::string readFile(const std::string& path);
    std::string directoryListing(const std::string& path);
};