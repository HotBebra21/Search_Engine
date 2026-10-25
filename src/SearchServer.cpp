#include "SearchServer.h"
#include <sstream>
#include <set>
#include <algorithm>
#include <cctype>

std::vector<std::vector<RelativeIndex>> SearchServer::search(const std::vector<std::string>& queries_input) {
    std::vector<std::vector<RelativeIndex>> result;
    result.reserve(queries_input.size());

    for (const auto& query : queries_input) {
        std::set<std::string> words;
        std::stringstream ss(query);
        std::string word;
        while (ss >> word) {
            for (char& c : word) {
                c = std::tolower(c);
            }
            words.insert(word);
        }

        if (words.empty()) {
            result.push_back({});
            continue;
        }

        std::vector<std::pair<std::string, size_t>> word_freqs;
        word_freqs.reserve(words.size());

        for (const auto& w : words) {
            auto entries = _index.GetWordCount(w);
            size_t count = 0;
            for (const auto& entry : entries) {
                count += entry.count;
            }
            word_freqs.push_back({w, count});
        }

        std::sort(word_freqs.begin(), word_freqs.end(), [](const auto& a, const auto& b) {
            return a.second < b.second;
        });

        std::map<size_t, size_t> doc_relevance;
        for (const auto& pair : word_freqs) {
            if (pair.second == 0) {
                continue;
            }

            auto entries = _index.GetWordCount(pair.first);
            for (const auto& entry : entries) {
                doc_relevance[entry.doc_id] += entry.count;
            }
        }

        if (doc_relevance.empty()) {
            result.push_back({});
            continue;
        }

        size_t max_rel = 0;
        for (const auto& pair : doc_relevance) {
            if (pair.second > max_rel) {
                max_rel = pair.second;
            }
        }

        if (max_rel == 0) {
            result.push_back({});
            continue;
        }

        std::vector<RelativeIndex> rel_list;
        rel_list.reserve(doc_relevance.size());

        for (const auto& pair : doc_relevance) {
            float rank = (float)pair.second / (float)max_rel;
            rel_list.push_back(RelativeIndex{pair.first, rank});
        }

        std::sort(rel_list.begin(), rel_list.end(), [](const RelativeIndex& a, const RelativeIndex& b) {
            if (std::abs(a.rank - b.rank) > 1e-5f) {
                return a.rank > b.rank;
            }
            return a.doc_id < b.doc_id;
        });

        if (rel_list.size() > (size_t)_max_responses) {
            rel_list.resize(_max_responses);
        }

        result.push_back(rel_list);
    }

    return result;
}
