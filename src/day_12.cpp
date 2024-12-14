#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

using namespace std;

namespace {
vector<string> map;
int m = 0;
int n = 0;

struct Coord {
  int i, j;

  bool operator==(const Coord& o) const { return i == o.i && j == o.j; }
};

struct Solution {
  vector<vector<Coord>> parent;
  vector<vector<int>> area;
  vector<vector<int>> perimeter;

  Coord find(Coord c) {
    if (c == parent[c.i][c.j]) return c;  // Representative node
    return parent[c.i][c.j] = find(parent[c.i][c.j]);
  }

  void maybe_union(Coord a, Coord b) {
    if (map[a.i][a.j] != map[b.i][b.j]) return;
    a = find(a);
    b = find(b);
    perimeter[a.i][a.j] -= 2;
    if (a == b) return;
    if (area[a.i][a.j] < area[b.i][b.j]) swap(a, b);
    parent[b.i][b.j] = a;
    area[a.i][a.j] += area[b.i][b.j];
    perimeter[a.i][a.j] += perimeter[b.i][b.j];
  }

  long long sum() {
    long long sum = 0;
    for (int i = 0; i < m; ++i) {
      for (int j = 0; j < n; ++j) {
        if (parent[i][j] == Coord{i, j}) {
          sum += area[i][j] * perimeter[i][j];
        }
      }
    }
    return sum;
  }

  long long solve_part1() {
    parent.resize(m, vector<Coord>(n, {0, 0}));
    area.resize(m, vector(n, 1));
    perimeter.resize(m, vector(n, 4));
    for (int i = 0; i < m; ++i) {
      for (int j = 0; j < n; ++j) {
        parent[i][j] = {i, j};
      }
    }
    for (int i = 0; i < m; ++i) {
      for (int j = 0; j < n; ++j) {
        if (j < n - 1) maybe_union({i, j}, {i, j + 1});
        if (i < m - 1) maybe_union({i, j}, {i + 1, j});
      }
    }
    return sum();
  }

  void update_perim(Coord c0, Coord c1) {
    c0 = find(c0);
    c1 = find(c1);
    if (c0 == c1) --perimeter[c0.i][c0.j];
  }

  void update_perim(Coord c0, Coord c1, Coord c2, Coord c3) {
    c0 = find(c0);
    c1 = find(c1);
    c2 = find(c2);
    c3 = find(c3);
    if (c0 == c1 && c0 != c2 && c1 != c3) --perimeter[c0.i][c0.j];
  }

  long long solve_part2() {
    for (int j = 0; j < n - 1; ++j) {
      update_perim({0, j}, {0, j + 1});
      update_perim({m - 1, j}, {m - 1, j + 1});
    }
    for (int i = 0; i < m - 1; ++i) {
      update_perim({i, 0}, {i + 1, 0});
      update_perim({i, n - 1}, {i + 1, n - 1});
    }
    for (int i = 0; i < m - 1; ++i) {
      for (int j = 0; j < n - 1; ++j) {
        update_perim({i, j}, {i, j + 1}, {i + 1, j}, {i + 1, j + 1});
        update_perim({i + 1, j}, {i + 1, j + 1}, {i, j}, {i, j + 1});
        update_perim({i, j}, {i + 1, j}, {i, j + 1}, {i + 1, j + 1});
        update_perim({i, j + 1}, {i + 1, j + 1}, {i, j}, {i + 1, j});
      }
    }
    return sum();
  }
};
}  // namespace

int main() {
  ifstream input_file("../data/day_12_input.txt");
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

  Solution solution;
  cout << solution.solve_part1() << "\n";
  cout << solution.solve_part2() << "\n";

  return 0;
}
