#include "http_server.h"
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <iostream>
#include "logger.h"
#include "public_constant.h"
#include "request_handler.h"

HttpServer::HttpServer(const std::string &host, int port) : host(host), port(port) {
    REGISTER_HANDLER("/search", SearchHandler);
    REGISTER_HANDLER("/insert", InsertHandler);
}

void HttpServer::start() {
    // 动态绑定路由
    for (const auto &endpoint : {"search", "insert"}) {
        server.Post("/" + std::string(endpoint),
                    [this](const httplib::Request &req, httplib::Response &res) {
                        auto handler = HandlerFactory::Instance().CreateHandler(req.path);
                        if (handler) {
                            handler->ProcessRequest(req, res);
                        } else {
                            res.status = 404;
                            SetErrJsonResponse(res, res.status, "request handler not found");
                        }
                    });
    }

    server.listen(host.c_str(), port);
}

void HttpServer::SetJsonResponse(const rapidjson::Document &json_response, httplib::Response &res) {
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    json_response.Accept(writer);

    res.set_content(buffer.GetString(), RESP_CONTENT_TYPE_JSON);
}

void HttpServer::SetErrJsonResponse(httplib::Response &res, int error_code,
                                    const std::string &errorMsg) {
    rapidjson::Document json_response;
    json_response.SetObject();
    json_response.AddMember("error_code", error_code, json_response.GetAllocator());
    json_response.AddMember("error_message", rapidjson::StringRef(errorMsg.c_str()),
                            json_response.GetAllocator());

    SetJsonResponse(json_response, res);
}
