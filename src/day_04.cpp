#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

namespace {
vector<string> lines;
int m = 0;
int n = 0;
}  // namespace

bool verify_1d(int i, int j, int di, int dj) {
  static constexpr string pattern = "XMAS";
  static constexpr size_t p = pattern.size();
  for (size_t k = 0; k < p; ++k) {
    if (i < 0 || i >= m || j < 0 || j >= n || lines[i][j] != pattern[k]) {
      return false;
    }

    i += di;
    j += dj;
  }
  return true;
}

bool verify_2d(int i, int j) {
  if (i < 0 || i >= m - 2 || j < 0 || j >= n - 2 ||
      lines[i + 1][j + 1] != 'A') {
    return false;
  }

  const bool b1 = (lines[i][j] == 'M' && lines[i + 2][j + 2] == 'S') ||
                  (lines[i][j] == 'S' && lines[i + 2][j + 2] == 'M');
  const bool b2 = (lines[i + 2][j] == 'M' && lines[i][j + 2] == 'S') ||
                  (lines[i + 2][j] == 'S' && lines[i][j + 2] == 'M');

  return b1 && b2;
}

int main() {
  ifstream input_file("../data/day_04_input.txt");
  if (!input_file) {
    cerr << "Error opening file.\n";
    return 1;
  }

  string line;
  while (getline(input_file, line)) {
    lines.emplace_back(line);
  }
  input_file.close();

  m = lines.size();
  n = lines[0].size();

  int num_patterns_part1 = 0;
  int num_patterns_part2 = 0;
  for (int i = 0; i < m; ++i) {
    for (int j = 0; j < n; ++j) {
      // Part 1
      for (int di = -1; di <= 1; ++di) {
        for (int dj = -1; dj <= 1; ++dj) {
          if (di == 0 && dj == 0) {
            continue;
          }

          if (verify_1d(i, j, di, dj)) {
            ++num_patterns_part1;
          }
        }
      }

      // Part 2
      if (verify_2d(i, j)) {
        ++num_patterns_part2;
      }
    }
  }

  cout << num_patterns_part1 << "\n";
  cout << num_patterns_part2 << "\n";

  return 0;
}
