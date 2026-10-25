#include <iostream>
#include <vector>
#include <string>
#include "ConverterJSON.h"
#include "InvertedIndex.h"
#include "SearchServer.h"

using namespace std;

int main() {
    try {
        ConverterJSON converter;

        cout << "Starting " << converter.GetProgramName()
             << " (version " << converter.GetProgramVersion() << ")" << endl;

        vector<string> docs;
        try {
            docs = converter.GetTextDocuments();
        } catch (const ConfigFileMissingException& e) {
            cerr << e.what() << endl;
            return 1;
        } catch (const ConfigFileEmptyException& e) {
            cerr << e.what() << endl;
            return 1;
        } catch (const exception& e) {
            cerr << "Error: " << e.what() << endl;
            return 1;
        }

        cout << "Loaded " << docs.size() << " documents for indexing." << endl;

        InvertedIndex index;
        index.UpdateDocumentBase(docs);
        cout << "Inverted index built successfully." << endl;

        auto requests = converter.GetRequests();
        cout << "Loaded " << requests.size() << " search requests." << endl;

        SearchServer server(index);
        server.setMaxResponses(converter.GetResponsesLimit());

        auto answers = server.search(requests);

        vector<vector<pair<int, float>>> answers_pairs;
        for (const auto& query_answers : answers) {
            vector<pair<int, float>> pairs;
            for (const auto& item : query_answers) {
                pairs.push_back({(int)item.doc_id, item.rank});
            }
            answers_pairs.push_back(pairs);
        }

        converter.putAnswers(answers_pairs);

        cout << "Search finished. Results saved to answers.json." << endl;

    } catch (const exception& e) {
        cerr << "Fatal error: " << e.what() << endl;
        return 1;
    }

    return 0;
}
