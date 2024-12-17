#include <fstream>
#include <functional>
#include <iostream>
#include <optional>
#include <queue>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

using namespace std;

vector<string> map;
int m = 0;
int n = 0;

constexpr int di[] = {-1, 0, 1, 0};
constexpr int dj[] = {0, 1, 0, -1};

struct Coord {
  int i, j;  // Position
  size_t k;  // Orientation

  bool valid() const { return i >= 0 && i < m && j >= 0 && j < n; }
  Coord move() const { return {i + di[k], j + dj[k], k}; }
  Coord rotate(size_t dk) const { return {i, j, (k + dk) % 4}; }
  bool operator==(const Coord& o) const {
    return i == o.i && j == o.j && k == o.k;
  }
  bool operator<(const Coord& o) const {
    return make_tuple(i, j, k) < make_tuple(o.i, o.j, o.k);
  }
};

namespace std {
template <>
struct hash<Coord> {
  size_t operator()(const Coord& coord) const {
    return (hash<int>()(coord.i)) ^ (hash<int>()(coord.j) << 1) ^
           (hash<size_t>()(coord.k) << 2);
  }
};
}  // namespace std

Coord c_start, c_end;

optional<Coord> find(char c) {
  for (int i = 0; i < m; ++i) {
    for (int j = 0; j < n; ++j) {
      if (map[i][j] == c) {
        return Coord{i, j, 1};
      }
    }
  }
  return nullopt;
}

pair<int, int> dijkstra() {
  // Part 1
  int best_d = -1;
  unordered_map<Coord, int> dist;
  unordered_map<Coord, vector<Coord>> parents;
  using DistToCoord = pair<int, Coord>;
  priority_queue<DistToCoord, vector<DistToCoord>, greater<DistToCoord>> queue;
  dist[c_start] = 0;
  queue.push({0, c_start});
  while (!queue.empty()) {
    const Coord c0 = queue.top().second;
    const int d_c0 = queue.top().first;
    queue.pop();
    if (d_c0 > dist[c0]) continue;  // Deprecated queue entry.
    if (c0.i == c_end.i && c0.j == c_end.j && best_d == -1) best_d = d_c0;
    if (best_d > 0 && d_c0 > best_d) break;  // No longer on the shortest path.
    vector<DistToCoord> adj;
    adj.reserve(4);
    const Coord c1 = c0.move();
    if (c1.valid() && map[c1.i][c1.j] != '#') adj.push_back({1, c1});
    adj.push_back({1000, c0.rotate(1)});
    adj.push_back({1000, c0.rotate(3)});
    for (const auto& [cost, c1] : adj) {
      const int d_c1 = d_c0 + cost;
      const auto it = dist.find(c1);
      if (it == dist.end() || d_c1 < it->second) {
        dist[c1] = d_c1;
        queue.push({d_c1, c1});
      }
      if (d_c1 == dist[c1]) parents[c1].push_back(c0);
    }
  }

  // Part 2
  unordered_set<Coord> visited;
  unordered_set<Coord> best;  // Ignores orientation.
  vector<Coord> stack;
  for (size_t k = 0; k < 4; ++k) {
    Coord c0{c_end.i, c_end.j, k};
    if (dist[c0] == best_d) {
      stack.push_back(c0);
      visited.insert(c0);
    }
  }
  while (!stack.empty()) {
    Coord c0 = stack.back();
    stack.pop_back();
    best.insert({c0.i, c0.j, 0});
    for (Coord c1 : parents[c0]) {
      if (visited.insert(c1).second) stack.push_back(c1);
    }
  }
  return {best_d, best.size()};
}

int main() {
  ifstream input_file("../data/day_16_input.txt");
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
  const auto [part1, part2] = dijkstra();

  cout << part1 << "\n";
  cout << part2 << "\n";

  return 0;
}
