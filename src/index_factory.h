#pragma once

#include <map>
#include <string>
#include <unordered_map>

// 单例工厂
class IndexFactory {
   public:
    enum class MetricType { L2, IP };
    enum class IndexType { FLAT, HNSW, UNKNOWN = -1 };

    void Init(IndexType type, int dim, MetricType metric = MetricType::L2, int max_elements = 10000);
    void *GetIndex(IndexType type) const;

    static IndexFactory::IndexType IndexTypeFromString(const std::string &type_str) {
        static const std::unordered_map<std::string, IndexFactory::IndexType> mapping = {
            {"FLAT", IndexFactory::IndexType::FLAT},
            {"HNSW", IndexFactory::IndexType::HNSW},
        };

        auto it = mapping.find(type_str);
        return (it != mapping.end()) ? it->second : IndexFactory::IndexType::UNKNOWN;
    }

   private:
    std::map<IndexType, void *> index_map;
};

IndexFactory *GetGlobalIndexFactory();
