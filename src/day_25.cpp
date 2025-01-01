#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

int main() {
  ifstream input_file("../data/day_25_input.txt");
  if (!input_file) {
    cerr << "Error opening file.\n";
    return 1;
  }

  vector<vector<int>> locks;
  vector<vector<int>> keys;
  string line;
  vector<int> heights;
  vector<vector<int>>* container = nullptr;
  bool is_first_line = true;
  while (getline(input_file, line)) {
    if (line.empty()) {
      container->push_back(heights);
      is_first_line = true;
      continue;
    }

    if (is_first_line) {
      if (line[0] == '#') {
        heights.assign(line.size(), 0);
        container = &locks;
      } else {
        heights.assign(line.size(), -1);
        container = &keys;
      }
      is_first_line = false;
      continue;
    }

    for (size_t i = 0; i < line.size(); ++i) {
      if (line[i] == '#') ++heights[i];
    }
  }
  container->push_back(heights);
  input_file.close();

  int num_lock_key_fits = 0;
  for (const vector<int>& lock : locks) {
    for (const vector<int>& key : keys) {
      bool lock_key_fits = true;
      for (size_t i = 0; lock_key_fits && i < lock.size(); ++i) {
        if (lock[i] + key[i] > 5) lock_key_fits = false;
      }
      if (lock_key_fits) ++num_lock_key_fits;
    }
  }

  cout << num_lock_key_fits << "\n";

  return 0;
}
