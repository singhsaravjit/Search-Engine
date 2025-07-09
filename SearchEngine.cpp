#include "SearchEngine.h"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

using namespace std;

SearchEngine::SearchEngine() {}

// Why return bool? Caller can check if operation succeeded.
bool SearchEngine::addDocument(const string &filename) {
  ifstream file(filename);
  if (!file.is_open()) {
    cerr << "Error: Cannot open file " << filename << endl;
    return false;
  }

  string content;
  string line;
  while (getline(file, line)) {
    content += line + " ";
  }
  // Why add space? Prevent words from different lines joining together.
  file.close();

  if (content.empty()) {
    cerr << "Warning: Empty file " << filename << endl;
    return false;
  }

  // Create document
  int docId = documents.size();
  Document doc(docId, filename, content);
  documents.push_back(doc);
  // Why size as ID? Creates sequential IDs: 0, 1, 2, 3...

  // Update inverted index
  vector<string> words = doc.getUniqueWords();
  for (const string &word : words) {
    invertedIndex[word].insert(
        docId); // Add this document to word's posting list
    if (invertedIndex[word].size() == 1) {
      documentFrequency[word] = 1;
    } else {
      documentFrequency[word] =
          invertedIndex[word].size(); // Count docs containing word
    }
  }

  // Why update both? Inverted index for fast search, document frequency for IDF
  // calculation.

  cout << "Added document: " << filename << " (ID: " << docId << ")" << endl;
  return true;
}

vector<string> SearchEngine::tokenize(const string &text) {
  vector<string> tokens;
  string lowerText = toLowerCase(text);
  istringstream iss(lowerText);
  string word;

  while (iss >> word) {
    // Remove punctuation
    string cleanWord = "";
    for (char c : word) {
      if (isalnum(c)) {
        cleanWord += c;
      }
    }

    if (cleanWord.length() > 2) {
      tokens.push_back(cleanWord);
    }
  }

  return tokens;
}

string SearchEngine::toLowerCase(const string &text) {
  string result = text;
  transform(result.begin(), result.end(), result.begin(), ::tolower);
  return result;
}

set<int>
SearchEngine::findCandidateDocuments(const vector<string> &queryTerms) {
  set<int> candidates;

  if (queryTerms.empty())
    return candidates;

  // Start with documents containing the first term
  auto it = invertedIndex.find(queryTerms[0]);
  if (it != invertedIndex.end()) {
    candidates = it->second;
  }


    // hello ---[1,2,3]
    // world  ---[1,2,5]
    // candidates = [1,2,3,5]
    
  // For additional terms, find union (OR operation)
  for (size_t i = 1; i < queryTerms.size(); i++) {
    auto termIt = invertedIndex.find(queryTerms[i]);
    if (termIt != invertedIndex.end()) {
      set<int> intersection;
      set_union(candidates.begin(), candidates.end(), termIt->second.begin(),
                termIt->second.end(),
                inserter(intersection, intersection.begin()));
      candidates = intersection;
    }
  }

  return candidates;
}

double SearchEngine::calculateTF(const string &word, const Document &doc) {
  int wordCount = doc.getWordFrequency(word);
  int totalWords = doc.getTotalWords();

  if (totalWords == 0)
    return 0.0;

  // Using log normalization: 1 + log(tf)
  return wordCount > 0 ? 1.0 + log(static_cast<double>(wordCount)) : 0.0;
}

double SearchEngine::calculateIDF(const string &word) {
  int totalDocs = documents.size();
  int docsWithWord = documentFrequency[word];

  if (docsWithWord == 0)
    return 0.0;

  return log(static_cast<double>(totalDocs) /
             static_cast<double>(docsWithWord));
  // Why this formula? Rare words (appearing in few documents) get higher
  // scores.
}

double SearchEngine::calculateTFIDF(const string &word, const Document &doc) {
  return calculateTF(word, doc) * calculateIDF(word);
}

string SearchEngine::generateSnippet(const Document &doc,
                                     const vector<string> &queryTerms) {
  string content = doc.getContent();
  if (content.length() <= 100)
    return content;

  // Find first occurrence of any query term
  size_t pos = string::npos;
  for (const string &term : queryTerms) {
    size_t termPos = content.find(term);
    if (termPos != string::npos) {
      if (pos == string::npos || termPos < pos) {
        pos = termPos;
      }
    }
  }
  // Extract snippet around the term
  size_t start = (pos >= 50) ? pos - 50 : 0;
  size_t length = min(static_cast<size_t>(100), content.length() - start);

  return content.substr(start, length) + "...";
}

vector<SearchResult> SearchEngine::search(const string &query, int maxResults) {
  vector<SearchResult> results;

  if (query.empty()) {
    cout << "Empty query!" << endl;
    return results;
  }

  cout << "\n=== Searching for: \"" << query << "\" ===" << endl;

  // Tokenize query
  vector<string> queryTerms = tokenize(query);
  if (queryTerms.empty()) {
    cout << "No valid search terms found!" << endl;
    return results;
  }

  cout << "Query terms: ";
  for (const string &term : queryTerms) {
    cout << "'" << term << "' ";
  }
  cout << endl;

  // Find candidate documents
  set<int> candidates = findCandidateDocuments(queryTerms);

  if (candidates.empty()) {
    cout << "No documents found containing the search terms." << endl;
    return results;
  }

  cout << "Found " << candidates.size() << " candidate document(s)" << endl;

  // Calculate scores for candidates
  vector<pair<int, double>> scoredDocs;

  for (int docId : candidates) {
    double totalScore = 0.0;

    for (const string &term : queryTerms) {
      totalScore += calculateTFIDF(term, documents[docId]);
    }

    scoredDocs.push_back({docId, totalScore});
  }

  // Sort by score (descending)
  sort(scoredDocs.begin(), scoredDocs.end(),
       [](const pair<int, double> &a, const pair<int, double> &b) {
         return a.second > b.second;
       });

  // Create results
  int resultCount = min(maxResults, static_cast<int>(scoredDocs.size()));
  for (int i = 0; i < resultCount; i++) {
    int docId = scoredDocs[i].first;
    double score = scoredDocs[i].second;

    SearchResult result;
    result.documentId = docId;
    result.filename = documents[docId].getFilename();
    result.score = score;
    result.snippet = generateSnippet(documents[docId], queryTerms);

    results.push_back(result);
  }

  // Display results
  cout << "\n--- Search Results ---" << endl;
  for (size_t i = 0; i < results.size(); i++) {
    cout << (i + 1) << ". " << results[i].filename << " (Score: " << fixed
         << setprecision(4) << results[i].score << ")" << endl;
    cout << "   " << results[i].snippet << endl << endl;
  }

  return results;
}

void SearchEngine::printStatistics() {
  cout << "\n=== Search Engine Statistics ===" << endl;
  cout << "Total Documents: " << documents.size() << endl;
  cout << "Vocabulary Size: " << invertedIndex.size() << endl;

  if (!documents.empty()) {
    int totalWords = 0;
    for (const Document &doc : documents) {
      totalWords += doc.getTotalWords();
    }
    cout << "Total Words: " << totalWords << endl;
    cout << "Average Words per Document: " << (totalWords / documents.size())
         << endl;
  }
}