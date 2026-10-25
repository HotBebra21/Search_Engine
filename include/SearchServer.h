#pragma once

#include <vector>
#include <string>
#include <cmath>
#include "InvertedIndex.h"

struct RelativeIndex {
    size_t doc_id = 0;
    float rank = 0.0f;

    bool operator==(const RelativeIndex& other) const {
        return (doc_id == other.doc_id && std::abs(rank - other.rank) < 1e-4f);
    }
};

class SearchServer {
public:
    explicit SearchServer(InvertedIndex& idx) : _index(idx) {}

    std::vector<std::vector<RelativeIndex>> search(const std::vector<std::string>& queries_input);

    void setMaxResponses(int max_responses) {
        _max_responses = max_responses;
    }

    int getMaxResponses() const {
        return _max_responses;
    }

private:
    InvertedIndex& _index;
    int _max_responses = 5;
};
