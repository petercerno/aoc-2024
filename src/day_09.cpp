#include <cassert>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <string>

using namespace std;

namespace {
map<int, int> parse(const string& line) {
  map<int, int> m;
  int k = 0;
  for (int i = 0; i < line.size(); ++i) {
    m[k] = (i % 2 == 0) ? i / 2 : -1;
    k += line[i] - '0';
  }
  m[k] = -1;
  return m;
}

template <typename U, typename V>
void transfer(map<int, int>& m, U& l1, U& l2, V& r1, V& r2) {
  int left_size = l2->first - l1->first;
  int right_size = r2->first - r1->first;
  if (left_size > right_size) {
    l1->second = r1->second;
    m[l1->first + right_size] = -1;
    --l2;
    r1->second = -1;
  } else if (left_size == right_size) {
    l1->second = r1->second;
    r1->second = -1;
  } else {
    l1->second = r1->second;
    m[r2->first - left_size] = -1;
    ++r2;
  }
}

void compact_part1(map<int, int>& m) {
  auto l1 = m.begin();
  auto l2 = m.begin();
  ++l2;
  auto r1 = m.rbegin();
  auto r2 = m.rbegin();
  ++r1;
  while (true) {
    while (l1->second >= 0) {
      l1 = l2;
      ++l2;
    }
    while (r1->second == -1) {
      r2 = r1;
      ++r1;
    }
    if (l1->first > r1->first) {
      break;
    }

    transfer(m, l1, l2, r1, r2);
  }
}

void compact_part2(map<int, int>& m) {
  // Maps empty interval size to set of starting positions.
  map<int, set<int>> empty_intervals;
  auto l1 = m.begin();
  auto l2 = m.begin();
  ++l2;
  while (l2 != m.end()) {
    if (l1->second == -1) {
      empty_intervals[l2->first - l1->first].insert(l1->first);
    }
    l1 = l2;
    ++l2;
  }

  auto r1 = m.rbegin();
  auto r2 = m.rbegin();
  ++r1;
  while (true) {
    while (r1 != m.rend() && r1->second == -1) {
      r2 = r1;
      ++r1;
    }
    if (r1 == m.rend()) {
      break;
    }

    int right_size = r2->first - r1->first;
    // Find the left-most empty interval of size at least right_size.
    auto best_empty_it = empty_intervals.lower_bound(right_size);
    for (auto it = best_empty_it; it != empty_intervals.end(); ++it) {
      if (*it->second.begin() < *best_empty_it->second.begin()) {
        best_empty_it = it;
      }
    }
    if (best_empty_it != empty_intervals.end()) {
      // Found left-most long enough empty interval.
      int left_size = best_empty_it->first;
      auto first_pos = best_empty_it->second.begin();
      if (*first_pos > r1->first) {
        // Cannot use as it is beyond the right interval (file).
        empty_intervals.erase(best_empty_it);
        continue;
      }

      auto l1 = m.find(*first_pos);  // Guaranteed to exist.
      auto l2 = l1;
      ++l2;
      best_empty_it->second.erase(first_pos);
      if (best_empty_it->second.empty()) {
        empty_intervals.erase(best_empty_it);
      }
      if (left_size > right_size) {
        empty_intervals[left_size - right_size].insert(l1->first + right_size);
      }
      transfer(m, l1, l2, r1, r2);
    }
    r2 = r1;
    ++r1;
  }
}

long long checksum(const map<int, int>& m) {
  long long sum = 0;
  auto l1 = m.begin();
  auto l2 = m.begin();
  ++l2;
  while (l1 != m.end() && l2 != m.end()) {
    if (l1->second >= 0) {
      long long a = l1->first;
      long long b = l2->first - l1->first;
      sum += l1->second * (b * a + b * (b - 1) / 2);
    }
    l1 = l2;
    ++l2;
  }
  return sum;
}
}  // namespace

int main() {
  ifstream input_file("../data/day_09_input.txt");
  if (!input_file) {
    cerr << "Error opening file.\n";
    return 1;
  }

  string line;
  if (!getline(input_file, line)) {
    cerr << "No input.\n";
    return 1;
  }
  input_file.close();

  map<int, int> m1 = parse(line);
  map<int, int> m2 = m1;
  compact_part1(m1);
  compact_part2(m2);

  cout << checksum(m1) << "\n";
  cout << checksum(m2) << "\n";

  return 0;
}
