#include "request_handler.h"
#include "faiss_index.h"
#include "public_constant.h"

void RequestHandler::HandleRequest(const httplib::Request &req, httplib::Response &res) {
    GlobalLogger->debug("Received request: {}", req.body);

    // 1. parse
    rapidjson::Document json_request;
    ParseRequest(req, json_request);

    // 2. verify
    ValidateRequest(json_request);

    // 3. execute
    auto json_response = ExecuteProcess(json_request);

    // 4. response
    SetResponse(res, json_response);
}

void RequestHandler::ProcessRequest(const httplib::Request &req, httplib::Response &res) {
    try {
        HandleRequest(req, res);
    } catch (const std::exception &e) {
        HandleError(res, e);
    }
}

IndexFactory::IndexType RequestHandler::GetIndexTypeByReq(const rapidjson::Document &json_request) {
    if (json_request.HasMember(REQUEST_INDEXT_TYPE) &&
        json_request[REQUEST_INDEXT_TYPE].IsString()) {
        std::string index_type_str = json_request[REQUEST_INDEXT_TYPE].GetString();
        return IndexFactory::IndexTypeFromString(index_type_str);
    }
    return IndexFactory::IndexType::UNKNOWN;
}

void SearchHandler::ValidateRequest(const rapidjson::Document &json_request) {
    if (!(json_request.HasMember(REQUEST_VECTOR) && json_request.HasMember(REQUEST_TOP_K) &&
          (json_request.HasMember(REQUEST_INDEXT_TYPE) &&
           json_request[REQUEST_INDEXT_TYPE].IsString()))) {
        throw std::runtime_error("Invalid search request parameters");
    }
}

rapidjson::Document SearchHandler::ExecuteProcess(const rapidjson::Document &json_request) {
    std::vector<float> query_vector;
    for (const auto &data : json_request[REQUEST_VECTOR].GetArray()) {
        query_vector.push_back(data.GetFloat());
    }
    uint64_t top_k = json_request[REQUEST_TOP_K].GetUint64();

    IndexFactory::IndexType index_type = RequestHandler::GetIndexTypeByReq(json_request);
    if (index_type == IndexFactory::IndexType::UNKNOWN) {
        throw std::runtime_error("Unknown index type");
    }

    void *index = GetGlobalIndexFactory()->GetIndex(index_type);

    // TODO:func router
    std::pair<std::vector<long>, std::vector<float>> search_result;
    switch (index_type) {
        case IndexFactory::IndexType::FLAT: {
            FaissIndex *faiss_index = static_cast<FaissIndex *>(index);
            search_result = faiss_index->SearchVectorsTopK(query_vector, top_k);
            break;
        }

        default:
            throw std::runtime_error("Unsupported index type for search");
    }

    rapidjson::Document json_response;
    json_response.SetObject();
    rapidjson::Document::AllocatorType &allocator = json_response.GetAllocator();

    rapidjson::Value vectors(rapidjson::kArrayType);
    rapidjson::Value distances(rapidjson::kArrayType);
    for (size_t i = 0; i < search_result.first.size(); ++i) {
        if (search_result.first[i] != -1) {
            vectors.PushBack(search_result.first[i], allocator);
            distances.PushBack(search_result.second[i], allocator);
        }
    }

    json_response.AddMember(RESP_VECTORS, vectors, allocator);
    json_response.AddMember(RESP_DISTANCES, distances, allocator);
    json_response.AddMember(RESP_RETCODE, RESP_SUCC_RET, allocator);
    return json_response;
}

void InsertHandler::ValidateRequest(const rapidjson::Document &json_request) {
    if (!(json_request.HasMember(REQUEST_VECTOR) && json_request.HasMember(REQUEST_ID) &&
          (json_request.HasMember(REQUEST_INDEXT_TYPE) &&
           json_request[REQUEST_INDEXT_TYPE].IsString()))) {
        throw std::runtime_error("Invalid insert request parameters");
    }
}

rapidjson::Document InsertHandler::ExecuteProcess(const rapidjson::Document &json_request) {
    std::vector<float> insert_data;
    for (const auto &d : json_request[REQUEST_VECTOR].GetArray()) {
        insert_data.push_back(d.GetFloat());
    }
    uint64_t id = json_request[REQUEST_ID].GetUint64();

    IndexFactory::IndexType index_type = RequestHandler::GetIndexTypeByReq(json_request);
    if (index_type == IndexFactory::IndexType::UNKNOWN) {
        throw std::runtime_error("Unknown index type");
    }

    void *index = GetGlobalIndexFactory()->GetIndex(index_type);

    switch (index_type) {
        case IndexFactory::IndexType::FLAT: {
            FaissIndex *faiss_index = static_cast<FaissIndex *>(index);
            faiss_index->InsertVectorsWithUsrId(insert_data, id);
            break;
        }
        default:
            throw std::runtime_error("Unsupported index type for insert");
    }

    // 构建响应
    rapidjson::Document json_response;
    json_response.SetObject();
    rapidjson::Document::AllocatorType &allocator = json_response.GetAllocator();

    json_response.AddMember(RESP_RETCODE, RESP_SUCC_RET, allocator);

    return json_response;
}
