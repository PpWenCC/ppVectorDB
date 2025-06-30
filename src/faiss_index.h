#pragma once

#include <faiss/Index.h>
#include <vector>

class FaissIndex{
   public:
    FaissIndex(faiss::Index *index);

    // insert vector with UsrId
    void InsertVectorsWithUsrId(const std::vector<float> &data, uint64_t label);

    // topK search
    std::pair<std::vector<long>, std::vector<float>> SearchVectorsTopK(
        const std::vector<float> &query, int k);

   private:
    faiss::Index *index;
};