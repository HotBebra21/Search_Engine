#include "InvertedIndex.h"
#include <sstream>
#include <cctype>
#include <thread>
#include <algorithm>

void InvertedIndex::UpdateDocumentBase(const std::vector<std::string>& input_docs) {
    if (input_docs.empty()) {
        std::lock_guard<std::mutex> lock(dict_mutex);
        docs.clear();
        freq_dictionary.clear();
        return;
    }

    std::vector<std::map<std::string, size_t>> doc_word_counts(input_docs.size());
    std::vector<std::thread> threads;
    threads.reserve(input_docs.size());

    for (size_t i = 0; i < input_docs.size(); ++i) {
        threads.emplace_back([i, &input_docs, &doc_word_counts]() {
            std::stringstream ss(input_docs[i]);
            std::string word;
            while (ss >> word) {
                for (char& c : word) {
                    c = std::tolower(c);
                }
                doc_word_counts[i][word]++;
            }
        });
    }

    for (auto& th : threads) {
        if (th.joinable()) {
            th.join();
        }
    }

    std::lock_guard<std::mutex> lock(dict_mutex);
    docs = input_docs;
    freq_dictionary.clear();

    for (size_t i = 0; i < doc_word_counts.size(); ++i) {
        for (const auto& p : doc_word_counts[i]) {
            freq_dictionary[p.first].push_back(Entry{i, p.second});
        }
    }
}

std::vector<Entry> InvertedIndex::GetWordCount(const std::string& word) {
    std::lock_guard<std::mutex> lock(dict_mutex);

    std::string lower_word = word;
    for (char& c : lower_word) {
        c = std::tolower(c);
    }

    auto it = freq_dictionary.find(lower_word);
    if (it != freq_dictionary.end()) {
        return it->second;
    }
    return {};
}
