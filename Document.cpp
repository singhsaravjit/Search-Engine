#include "Document.h"
#include <algorithm>
#include <sstream>
#include <cctype>

using namespace std;

Document::Document() : id(-1), totalWords(0) {}

Document::Document(int docId, const string& fname, const string& text) 
    : id(docId), filename(fname), content(text), totalWords(0) {
    processContent();
        //Why call processContent() here?
        //Answer: To process the content of the document, such as converting it to lowercase, removing punctuation, and calculating word frequencies.
}

void Document::processContent() {
    // Convert to lowercase
    string processed = content;
    transform(processed.begin(), processed.end(), processed.begin(), ::tolower);
//Why convert to lowercase?
        //Answer: To make the search case-insensitive.

    
    // Remove punctuation and split into words
    string cleanText = "";
    for (char c : processed) {
        if (isalnum(c) || isspace(c)) {
            cleanText += c;
        } else {
            cleanText += ' ';
        }
    }
    //Why replace with space? "word1,word2" becomes "word1 word2" instead of "word1word2".

    content = cleanText;
    calculateWordFrequencies();
}

void Document::calculateWordFrequencies() {
    istringstream iss(content);
    string word;

    wordFrequency.clear();
    totalWords = 0;

    while (iss >> word) {
        if (word.length() > 2) { // Skip very short words
            wordFrequency[word]++;
            totalWords++;
        }
    }
}

//Why skip short words? Words like "a", "is", "of" are common but not meaningful for search.

int Document::getWordFrequency(const string& word) const {
    auto it = wordFrequency.find(word);
    return (it != wordFrequency.end()) ? it->second : 0;
}

vector<string> Document::getUniqueWords() const {
    vector<string> words;
    for (const auto& pair : wordFrequency) {
        words.push_back(pair.first);
    }
    return words;
}