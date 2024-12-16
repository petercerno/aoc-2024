#include <fstream>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;

struct Map {
  vector<string> data;
  int m = 0;
  int n = 0;

  int sum() {
    int result = 0;
    for (int i = 0; i < m; ++i) {
      for (int j = 0; j < n; ++j) {
        if (data[i][j] == 'O' || data[i][j] == '[') {
          result += 100 * i + j;
        }
      }
    }
    return result;
  }
};

constexpr int di[] = {-1, 0, 1, 0};
constexpr int dj[] = {0, 1, 0, -1};

struct Coord {
  Map* map = nullptr;
  int i, j;

  bool valid() const { return i >= 0 && i < map->m && j >= 0 && j < map->n; }
  bool operator==(const Coord& o) const { return i == o.i && j == o.j; }
  Coord add(int di, int dj) const { return {map, i + di, j + dj}; }
  Coord move(size_t k) const { return add(di[k], dj[k]); }
  void move_self(size_t k) { i += di[k], j += dj[k]; }
};

namespace std {
template <>
struct hash<Coord> {
  size_t operator()(const Coord& coord) const {
    return hash<int>()(coord.i) ^ (hash<int>()(coord.j) << 1);
  }
};
}  // namespace std

optional<size_t> dtype(char c) {
  switch (c) {
    case '^':
      return 0;
    case '>':
      return 1;
    case 'v':
      return 2;
    case '<':
      return 3;
  }
  return nullopt;
}

optional<Coord> find_robot(Map& map) {
  for (int i = 0; i < map.m; ++i) {
    for (int j = 0; j < map.n; ++j) {
      if (map.data[i][j] == '@') {
        return Coord{&map, i, j};
      }
    }
  }
  return nullopt;
}

optional<Coord> next_free(Coord c, size_t k) {
  while (c.valid() && c.map->data[c.i][c.j] == 'O') c.move_self(k);
  if (c.valid() && c.map->data[c.i][c.j] == '.') return c;
  return nullopt;
}

Coord move_robot_part1(Coord c0, size_t k) {
  vector<string>& data = c0.map->data;
  Coord c1 = c0.move(k);
  optional<Coord> maybe_c2 = next_free(c1, k);
  if (!maybe_c2.has_value()) return c0;
  Coord c2 = maybe_c2.value();
  data[c0.i][c0.j] = '.';
  data[c2.i][c2.j] = 'O';
  data[c1.i][c1.j] = '@';
  return c1;
}

Map expand(const Map& map1) {
  Map map2;
  map2.data.reserve(map1.m);
  for (int i = 0; i < map1.m; ++i) {
    string line;
    line.reserve(2 * map1.n);
    for (int j = 0; j < map1.n; ++j) {
      const char c = map1.data[i][j];
      if (c == 'O') {
        line.push_back('[');
        line.push_back(']');
      } else if (c == '@') {
        line.push_back('@');
        line.push_back('.');
      } else {
        line.push_back(c);
        line.push_back(c);
      }
    }
    map2.data.emplace_back(std::move(line));
  }
  map2.m = map2.data.size();
  map2.n = map2.data[0].size();
  return map2;
}

bool move_block(Coord c0, size_t k) {
  vector<string>& data = c0.map->data;
  if (!c0.valid() || data[c0.i][c0.j] == '#') return false;
  if (data[c0.i][c0.j] == '.') return true;  // Nothing to move
  unordered_set<Coord> block;
  vector<Coord> stack;
  block.insert(c0);
  stack.push_back(c0);
  while (!stack.empty()) {
    Coord c0 = stack.back();
    stack.pop_back();
    Coord c1 = c0.add(0, -1);
    Coord c2 = c0.add(0, 1);
    Coord c3 = c0.move(k);
    if (data[c0.i][c0.j] == ']' && block.insert(c1).second) stack.push_back(c1);
    if (data[c0.i][c0.j] == '[' && block.insert(c2).second) stack.push_back(c2);
    if (!c3.valid() || data[c3.i][c3.j] == '#') return false;  // Cannot move
    if (data[c3.i][c3.j] != '.' && block.insert(c3).second) stack.push_back(c3);
  }
  unordered_map<Coord, char> moved_block;
  for (Coord c0 : block) moved_block.insert({c0.move(k), data[c0.i][c0.j]});
  for (Coord c0 : block) data[c0.i][c0.j] = '.';
  for (auto [c1, c0_value] : moved_block) data[c1.i][c1.j] = c0_value;
  return true;
}

Coord move_robot_part2(Coord c0, size_t k) {
  vector<string>& data = c0.map->data;
  Coord c1 = c0.move(k);
  if (!move_block(c1, k)) return c0;
  data[c0.i][c0.j] = '.';
  data[c1.i][c1.j] = '@';
  return c1;
}

int main() {
  ifstream input_file("../data/day_15_input.txt");
  if (!input_file) {
    cerr << "Error opening file.\n";
    return 1;
  }

  Map map1;
  Map map2;
  string line;
  while (getline(input_file, line)) {
    if (line.empty()) break;
    map1.data.emplace_back(line);
  }
  map1.m = map1.data.size();
  map1.n = map1.data[0].size();
  map2 = expand(map1);

  string moves;
  while (getline(input_file, line)) {
    moves += line;
  }
  input_file.close();

  Coord r1 = find_robot(map1).value();
  Coord r2 = find_robot(map2).value();
  for (char c : moves) {
    size_t k = dtype(c).value();
    r1 = move_robot_part1(r1, k);
    r2 = move_robot_part2(r2, k);
  }

  cout << map1.sum() << "\n";
  cout << map2.sum() << "\n";

  return 0;
}
