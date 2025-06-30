#include "index_factory.h"
#include <faiss/IndexFlat.h>
#include <faiss/IndexIDMap.h>
#include "HNSW_index.h"
#include "faiss_index.h"

// 屏蔽外界直接访问factory
namespace {
IndexFactory globalIndexFactory;
}

IndexFactory *GetGlobalIndexFactory() {
    return &globalIndexFactory;
}

void IndexFactory::Init(IndexType type, int dim, MetricType metric, int max_elements) {
    switch (type) {
        case IndexType::FLAT: {
            faiss::MetricType faiss_metric = (metric == MetricType::L2)
                                                 ? faiss::METRIC_L2
                                                 : faiss::METRIC_INNER_PRODUCT;
            index_map[type] = new FaissIndex(
                new faiss::IndexIDMap(new faiss::IndexFlat(dim, faiss_metric)));
            break;
        }
        case IndexType::HNSW:
            index_map[type] = new HNSWIndex(dim, max_elements, metric, 200, 16);
            break;
        default:
            break;
    }
}

void *IndexFactory::GetIndex(IndexType type) const {
    auto it = index_map.find(type);
    if (it != index_map.end()) {
        return it->second;
    }
    return nullptr;
}