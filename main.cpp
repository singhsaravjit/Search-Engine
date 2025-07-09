#include "SearchEngine.h"
#include <iostream>

using namespace std;

int main() {
  cout << "=== Simple Search Engine Demo ===" << endl;

  SearchEngine engine;

  // Add documents
  cout << "\n--- Loading Documents ---" << endl;
  engine.addDocument("sample_docs/doc1.txt");
  engine.addDocument("sample_docs/doc2.txt");
  engine.addDocument("sample_docs/doc3.txt");
  engine.addDocument("sample_docs/doc4.txt");
  engine.addDocument("sample_docs/doc5.txt");

  // Print statistics
  engine.printStatistics();

  // Perform searches
  engine.search("quick brown");
  engine.search("algorithm");
  engine.search("forest animals");
  engine.search("information");

  // Interactive search
  cout << "\n--- Interactive Search ---" << endl;
  cout << "Enter search queries (type 'quit' to exit):" << endl;

  string query;
  while (true) {
    cout << "\nSearch> ";
    getline(cin, query);

    if (query == "quit" || query == "exit") {
      break;
    }

    if (!query.empty()) {
      engine.search(query);
    }
  }

  cout << "Thank you for using the Search Engine!" << endl;
  return 0;
}