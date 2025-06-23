#pragma once

#include <string>
#include <rapidjson/document.h>
#include "httplib.h"

class HttpServer
{
public:
    enum class CheckType
    {
        SEARCH,
        INSERT
    };

    HttpServer(const std::string &host, int port);
    void start();

private:
    void SearchHandler(const httplib::Request &req, httplib::Response &res);
    void InsertHandler(const httplib::Request &req, httplib::Response &res);
    // TODO
    // void SetJsonResponse(const rapidjson::Document &json_response, httplib::Response &res);
    // void SetErrorJsonResponse(httplib::Response &res, int error_code, const std::string &errorMsg);
    // bool IsRequestValid(const rapidjson::Document &json_request, CheckType check_type);

    httplib::Server server;
    std::string host;
    int port;
};
