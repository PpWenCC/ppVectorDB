#include <iostream>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include "http_server.h"
#include "logger.h"

HttpServer::HttpServer(const std::string &host, int port) : host(host), port(port)
{
    server.Post("/search", [this](const httplib::Request &req, httplib::Response &res)
                { SearchHandler(req, res); });

    server.Post("/insert", [this](const httplib::Request &req, httplib::Response &res)
                { InsertHandler(req, res); });
}

void HttpServer::start()
{
    server.listen(host.c_str(), port);
}

void HttpServer::SearchHandler(const httplib::Request &req, httplib::Response &res)
{
    GlobalLogger->debug("Received search request");
    // TODO
}

void HttpServer::InsertHandler(const httplib::Request &req, httplib::Response &res)
{
    GlobalLogger->debug("Received insert request");
    // TODO
}
