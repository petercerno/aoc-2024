#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

template <typename T>
bool is_safe(T start, T end) {
  if (start == end) {
    return true;
  }

  auto prev = start;
  auto curr = start + 1;
  while (curr != end) {
    if (*curr <= *prev || *curr >= *prev + 4) {
      return false;
    }

    ++prev;
    ++curr;
  }
  return true;
}

template <typename T>
bool is_safe_relaxed(T start, T end) {
  if (start == end) {
    return true;
  }

  auto prev = start;
  auto curr = start + 1;
  while (curr != end) {
    if (*curr <= *prev || *curr >= *prev + 4) {
      // We need to drop prev or curr.
      if (curr + 1 == end) {
        return true;  // We can drop curr as it is the last level.
      }

      auto next = curr + 1;
      if (*next > *prev && *next < *prev + 4 && is_safe(next, end)) {
        return true;  // We can drop curr.
      }

      // We cannot drop curr. Try to drop prev.
      if (!is_safe(curr, end)) {
        return false;  // Dropping prev is not enough.
      }

      // Check if we can drop prev.
      if (prev == start) {
        return true;  // We can drop the first level.
      }

      auto prev_prev = prev - 1;
      return *curr > *prev_prev && *curr < *prev_prev + 4;
    }

    ++prev;
    ++curr;
  }
  return true;
}

int main() {
  ifstream input_file("../data/day_02_input.txt");
  if (!input_file) {
    cerr << "Error opening file.\n";
    return 1;
  }

  int num_safe_part1 = 0;
  int num_safe_part2 = 0;
  string line;
  while (getline(input_file, line)) {
    istringstream iss(line);
    vector<int> levels;
    levels.reserve(16);
    int num;
    while (iss >> num) {
      levels.push_back(num);
    }
    if (is_safe(levels.begin(), levels.end()) ||
        is_safe(levels.rbegin(), levels.rend())) {
      ++num_safe_part1;
    }
    if (is_safe_relaxed(levels.begin(), levels.end()) ||
        is_safe_relaxed(levels.rbegin(), levels.rend())) {
      ++num_safe_part2;
    }
  }
  input_file.close();

  cout << num_safe_part1 << "\n";
  cout << num_safe_part2 << "\n";
  return 0;
}
