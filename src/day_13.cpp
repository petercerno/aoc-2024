#include <cassert>
#include <fstream>
#include <iostream>
#include <regex>
#include <string>

using namespace std;

namespace {
using LL = long long;

void parse(const string& line, const regex& pattern, LL& x, LL& y) {
  smatch match;
  assert(regex_match(line, match, pattern));
  x = stoll(match[1]);
  y = stoll(match[2]);
}

bool solve(LL a1, LL b1, LL c1, LL a2, LL b2, LL c2, LL& D, LL& x, LL& y) {
  D = a1 * b2 - a2 * b1;
  assert(D != 0);  // Assume non-collinear equations
  const LL Dx = c1 * b2 - c2 * b1;
  const LL Dy = a1 * c2 - a2 * c1;
  if (Dx % D == 0 && Dy % D == 0) {
    x = Dx / D;
    y = Dy / D;
    return true;
  }

  return false;
}

struct Machine {
  LL ax, ay;
  LL bx, by;
  LL cx, cy;

  LL cost_part1() {
    LL D, x, y;
    if (solve(ax, bx, cx, ay, by, cy, D, x, y) &&  // nowrap
        x >= 0 && x <= 100 && y >= 0 && y <= 100) {
      return 3 * x + y;
    }
    return 0;
  }

  LL cost_part2() {
    static constexpr LL off = 10000000000000ll;
    LL D, x, y;
    if (solve(ax, bx, cx + off, ay, by, cy + off, D, x, y) &&  // nowrap
        x >= 0 && y >= 0) {
      return 3 * x + y;
    }
    return 0;
  }
};
}  // namespace

int main() {
  ifstream input_file("../data/day_13_input.txt");
  if (!input_file) {
    cerr << "Error opening file.\n";
    return 1;
  }

  LL sum_part1 = 0;
  LL sum_part2 = 0;
  regex button_pattern(R"raw(Button .: X\+(\d+), Y\+(\d+))raw");
  regex prize_pattern(R"raw(Prize: X=(\d+), Y=(\d+))raw");
  string line;
  while (true) {
    Machine m;
    getline(input_file, line);
    parse(line, button_pattern, m.ax, m.ay);
    getline(input_file, line);
    parse(line, button_pattern, m.bx, m.by);
    getline(input_file, line);
    parse(line, prize_pattern, m.cx, m.cy);
    sum_part1 += m.cost_part1();
    sum_part2 += m.cost_part2();
    if (!getline(input_file, line)) {
      break;
    }
  }

  cout << sum_part1 << "\n";
  cout << sum_part2 << "\n";

  return 0;
}
