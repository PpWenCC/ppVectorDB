#include "faiss_index.h"
#include <faiss/Index.h>
#include <vector>
#include "logger.h"

FaissIndex::FaissIndex(faiss::Index *index) : index(index){};

void FaissIndex::InsertVectorsWithUsrId(const std::vector<float> &data, uint64_t label) {
    long id = static_cast<long>(label);
    index->add_with_ids(1, data.data(), &id);
}

std::pair<std::vector<long>, std::vector<float>> FaissIndex::SearchVectorsTopK(
    const std::vector<float> &query, int k) {
    int dim = index->d;
    int num_queries = query.size() / dim;
    std::vector<long> resId(num_queries * k);
    std::vector<float> distances(num_queries * k);

    index->search(num_queries, query.data(), k, distances.data(), resId.data());

    GlobalLogger->debug("Retrieved values:");
    for (size_t i = 0; i < resId.size(); ++i) {
        if (resId[i] != -1) {
            GlobalLogger->debug("ID: {}, Distance: {}", resId[i], distances[i]);
        }
    }
    return {resId, distances};
}
