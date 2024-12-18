#include <cassert>
#include <fstream>
#include <iostream>
#include <limits>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

struct Comp {
  vector<size_t> reg;  // Registers
  vector<size_t> prg;  // Instructions
  size_t ip = 0;       // Instruction pointer
  vector<size_t> res;  // Output

  size_t lit() const { return prg[ip + 1]; }
  size_t combo() const { return lit() < 4 ? lit() : reg[lit() - 4]; }
  void adv() { reg[0] = reg[0] >> combo(), ip += 2; }
  void bxl() { reg[1] = reg[1] ^ lit(), ip += 2; }
  void bst() { reg[1] = combo() & 7, ip += 2; }
  void jnz() { ip = reg[0] == 0 ? ip + 2 : lit(); }
  void bxc() { reg[1] = reg[1] ^ reg[2], ip += 2; }
  void out() { res.push_back(combo() & 7), ip += 2; }
  void bdv() { reg[1] = reg[0] >> combo(), ip += 2; }
  void cdv() { reg[2] = reg[0] >> combo(), ip += 2; }
  void (Comp::* ins[8])() = {&Comp::adv, &Comp::bxl, &Comp::bst, &Comp::jnz,
                             &Comp::bxc, &Comp::out, &Comp::bdv, &Comp::cdv};
  bool halted() const { return ip >= prg.size(); }
  void exec() { (this->*ins[prg[ip]])(); }
  bool match() const { return res == prg; }
  void print() const {
    cout << res[0];
    for (size_t i = 1; i < res.size(); ++i) cout << ',' << res[i];
    cout << "\n";
  }
};

std::vector<size_t> split(const std::string& str) {
  vector<size_t> result;
  istringstream stream(str);
  string token;
  while (getline(stream, token, ',')) result.push_back(stoi(token));
  return result;
}

vector<size_t> reg;
vector<size_t> prg;
vector<vector<size_t>> out_to_reg;
size_t best_reg_a = numeric_limits<size_t>::max();

void search(size_t d, size_t acc_reg_a) {
  if (d == prg.size()) {
    Comp comp{{acc_reg_a, reg[1], reg[2]}, prg};
    while (!comp.halted()) comp.exec();
    if (comp.match() && acc_reg_a < best_reg_a) {
      best_reg_a = acc_reg_a;
    }
    return;
  }

  for (size_t reg_a : out_to_reg[prg[d]]) {
    if ((reg_a & 63) == (acc_reg_a >> (3 * d))) {
      search(d + 1, acc_reg_a + ((reg_a >> 6) << (3 * d + 6)));
    }
  }
}

int main() {
  ifstream input_file("../data/day_17_input.txt");
  if (!input_file) {
    cerr << "Error opening file.\n";
    return 1;
  }

  string line;
  regex register_pattern(R"raw(Register .: (\d+))raw");
  while (getline(input_file, line)) {
    if (line.empty()) break;
    smatch match;
    assert(regex_match(line, match, register_pattern));
    reg.push_back(stoi(match[1]));
  }
  assert(getline(input_file, line));
  input_file.close();
  regex program_pattern(R"raw(Program: (.*))raw");
  smatch match;
  assert(regex_match(line, match, program_pattern));
  prg = split(match[1]);

  // Part 1
  Comp comp{reg, prg};
  while (!comp.halted()) comp.exec();
  comp.print();

  // Part 2
  out_to_reg.assign(8, {});
  for (size_t reg_a = 0; reg_a < 1024; ++reg_a) {
    Comp comp{{reg_a, reg[1], reg[2]}, prg};
    while (comp.res.empty()) comp.exec();
    out_to_reg[comp.res[0]].push_back(reg_a);
  }
  for (size_t reg_a : out_to_reg[prg[0]]) search(1, reg_a);
  cout << best_reg_a << "\n";
  return 0;
}
