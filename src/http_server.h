#pragma once

#include <rapidjson/document.h>
#include <string>
#include "httplib.h"
#include "index_factory.h"


class HttpServer {
   public:
    HttpServer(const std::string &host, int port);
    void start();

    static void SetJsonResponse(const rapidjson::Document &json_response, httplib::Response &res);
    static void SetErrJsonResponse(httplib::Response &res, int error_code, const std::string &errorMsg);

   private:
    httplib::Server server;
    std::string host;
    int port;
};
