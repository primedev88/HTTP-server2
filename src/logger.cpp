//
// Created by anuj on 13/6/25.
//
#include "logger.h"
#include <fstream>
#include <mutex>
#include <ctime>

static std::mutex log_mutex;
static std::string log_file;

void Logger::init(const std::string& filename) {
    log_file = filename;
}

void Logger::log(const std::string& message) {
    std::lock_guard<std::mutex> lock(log_mutex);
    std::ofstream log(log_file, std::ios::app);
    std::time_t now = std::time(nullptr);
    log << std::ctime(&now) << ": "<< message << std::endl;
}