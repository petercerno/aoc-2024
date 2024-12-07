#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>

using namespace std;

int main() {
  ifstream input_file("../data/day_03_input.txt");
  if (!input_file) {
    cerr << "Error opening file.\n";
    return 1;
  }

  ostringstream buffer;
  buffer << input_file.rdbuf();
  string input = buffer.str();
  input_file.close();

  regex regex_pattern(R"raw((mul\((\d+),(\d+)\)|do\(\)|don't\(\)))raw");
  auto words_begin = sregex_iterator(input.begin(), input.end(), regex_pattern);
  auto words_end = sregex_iterator();

  long long sum_part1 = 0;
  long long sum_part2 = 0;
  bool enabled = true;
  for (sregex_iterator i = words_begin; i != words_end; ++i) {
    smatch match = *i;
    string match_str = match.str();
    if (match_str.starts_with("mul")) {
      long long mul = stoi(match[2]) * stoi(match[3]);
      sum_part1 += mul;
      if (enabled) {
        sum_part2 += mul;
      }
    } else if (match_str == "do()") {
      enabled = true;
    } else if (match_str == "don't()") {
      enabled = false;
    }
  }

  cout << sum_part1 << "\n";
  cout << sum_part2 << "\n";

  return 0;
}
