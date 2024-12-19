#include <cassert>
#include <deque>
#include <fstream>
#include <iostream>
#include <regex>
#include <string>
#include <utility>
#include <vector>

using namespace std;

constexpr int m = 71;
constexpr int n = 71;
constexpr int di[] = {-1, 0, 1, 0};
constexpr int dj[] = {0, 1, 0, -1};

struct Coord {
  int i, j;

  bool valid() const { return i >= 0 && i < m && j >= 0 && j < n; }
  Coord move(size_t k) const { return {i + di[k], j + dj[k]}; }
  bool operator==(const Coord& o) const { return i == o.i && j == o.j; }
};

vector<Coord> bytes;

void solve_part1() {
  vector<vector<int>> map(m, vector<int>(n, 0));
  for (size_t k = 0; k < 1024; ++k) {
    map[bytes[k].i][bytes[k].j] = -1;
  }
  deque<Coord> queue;
  queue.push_back({0, 0});
  map[0][0] = 1;
  while (!queue.empty() && map[m - 1][n - 1] == 0) {
    const Coord c0 = queue.front();
    queue.pop_front();
    for (size_t k = 0; k < 4; ++k) {
      const Coord c1 = c0.move(k);
      if (c1.valid() && map[c1.i][c1.j] == 0) {
        queue.push_back(c1);
        map[c1.i][c1.j] = map[c0.i][c0.j] + 1;
      }
    }
  }
  cout << map[m - 1][n - 1] - 1 << "\n";
}

vector<vector<Coord>> parent(m, vector<Coord>(n, {0, 0}));
vector<vector<int>> rank(m, vector<int>(n, 0));

Coord find_set(Coord c) {
  if (c == parent[c.i][c.j]) return c;
  return parent[c.i][c.j] = find_set(parent[c.i][c.j]);
}

void union_sets(Coord a, Coord b) {
  a = find_set(a);
  b = find_set(b);
  if (a == b) return;
  if (rank[a.i][a.j] < rank[b.i][b.j]) swap(a, b);
  parent[b.i][b.j] = a;
  if (rank[a.i][a.j] == rank[b.i][b.j]) ++rank[a.i][a.j];
}

void connect(const vector<vector<int>>& map, Coord c0) {
  for (size_t k = 0; k < 4; ++k) {
    const Coord c1 = c0.move(k);
    if (c1.valid() && map[c1.i][c1.j] == 0) {
      union_sets(c0, c1);
    }
  }
}

void solve_part2() {
  for (int i = 0; i < m; ++i) {
    for (int j = 0; j < n; ++j) {
      parent[i][j] = {i, j};
    }
  }
  vector<vector<int>> map(m, vector<int>(n, 0));
  for (const Coord c : bytes) ++map[c.i][c.j];
  for (int i = 0; i < m; ++i) {
    for (int j = 0; j < n; ++j) {
      if (map[i][j] == 0) connect(map, {i, j});
    }
  }
  for (auto it = bytes.rbegin(); it != bytes.rend(); ++it) {
    const Coord c = *it;
    --map[it->i][it->j];
    if (map[it->i][it->j] == 0) connect(map, *it);
    if (find_set({0, 0}) == find_set({m - 1, n - 1})) {
      cout << it->i << ',' << it->j << "\n";
      return;
    }
  }
}

int main() {
  ifstream input_file("../data/day_18_input.txt");
  if (!input_file) {
    cerr << "Error opening file.\n";
    return 1;
  }

  string line;
  regex coord_pattern(R"raw((\d+),(\d+))raw");
  while (getline(input_file, line)) {
    smatch match;
    assert(regex_match(line, match, coord_pattern));
    bytes.push_back({stoi(match[1]), stoi(match[2])});
  }
  input_file.close();

  solve_part1();
  solve_part2();

  return 0;
}
