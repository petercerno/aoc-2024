#include <fstream>
#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>

using namespace std;
using ULL = unsigned long long;

namespace std {
template <>
struct hash<pair<ULL, int>> {
  size_t operator()(const pair<ULL, int>& p) const {
    size_t h1 = hash<ULL>()(p.first);
    size_t h2 = hash<int>()(p.second);
    return h1 ^ (h2 << 1);
  }
};
}  // namespace std

namespace {
unordered_map<pair<ULL, int>, ULL> cache;

vector<ULL> parse_nums(const string& str) {
  vector<ULL> nums;
  istringstream iss(str);
  ULL num;
  while (iss >> num) {
    nums.push_back(num);
  }
  return nums;
}

int digits(ULL n) {
  int count = 0;
  while (n > 0) {
    n /= 10;
    ++count;
  }
  return count;
}

ULL blink(ULL num, int k) {
  auto it = cache.find({num, k});
  if (it != cache.end()) {
    return it->second;
  }

  ULL num_stones = 0;
  if (k == 0) {
    num_stones = 1;
  } else if (num == 0) {
    num_stones = blink(1, k - 1);
  } else {
    int d = digits(num);
    if (d % 2 == 0) {
      ULL num1 = num;
      ULL num2 = 1;
      d /= 2;
      for (int i = 0; i < d; ++i) {
        num1 /= 10;
        num2 *= 10;
      }
      num2 = num - num1 * num2;
      num_stones = blink(num1, k - 1) + blink(num2, k - 1);
    } else {
      num_stones = blink(2024 * num, k - 1);
    }
  }
  cache[{num, k}] = num_stones;
  return num_stones;
}
}  // namespace

int main() {
  ifstream input_file("../data/day_11_input.txt");
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

  ULL sum_part1 = 0;
  ULL sum_part2 = 0;
  vector<ULL> nums = parse_nums(line);
  for (ULL num : nums) {
    sum_part1 += blink(num, 25);
    sum_part2 += blink(num, 75);
  }

  cout << sum_part1 << "\n";
  cout << sum_part2 << "\n";

  return 0;
}
