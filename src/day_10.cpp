#include <fstream>
#include <functional>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

namespace {
vector<string> map;
int m = 0;
int n = 0;

const int di[] = {1, 0, -1, 0};
const int dj[] = {0, 1, 0, -1};
}  // namespace

struct Coord {
  int i, j;

  bool valid() { return i >= 0 && i < m && j >= 0 && j < n; }
  Coord move(size_t k) { return Coord{i + di[k], j + dj[k]}; }
  bool operator==(const Coord& o) const { return i == o.i && j == o.j; }
};

namespace std {
template <>
struct hash<Coord> {
  size_t operator()(const Coord& coord) const {
    return hash<int>()(coord.i) ^ (hash<int>()(coord.j) << 1);
  }
};
}  // namespace std

vector<vector<Coord>> get_reachable_coords(Coord c0) {
  vector<vector<Coord>> reachable(10, vector<Coord>());
  vector<vector<bool>> visited(m, vector<bool>(n, false));
  vector<Coord> s;
  s.reserve(10);
  reachable[0].push_back(c0);
  visited[c0.i][c0.j] = true;
  s.push_back(c0);
  while (!s.empty()) {
    c0 = s.back();
    s.pop_back();
    if (map[c0.i][c0.j] == '9') {
      continue;
    }

    for (size_t k = 0; k < 4; ++k) {
      Coord c1 = c0.move(k);
      if (c1.valid() && !visited[c1.i][c1.j] &&
          map[c1.i][c1.j] == map[c0.i][c0.j] + 1) {
        reachable[map[c1.i][c1.j] - '0'].push_back(c1);
        visited[c1.i][c1.j] = true;
        s.push_back(c1);
      }
    }
  }
  return reachable;
}

int get_trail_rating(Coord c0, const vector<vector<Coord>>& reachable) {
  int trail_rating = 0;
  unordered_map<Coord, int> count;
  count[c0] = 1;
  for (size_t depth = 1; depth < 10; ++depth) {
    for (Coord c1 : reachable[depth]) {
      for (size_t k = 0; k < 4; ++k) {
        Coord c0 = c1.move(k);
        if (c0.valid() && map[c1.i][c1.j] == map[c0.i][c0.j] + 1) {
          count[c1] += count[c0];
        }
      }
    }
  }
  for (Coord c0 : reachable[9]) {
    trail_rating += count[c0];
  }
  return trail_rating;
}

int main() {
  ifstream input_file("../data/day_10_input.txt");
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

  int sum_part1 = 0;
  int sum_part2 = 0;
  Coord c0;
  for (c0.i = 0; c0.i < m; ++c0.i) {
    for (c0.j = 0; c0.j < n; ++c0.j) {
      if (map[c0.i][c0.j] == '0') {
        vector<vector<Coord>> reachable = get_reachable_coords(c0);
        if (reachable[9].empty()) {
          continue;
        }

        sum_part1 += reachable[9].size();
        sum_part2 += get_trail_rating(c0, reachable);
      }
    }
  }

  cout << sum_part1 << "\n";
  cout << sum_part2 << "\n";

  return 0;
}
