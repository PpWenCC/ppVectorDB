#include "HNSW_index.h"


HNSWIndex::HNSWIndex(int dim, int max_elements, IndexFactory::MetricType metric,
                     int ef_construction, int M)
    : dim(dim) {
    if (metric == IndexFactory::MetricType::L2) {
        space = new hnswlib::L2Space(dim);
    } else {
        throw std::runtime_error("Invalid metric type.");
    }

    index = new hnswlib::HierarchicalNSW<float>(space, max_elements, ef_construction, M, 100, true);
}

void HNSWIndex::InsertVectorsWithUsrId(const std::vector<float>& data, uint64_t label) {
    index->addPoint(data.data(), label, true);
}

std::pair<std::vector<long>, std::vector<float>> HNSWIndex::SearchVectorsTopK(
    const std::vector<float>& query, int k, int ef_search) {
    index->setEf(ef_search);
    auto result = index->searchKnn(query.data(), k);

    std::vector<long> indices(k);
    std::vector<float> distances(k);
    for (int j = 0; j < k; j++) {
        auto item = result.top();
        indices[j] = item.second;
        distances[j] = item.first;
        result.pop();
    }

    return {indices, distances};
}
