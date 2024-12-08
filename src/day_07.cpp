#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

namespace {
using ULL = unsigned long long;

ULL concat(ULL num1, ULL num2) {
  ULL num0 = num2;
  while (num0 > 0) {
    num1 *= 10;
    num0 /= 10;
  }
  return num1 + num2;
}

struct Equation {
  ULL test;
  vector<ULL> nums;

  void parse_nums(const string& str) {
    istringstream iss(str);
    ULL num;
    while (iss >> num) {
      nums.push_back(num);
    }
  }

  bool check(bool extended) const { return _check(nums[0], 1, extended); }

 private:
  bool _check(ULL acc, size_t i, bool extended) const {
    if (i == nums.size()) {
      return acc == test;
    }

    if (acc > test) {
      return false;
    }

    return _check(acc + nums[i], i + 1, extended) ||
           _check(acc * nums[i], i + 1, extended) ||
           (extended && _check(concat(acc, nums[i]), i + 1, extended));
  }
};
}  // namespace

int main() {
  ifstream input_file("../data/day_07_input.txt");
  if (!input_file) {
    cerr << "Error opening file.\n";
    return 1;
  }

  ULL sum_part1 = 0;
  ULL sum_part2 = 0;
  string line;
  regex line_pattern(R"raw(^(\d+): (.*)$)raw");
  while (getline(input_file, line)) {
    smatch match;
    if (regex_match(line, match, line_pattern)) {
      Equation eq;
      eq.test = stoull(match[1]);
      eq.parse_nums(match[2]);
      if (eq.check(/*extended=*/false)) {
        sum_part1 += eq.test;
      }
      if (eq.check(/*extended=*/true)) {
        sum_part2 += eq.test;
      }
    }
  }
  input_file.close();

  cout << sum_part1 << "\n";
  cout << sum_part2 << "\n";

  return 0;
}
