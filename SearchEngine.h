#ifndef SEARCHENGINE_H
#define SEARCHENGINE_H

#include "Document.h"
#include <vector>
#include <map>
#include <set>
#include <string>

using namespace std;

struct SearchResult {
    int documentId;
    string filename;
    double score;
    string snippet;
};
//Why struct? Simple data container for returning search results.

class SearchEngine {
private:
    vector<Document> documents;
    map<string, set<int>> invertedIndex;
    map<string, int> documentFrequency;

//vector<Document>: Sequential access to documents by ID
//map<string, set<int>>: Fast lookup of which documents contain a word
//map<string, int>: Quick count of documents containing each word

    // Helper functions
    vector<string> tokenize(const string& text);
    string toLowerCase(const string& text);
    set<int> findCandidateDocuments(const vector<string>& queryTerms);
    double calculateTF(const string& word, const Document& doc);
    double calculateIDF(const string& word);
    double calculateTFIDF(const string& word, const Document& doc);
    string generateSnippet(const Document& doc, const vector<string>& queryTerms);

public:
    SearchEngine();

    // Core functions
    bool addDocument(const string& filename);
    vector<SearchResult> search(const string& query, int maxResults = 10);

    // Utility functions
    void printStatistics();
    int getDocumentCount() const { return documents.size(); }
    int getVocabularySize() const { return invertedIndex.size(); }
};

#endif