#pragma once

#include <vector>
#include <string>
#include <exception>

class ConfigFileMissingException : public std::exception {
public:
    const char* what() const noexcept override {
        return "config file is missing";
    }
};

class ConfigFileEmptyException : public std::exception {
public:
    const char* what() const noexcept override {
        return "config file is empty";
    }
};

class ConverterJSON {
public:
    ConverterJSON() = default;

    std::vector<std::string> GetTextDocuments();
    int GetResponsesLimit();
    std::vector<std::string> GetRequests();
    void putAnswers(std::vector<std::vector<std::pair<int, float>>> answers);

    void setConfigPath(const std::string& path) { config_path = path; }
    void setRequestsPath(const std::string& path) { requests_path = path; }
    void setAnswersPath(const std::string& path) { answers_path = path; }

    std::string GetProgramName();
    std::string GetProgramVersion();

private:
    std::string config_path = "config.json";
    std::string requests_path = "requests.json";
    std::string answers_path = "answers.json";
};
