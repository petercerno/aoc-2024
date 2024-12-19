#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

struct Node {
  vector<int> next = vector<int>(26, -1);
  bool is_pattern = false;
};

vector<Node> trie(1);

void add_pattern(const string& pattern) {
  int node = 0;
  for (const char ch : pattern) {
    int c = ch - 'a';
    if (trie[node].next[c] == -1) {
      trie[node].next[c] = trie.size();
      trie.emplace_back();
    }
    node = trie[node].next[c];
  }
  trie[node].is_pattern = true;
}

long long count_ways(string::const_iterator begin, string::const_iterator end,
                     vector<long long>& cache) {
  if (begin == end) return 1;
  if (cache[end - begin] >= 0) return cache[end - begin];
  long long count = 0;
  int node = 0;
  auto it = begin;
  while (it != end) {
    node = trie[node].next[*it - 'a'];
    ++it;
    if (node == -1) break;
    if (trie[node].is_pattern) count += count_ways(it, end, cache);
  }
  cache[end - begin] = count;
  return count;
}

vector<string> split(const string& str) {
  vector<string> tokens;
  tokens.emplace_back();
  for (const char ch : str) {
    if (ch == ',') tokens.emplace_back();
    if (ch == ',' || ch == ' ') continue;
    tokens.back().push_back(ch);
  }
  return tokens;
}

int main() {
  ifstream input_file("../data/day_19_input.txt");
  if (!input_file) {
    cerr << "Error opening file.\n";
    return 1;
  }

  string line;
  assert(getline(input_file, line));
  vector<string> patterns = split(line);
  for (const string& pattern : patterns) {
    add_pattern(pattern);
  }
  assert(getline(input_file, line) && line.empty());
  long long count_part1 = 0;
  long long count_part2 = 0;
  while (getline(input_file, line)) {
    vector<long long> cache(line.size() + 1, -1);
    long long count = count_ways(line.begin(), line.end(), cache);
    count_part1 += count > 0 ? 1 : 0;
    count_part2 += count;
  }
  input_file.close();

  cout << count_part1 << "\n";
  cout << count_part2 << "\n";

  return 0;
}
