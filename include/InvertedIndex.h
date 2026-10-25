#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <mutex>

struct Entry {
    size_t doc_id = 0;
    size_t count = 0;

    bool operator==(const Entry& other) const {
        return (doc_id == other.doc_id && count == other.count);
    }
};

class InvertedIndex {
public:
    InvertedIndex() = default;

    void UpdateDocumentBase(const std::vector<std::string>& input_docs);

    std::vector<Entry> GetWordCount(const std::string& word);

    const std::vector<std::string>& GetDocuments() const {
        return docs;
    }

private:
    std::vector<std::string> docs;
    std::map<std::string, std::vector<Entry>> freq_dictionary;
    std::mutex dict_mutex;
};
