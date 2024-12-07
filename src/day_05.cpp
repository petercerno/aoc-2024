#include <algorithm>
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>

using namespace std;

namespace {
unordered_map<int, unordered_set<int>> rules;
}  // namespace

std::vector<int> split(const std::string& str) {
  vector<int> result;
  result.reserve(32);

  istringstream stream(str);
  string token;
  while (getline(stream, token, ',')) {
    result.push_back(stoi(token));
  }

  return result;
}

bool check(const vector<int>& updates) {
  for (size_t i = 0; i < updates.size() - 1; ++i) {
    if (!rules[updates[i]].contains(updates[i + 1])) {
      return false;
    }
  }

  return true;
}

int main() {
  ifstream input_file("../data/day_05_input.txt");
  if (!input_file) {
    cerr << "Error opening file.\n";
    return 1;
  }

  string line;
  regex rule_pattern(R"raw((\d+)\|(\d+))raw");
  while (getline(input_file, line)) {
    smatch match;
    if (regex_match(line, match, rule_pattern)) {
      rules[stoi(match[1])].insert(stoi(match[2]));
    } else {
      break;
    }
  }

  long long sum_part1 = 0;
  long long sum_part2 = 0;
  while (getline(input_file, line)) {
    vector<int> updates = split(line);
    if (check(updates)) {
      sum_part1 += updates[(updates.size() - 1) / 2];
    } else {
      sort(updates.begin(), updates.end(),
           [](int a, int b) { return rules[a].contains(b); });
      sum_part2 += updates[(updates.size() - 1) / 2];
    }
  }
  input_file.close();

  cout << sum_part1 << "\n";
  cout << sum_part2 << "\n";

  return 0;
}
