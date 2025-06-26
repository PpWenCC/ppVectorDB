#pragma once
#include <functional>
#include <memory>
#include <unordered_map>
#include "http_server.h"
#include "index_factory.h"
#include "logger.h"
#include "rapidjson/document.h"

class RequestHandler {
   public:
    virtual ~RequestHandler() = default;

    void HandleRequest(const httplib::Request &req, httplib::Response &res);
    void ProcessRequest(const httplib::Request &req, httplib::Response &res);

   protected:
    IndexFactory::IndexType GetIndexTypeByReq(const rapidjson::Document &json_request);
    virtual void ValidateRequest(const rapidjson::Document &json_request) = 0;
    virtual rapidjson::Document ExecuteProcess(const rapidjson::Document &json_request) = 0;
    virtual std::string GetHandlerName() const = 0;
    virtual void ParseRequest(const httplib::Request &req, rapidjson::Document &json_request) {
        json_request.Parse(req.body.c_str(), req.body.size());
        if (json_request.HasParseError() || !json_request.IsObject()) {
            throw std::runtime_error("Invalid JSON request format");
        }
    }

    virtual void SetResponse(httplib::Response &res, rapidjson::Document &json_response) {
        HttpServer::SetJsonResponse(json_response, res);
    }

    virtual void HandleError(httplib::Response &res, const std::exception &e) {
        GlobalLogger->error("{} error: {}", GetHandlerName(), e.what());
        res.status = 400;  // Bad Request
        HttpServer::SetErrJsonResponse(res, res.status, e.what());
    }
};

class SearchHandler : public RequestHandler {
   protected:
    void ValidateRequest(const rapidjson::Document &json_request) override;
    rapidjson::Document ExecuteProcess(const rapidjson::Document &json_request) override;
    std::string GetHandlerName() const override { return "SearchHandler"; }
};

class InsertHandler : public RequestHandler {
   protected:
    void ValidateRequest(const rapidjson::Document &json_request) override;
    rapidjson::Document ExecuteProcess(const rapidjson::Document &json_request) override;
    std::string GetHandlerName() const override { return "InsertHandler"; }
};

class HandlerFactory {
   public:
    using HandlerCreator = std::function<std::unique_ptr<RequestHandler>()>;

    static HandlerFactory &Instance() {
        static HandlerFactory instance;
        return instance;
    }

    void RegisterHandler(const std::string &endpoint, HandlerCreator creator) {
        handlers[endpoint] = std::move(creator);
    }

    std::unique_ptr<RequestHandler> CreateHandler(const std::string &endpoint) {
        auto it = handlers.find(endpoint);
        if (it != handlers.end()) {
            return it->second();
        }
        return nullptr;
    }

   private:
    std::unordered_map<std::string, HandlerCreator> handlers;
};

#define REGISTER_HANDLER(endpoint, handler_type)                                                   \
    HandlerFactory::Instance().RegisterHandler(endpoint, []() -> std::unique_ptr<RequestHandler> { \
        return std::make_unique<handler_type>();                                                   \
    })
