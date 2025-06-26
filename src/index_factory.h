#pragma once

#include <map>
#include <string>
#include <unordered_map>
#include "faiss_index.h"

// 单例工厂
class IndexFactory {
   public:
    enum class IndexType { FLAT, UNKNOWN = -1 };

    enum class MetricType { L2, IP };

    void Init(IndexType type, int dim, MetricType metric = MetricType::L2);
    void *GetIndex(IndexType type) const;

    static IndexFactory::IndexType IndexTypeFromString(const std::string &type_str) {
        static const std::unordered_map<std::string, IndexFactory::IndexType> mapping = {
            {"FLAT", IndexFactory::IndexType::FLAT}};

        auto it = mapping.find(type_str);
        return (it != mapping.end()) ? it->second : IndexFactory::IndexType::UNKNOWN;
    }

   private:
    std::map<IndexType, void *> index_map;
};

IndexFactory *GetGlobalIndexFactory();
