//
// Created by anuj on 13/6/25.
//
#include "server.h"
#include "logger.h"
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <fcntl.h>
#include <fstream>
#include <sstream>
#include <utility>
#include <sys/stat.h>
#include <dirent.h>

Server::Server(int port, std::string  www_root): port(port), www_root(std::move(www_root)), pool(4) {
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 10) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    std::cout << "Server listening on port " << port << std::endl;
}

void Server::start() {
    while (true) {
        socklen_t addrlen = sizeof(address);
        int client_fd = accept(server_fd, (struct sockaddr*)&address, &addrlen);
        if ( client_fd < 0) {
            perror("accept");
            continue;
        }

        pool.enqueue([this, client_fd]() {
            this->handleClient(client_fd);
            close(client_fd);
        });
    }
}

void Server::handleClient(int client_fd) {
    char buffer[8192] = {0};
    int bytesRead = read(client_fd, buffer, sizeof(buffer));
    if (bytesRead <= 0) return;

    std::string request_str(buffer);
    HttpRequest req = HttpRequest::parse(request_str);

    std::string path = req.path;
    if (path == "/") path = "/index.html";
    std::string full_path = www_root + path;

    struct stat stat_buf;
    if (stat(full_path.c_str(), &stat_buf) == 0) {
        if (S_ISREG(stat_buf.st_mode)) {
            std::string content = readFile(full_path);
            std::string mime = getMimeType(full_path);
            std::string response = "HTTP/1.1 200 OK\r\nContent-Type: " + mime + "\r\n\r\n" + content;
            send(client_fd, response.c_str(), response.size(), 0);
            Logger::log(req.method + " " + path + " 200");
        } else if (S_ISDIR(stat_buf.st_mode)) {
            std::string content = directoryListing(full_path);
            std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n" + content;
            send(client_fd, response.c_str(), response.size(), 0);
            Logger::log(req.method + " " + path + " 200 (dir)");
        }
    } else {
        std::string content = readFile(www_root + "/404.html");
        std::string response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n" + content;
        send(client_fd, response.c_str(), response.size(), 0);
        Logger::log(req.method + " " + path + " 404");
    }
}

std::string Server::readFile(const std::string& path) {
    std::ifstream file(path);
    std::cout << "Trying to read file: " << path << std::endl;
    if (!file.is_open()) return "<html><body><h1>404 Not Found</h1></body></html>";
    std::ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

std::string Server::getMimeType(const std::string& path) {
    if (path.ends_with(".html")) return "text/html";
    if (path.ends_with(".css")) return "text/css";
    if (path.ends_with(".js")) return "application/javascript";
    if (path.ends_with(".png")) return "image/png";
    if (path.ends_with(".jpg") || path.ends_with(".jpeg")) return "image/jpeg";
    return "text/plain";
}

std::string Server::directoryListing(const std::string &path) {
    DIR* dir = opendir(path.c_str());
    if (!dir) return "<html><body><h1>403 Forbidden</h1></body></html";

    std::ostringstream html;
    html << "<html><body><h1>Index of " << path << "</h1><ul>";
    struct dirent* entry;
    while((entry = readdir(dir)) != NULL) {
        html << "<li><a href ='" << entry->d_name << "'>" << entry->d_name << "</a></li>";
    }
    html << "</ul></body></html>";
    closedir(dir);
    return html.str();
}