#include <cassert>
#include <fstream>
#include <iostream>
#include <regex>
#include <string>

using namespace std;

namespace {
constexpr int w = 101;
constexpr int h = 103;
constexpr int mx = (w - 1) / 2;
constexpr int my = (h - 1) / 2;

struct Robot {
  int px, py;
  int vx, vy;

  Robot move(int k) {
    return Robot{(px + vx * k) % w, (py + vy * k) % h, vx, vy};
  }
};

vector<Robot> robots;

Robot parse_robot(const string& line) {
  regex robot_pattern(R"raw(p=(-?\d+),(-?\d+) v=(-?\d+),(-?\d+))raw");
  smatch match;
  assert(regex_match(line, match, robot_pattern));
  Robot r{stoi(match[1]), stoi(match[2]), stoi(match[3]), stoi(match[4])};
  if (r.vx < 0) r.vx += w;
  if (r.vy < 0) r.vy += h;
  return r;
}

vector<vector<int>> get_map(int k) {
  vector<vector<int>> map(h, vector<int>(w, 0));
  for (Robot r : robots) {
    r = r.move(k);
    ++map[r.py][r.px];
  }
  return map;
}

bool is_suspect(const vector<vector<int>>& map) {
  for (int i = 0; i < h; ++i) {
    for (int j = 0; j < w; ++j) {
      for (int l = 0; j < w && map[i][j] > 0; ++l, ++j) {
        if (l > 15) return true;  // Long horizontal line
      }
    }
  }
  return false;
}

void print(const vector<vector<int>>& map) {
  for (int i = 0; i < h; ++i) {
    for (int j = 0; j < w; ++j) {
      cout << (map[i][j] == 0 ? '.' : 'X');
    }
    cout << "\n";
  }
}
}  // namespace

int main() {
  ifstream input_file("../data/day_14_input.txt");
  if (!input_file) {
    cerr << "Error opening file.\n";
    return 1;
  }

  robots.reserve(500);
  string line;
  while (getline(input_file, line)) {
    robots.push_back(parse_robot(line));
  }
  input_file.close();

  int q[2][2] = {{0, 0}, {0, 0}};
  for (Robot r : robots) {
    r = r.move(100);
    if (r.px != mx && r.py != my) {
      int i = r.px / (mx + 1);
      int j = r.py / (my + 1);
      ++q[i][j];
    }
  }

  // Part 1
  cout << q[0][0] * q[0][1] * q[1][0] * q[1][1] << "\n";

  // Part 2
  for (int k = 0; k < 10000; ++k) {
    vector<vector<int>> map = get_map(k);
    if (is_suspect(map)) {
      cout << k << ":\n";
      print(map);
    }
  }

  return 0;
}
