#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

namespace {
vector<string> map;
int m = 0;
int n = 0;

struct Coord {
  int i, j;

  bool valid() const { return i >= 0 && i < m && j >= 0 && j < n; }
};

unordered_map<char, vector<Coord>> antennas;

int gcd(int a, int b) {
  while (b != 0) {
    int c = b;
    b = a % b;
    a = c;
  }
  return a;
}

vector<Coord> get_antinodes(Coord c1, Coord c2, bool simple) {
  if (simple) {
    return {Coord{2 * c2.i - c1.i, 2 * c2.j - c1.j},
            Coord{2 * c1.i - c2.i, 2 * c1.j - c2.j}};
  }

  vector<Coord> antinodes;
  int d = gcd(abs(c2.i - c1.i), abs(c2.j - c1.j));
  int di = (c2.i - c1.i) / d;
  int dj = (c2.j - c1.j) / d;
  for (int k = -1; k <= 1; k += 2) {
    Coord a = Coord{c1.i, c1.j};
    while (a.valid()) {
      antinodes.push_back(a);
      a.i += k * di;
      a.j += k * dj;
    }
  }
  return antinodes;
}

int solve(bool simple) {
  int num_antinodes = 0;
  vector<vector<bool>> visited(m, vector<bool>(n, false));
  for (const auto& [_, coords] : antennas) {
    for (size_t i = 0; i < coords.size() - 1; ++i) {
      for (size_t j = i + 1; j < coords.size(); ++j) {
        vector<Coord> antinodes = get_antinodes(coords[i], coords[j], simple);
        for (const Coord& c : antinodes) {
          if (c.valid() && !visited[c.i][c.j]) {
            visited[c.i][c.j] = true;
            ++num_antinodes;
          }
        }
      }
    }
  }
  return num_antinodes;
}
}  // namespace

int main() {
  ifstream input_file("../data/day_08_input.txt");
  if (!input_file) {
    cerr << "Error opening file.\n";
    return 1;
  }

  string line;
  while (getline(input_file, line)) {
    map.emplace_back(line);
  }
  input_file.close();

  m = map.size();
  n = map[0].size();
  for (int i = 0; i < m; ++i) {
    for (int j = 0; j < n; ++j) {
      const char c = map[i][j];
      if (c != '.') antennas[c].push_back(Coord{i, j});
    }
  }

  cout << solve(/*simple=*/true) << "\n";
  cout << solve(/*simple=*/false) << "\n";

  return 0;
}
