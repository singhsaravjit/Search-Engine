#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <map>
#include <string>
#include <vector>

using namespace std;

class Document {
private:
  int id;
  string filename;
  string content;
  map<string, int> wordFrequency;
  int totalWords;

  // Why are these private?
  // Encapsulation - only Document class can modify these variables

public:
  Document();
  Document(int docId, const string &fname, const string &text);
  // Why two constructors?
  // Create empty document or initialize with data.

  // Getters
  int getId() const { return id; }
  string getFilename() const { return filename; }
  string getContent() const { return content; }
  int getWordFrequency(const string &word) const;
  int getTotalWords() const { return totalWords; }
  // Why const?
  // These methods do not modify the object, so they should be marked as const.

  // Processing
  void processContent();
  void calculateWordFrequencies();

  // Utility
  vector<string> getUniqueWords() const;
};

#endif