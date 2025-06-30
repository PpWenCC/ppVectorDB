#include <iostream>
#include "http_server.h"
#include "logger.h"

int main() {
    // std::cout << "hello world" << std::endl;
    init_global_logger();
    set_log_level(spdlog::level::debug);  // 设置日志级别为debug
    GlobalLogger->info("Global logger initialized");

    // TEST
    int dim = 1;
    IndexFactory *globalIndexFactory = GetGlobalIndexFactory();
    globalIndexFactory->Init(IndexFactory::IndexType::FLAT, dim);
    globalIndexFactory->Init(IndexFactory::IndexType::HNSW, dim, IndexFactory::MetricType::L2, 10000);
    GlobalLogger->info("Global IndexFactory initialized");

    HttpServer server("localhost", 8080);
    GlobalLogger->info("HttpServer created");
    server.start();
    return 0;
}