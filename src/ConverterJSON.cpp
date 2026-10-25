#include "ConverterJSON.h"
#include "nlohmann/json.hpp"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cmath>

using json = nlohmann::json;

std::vector<std::string> ConverterJSON::GetTextDocuments() {
    std::ifstream file(config_path);
    if (!file.is_open()) {
        throw ConfigFileMissingException();
    }

    json config_json;
    try {
        file >> config_json;
    } catch (...) {
        file.close();
        throw ConfigFileEmptyException();
    }
    file.close();

    if (!config_json.contains("config") || config_json["config"].is_null() || config_json["config"].empty()) {
        throw ConfigFileEmptyException();
    }

    std::vector<std::string> result;
    if (config_json.contains("files") && config_json["files"].is_array()) {
        for (const auto& file_path_val : config_json["files"]) {
            std::string file_path = file_path_val.get<std::string>();
            std::ifstream doc_file(file_path);
            if (!doc_file.is_open()) {
                std::cerr << "Warning: file not found: " << file_path << std::endl;
                result.push_back("");
            } else {
                std::string line;
                std::string content;
                while (std::getline(doc_file, line)) {
                    content += line + "\n";
                }
                doc_file.close();
                result.push_back(content);
            }
        }
    }

    return result;
}

int ConverterJSON::GetResponsesLimit() {
    std::ifstream file(config_path);
    if (!file.is_open()) {
        return 5;
    }

    json config_json;
    try {
        file >> config_json;
        file.close();
        if (config_json.contains("config") && config_json["config"].contains("max_responses")) {
            return config_json["config"]["max_responses"].get<int>();
        }
    } catch (...) {
        file.close();
        return 5;
    }

    return 5;
}

std::vector<std::string> ConverterJSON::GetRequests() {
    std::ifstream file(requests_path);
    if (!file.is_open()) {
        std::cerr << "Warning: requests file is missing: " << requests_path << std::endl;
        return {};
    }

    json requests_json;
    try {
        file >> requests_json;
        file.close();
    } catch (...) {
        file.close();
        std::cerr << "Warning: requests file is invalid JSON: " << requests_path << std::endl;
        return {};
    }

    std::vector<std::string> requests;
    if (requests_json.contains("requests") && requests_json["requests"].is_array()) {
        for (const auto& req : requests_json["requests"]) {
            requests.push_back(req.get<std::string>());
        }
    }

    return requests;
}

void ConverterJSON::putAnswers(std::vector<std::vector<std::pair<int, float>>> answers) {
    json root;
    json answers_map = json::object();

    for (size_t i = 0; i < answers.size(); ++i) {
        std::ostringstream ss;
        ss << "request" << std::setw(3) << std::setfill('0') << (i + 1);
        std::string req_key = ss.str();

        const auto& resp = answers[i];
        if (resp.empty()) {
            answers_map[req_key]["result"] = "false";
        } else if (resp.size() == 1) {
            double clean_rank = std::round(resp[0].second * 1000.0) / 1000.0;
            answers_map[req_key]["result"] = "true";
            answers_map[req_key]["docid"] = resp[0].first;
            answers_map[req_key]["rank"] = clean_rank;
        } else {
            answers_map[req_key]["result"] = "true";
            json rel_list = json::array();
            for (const auto& item : resp) {
                double clean_rank = std::round(item.second * 1000.0) / 1000.0;
                rel_list.push_back({
                    {"docid", item.first},
                    {"rank", clean_rank}
                });
            }
            answers_map[req_key]["relevance"] = rel_list;
        }
    }

    root["answers"] = answers_map;

    std::ofstream out_file(answers_path);
    if (out_file.is_open()) {
        out_file << std::setw(2) << root << std::endl;
        out_file.close();
    } else {
        std::cerr << "Error: could not write to " << answers_path << std::endl;
    }
}

std::string ConverterJSON::GetProgramName() {
    std::ifstream file(config_path);
    if (!file.is_open()) return "SearchServer";
    try {
        json j;
        file >> j;
        file.close();
        if (j.contains("config") && j["config"].contains("name")) {
            return j["config"]["name"].get<std::string>();
        }
    } catch (...) {
        file.close();
    }
    return "SearchServer";
}

std::string ConverterJSON::GetProgramVersion() {
    std::ifstream file(config_path);
    if (!file.is_open()) return "0.1";
    try {
        json j;
        file >> j;
        file.close();
        if (j.contains("config") && j["config"].contains("version")) {
            return j["config"]["version"].get<std::string>();
        }
    } catch (...) {
        file.close();
    }
    return "0.1";
}
