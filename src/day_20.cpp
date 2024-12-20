#include <deque>
#include <fstream>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

using namespace std;

vector<string> map;
int m = 0;
int n = 0;

constexpr int di[] = {-1, 0, 1, 0};
constexpr int dj[] = {0, 1, 0, -1};

struct Coord {
  int i, j;

  bool valid() const { return i >= 0 && i < m && j >= 0 && j < n; }
  Coord move(size_t k) const { return {i + di[k], j + dj[k]}; }
  bool operator==(const Coord& o) const { return i == o.i && j == o.j; }
};

optional<Coord> find(char c) {
  for (int i = 0; i < m; ++i) {
    for (int j = 0; j < n; ++j) {
      if (map[i][j] == c) return Coord{i, j};
    }
  }
  return nullopt;
}

vector<vector<int>> bfs(Coord c0) {
  vector<vector<int>> dist(m, vector<int>(n, -1));
  deque<Coord> queue;
  dist[c0.i][c0.j] = 0;
  queue.push_back(c0);
  while (!queue.empty()) {
    const Coord c0 = queue.front();
    queue.pop_front();
    for (size_t k = 0; k < 4; ++k) {
      const Coord c1 = c0.move(k);
      if (c1.valid() && map[c1.i][c1.j] != '#' && dist[c1.i][c1.j] == -1) {
        dist[c1.i][c1.j] = dist[c0.i][c0.j] + 1;
        queue.push_back(c1);
      }
    }
  }
  return dist;
}

Coord c_start, c_end;
vector<vector<int>> dist_s;
vector<vector<int>> dist_e;
int dist;

int solve(int cheat_len) {
  int count = 0;
  Coord c0, c1;
  for (c0.i = 0; c0.i < m; ++c0.i) {
    for (c0.j = 0; c0.j < n; ++c0.j) {
      if (map[c0.i][c0.j] == '#' || dist_s[c0.i][c0.j] < 0) continue;
      for (c1.i = c0.i - cheat_len; c1.i <= c0.i + cheat_len; ++c1.i) {
        const int rem = cheat_len - abs(c1.i - c0.i);
        for (c1.j = c0.j - rem; c1.j <= c0.j + rem; ++c1.j) {
          if (c1.valid() && map[c1.i][c1.j] != '#' && dist_e[c1.i][c1.j] >= 0 &&
              dist >= dist_s[c0.i][c0.j] + dist_e[c1.i][c1.j] +
                          abs(c1.i - c0.i) + abs(c1.j - c0.j) + 100)
            ++count;
        }
      }
    }
  }
  return count;
}

int main() {
  ifstream input_file("../data/day_20_input.txt");
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
  c_start = find('S').value();
  c_end = find('E').value();
  dist_s = bfs(c_start);
  dist_e = bfs(c_end);
  dist = dist_s[c_end.i][c_end.j];

  cout << solve(2) << "\n";
  cout << solve(20) << "\n";

  return 0;
}
