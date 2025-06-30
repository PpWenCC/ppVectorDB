#pragma once

#include <vector>
#include "hnswlib/hnswlib.h"
#include "index_factory.h"


class HNSWIndex {
   public:
    HNSWIndex(int dim, int max_elementsm, IndexFactory::MetricType metric,
              int ef_construction = 200, int M = 16);

    // Insert a vector with a user-defined label id
    void InsertVectorsWithUsrId(const std::vector<float>& data, uint64_t label);

    // Search for the top K nearest neighbors of a query vector
    std::pair<std::vector<long>, std::vector<float>> SearchVectorsTopK(
        const std::vector<float>& query, int k, int ef_search = 16);

   private:
    int dim;
    hnswlib::SpaceInterface<float>* space;
    hnswlib::HierarchicalNSW<float>* index;
};
