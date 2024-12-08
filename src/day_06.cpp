#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

namespace {
vector<string> map;
int m = 0;
int n = 0;

struct Guard {
  int i, j;
  char d;

  static uint8_t dtype(char d) {
    switch (d) {
      case '^':
        return 1;
      case 'v':
        return 2;
      case '<':
        return 4;
      case '>':
        return 8;
    }
    return 0;
  }

  uint8_t dtype() const { return dtype(d); }

  static Guard find() {
    for (int i = 0; i < m; ++i) {
      for (int j = 0; j < n; ++j) {
        if (dtype(map[i][j]) != 0) {
          return Guard{i, j, map[i][j]};
        }
      }
    }
    return Guard{-1, -1, '?'};
  }

  Guard move() const {
    switch (d) {
      case '^':
        return Guard{i - 1, j, d};
      case 'v':
        return Guard{i + 1, j, d};
      case '<':
        return Guard{i, j - 1, d};
      case '>':
        return Guard{i, j + 1, d};
    }
    return Guard{-1, -1, '?'};
  }

  Guard turn() const {
    switch (d) {
      case '^':
        return Guard{i, j, '>'};
      case 'v':
        return Guard{i, j, '<'};
      case '<':
        return Guard{i, j, '^'};
      case '>':
        return Guard{i, j, 'v'};
    }
    return Guard{-1, -1, '?'};
  }

  bool valid() const { return i >= 0 && i < m && j >= 0 && j < n; }
  bool blocked() const { return valid() && map[i][j] == '#'; }
};

Guard s;

int solve_part1(bool mark) {
  vector<vector<uint8_t>> visited(m, vector<uint8_t>(n, 0));
  int num_positions = 1;
  Guard g = s;
  map[g.i][g.j] = 'S';  // Starting position.
  while (g.valid()) {
    if ((g.dtype() & visited[g.i][g.j]) != 0) {
      return -1;  // Stuck in a loop.
    } else {
      visited[g.i][g.j] |= g.dtype();
    }

    if (mark && map[g.i][g.j] != 'S' && map[g.i][g.j] != 'X') {
      map[g.i][g.j] = 'X';
      ++num_positions;
    }

    Guard ng = g.move();
    if (!ng.valid()) {
      return num_positions;  // Left the area.
    }

    if (ng.blocked()) {
      g = g.turn();
    } else {
      g = ng;
    }
  }
  return 0;
}

int solve_part2() {
  int num_positions = 0;
  for (int i = 0; i < m; ++i) {
    for (int j = 0; j < n; ++j) {
      if (map[i][j] != 'X') {
        continue;  // Assumes we solved part 1.
      }

      map[i][j] = '#';
      if (solve_part1(/*mark=*/false) == -1) {
        ++num_positions;
      }
      map[i][j] = 'X';
    }
  }
  return num_positions;
}
}  // namespace

int main() {
  ifstream input_file("../data/day_06_input.txt");
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
  s = Guard::find();

  cout << solve_part1(/*mark=*/true) << "\n";
  cout << solve_part2() << "\n";

  return 0;
}
