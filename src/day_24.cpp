#include <algorithm>
#include <cassert>
#include <deque>
#include <fstream>
#include <iostream>
#include <memory>
#include <regex>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

string format_label(char c, size_t num) {
  string label(3, ' ');
  label[0] = c;
  label[1] = '0' + (num / 10) % 10;
  label[2] = '0' + (num % 10);
  return label;
}

struct GenericGate {
  size_t inp1;
  size_t inp2;
  string gate;
};

struct GenericCircuit {
  size_t maybe_add_label(const string& label) {
    const size_t new_index = index_to_label.size();
    const auto insert_result = label_to_index.insert({label, new_index});
    if (!insert_result.second) return insert_result.first->second;
    index_to_label.emplace_back(label);          // New wire added
    connections.emplace_back();                  // No connected input wires
    value.push_back(-1);                         // Invalid wire value
    pending.push_back(2);                        // Two pending inputs
    gate.push_back({new_index, new_index, ""});  // Invalid gate
    return new_index;
  }

  void set_wire_value(const string& wire_label, int wire_value) {
    const size_t index = maybe_add_label(wire_label);
    value[index] = wire_value;
    pending[index] = 0;
    queue.push_back(index);
  }

  void set_wire_gate(const string& inp1_label, const string& gate_label,
                     const string& inp2_label, const string& out_label) {
    const size_t i1 = maybe_add_label(inp1_label);
    const size_t i2 = maybe_add_label(inp2_label);
    const size_t i0 = maybe_add_label(out_label);
    connections[i1].push_back(i0);
    connections[i2].push_back(i0);
    gate[i0] = {i1, i2, gate_label};
  }

  long long solve() {
    while (!queue.empty()) {
      const size_t i0 = queue.front();
      queue.pop_front();
      for (size_t j0 : connections[i0]) {
        --pending[j0];
        if (pending[j0] == 0) {
          const size_t i1 = gate[j0].inp1;
          const size_t i2 = gate[j0].inp2;
          if (gate[j0].gate == "AND") {
            value[j0] = value[i1] & value[i2];
          } else if (gate[j0].gate == "OR") {
            value[j0] = value[i1] | value[i2];
          } else if (gate[j0].gate == "XOR") {
            value[j0] = value[i1] ^ value[i2];
          }
          queue.push_back(j0);
        }
      }
    }
    long long z_num = 0;
    for (size_t i = 0;; ++i) {
      const string z_i = format_label('z', i);
      const auto it = label_to_index.find(z_i);
      if (it == label_to_index.end()) break;
      z_num += static_cast<long long>(value.at(it->second)) << i;
    }
    return z_num;
  }

  vector<string> index_to_label;                 // Maps wire index to label
  unordered_map<string, size_t> label_to_index;  // Maps wire label to index
  vector<vector<size_t>> connections;            // Connections to input wires
  vector<int> value;                             // Wire value
  vector<int> pending;                           // Number of pending inputs
  vector<GenericGate> gate;                      // Gate computing the wire
  deque<size_t> queue;                           // Wires to be processed
};

struct AdderGate {
  string inp1;
  string inp2;
  string out;
  string gate;
  size_t inp1_num;
  size_t inp2_num;
  size_t out_num;
};

struct AdderCircuit {
  void set_wire_gate(const string& inp1_label, const string& gate_label,
                     const string& inp2_label, const string& out_label) {
    const size_t index = gate.size();
    AdderGate& ag = gate.emplace_back(
        AdderGate{inp1_label, inp2_label, out_label, gate_label});
    if (is_digit_label(inp1_label)) ag.inp1_num = get_label_num(inp1_label);
    if (is_digit_label(inp2_label)) ag.inp2_num = get_label_num(inp2_label);
    if (is_digit_label(out_label)) ag.out_num = get_label_num(out_label);
    if (is_xy_label(inp1_label)) {
      xy_gate_indices.push_back(index);
    } else {
      if (ag.gate == "XOR") {
        xor_gate_indices.push_back(index);
      } else if (ag.gate == "AND") {
        and_gate_indices.push_back(index);
      } else if (ag.gate == "OR") {
        or_gate_indices.push_back(index);
      }
    }
  }

  vector<string> solve() {
    n = 0;
    for (size_t i = 0; i < gate.size(); ++i) {
      if (!is_z_label(gate[i].out)) continue;
      if (gate[i].out_num + 1 > n) n = gate[i].out_num + 1;
    }
    vector<string> swaps;
    vector<size_t> candidates = get_candidates();
    // Greedily improve the binary adder circuit structure:
    while (!candidates.empty()) {
      vector<size_t> best_next_candidates = candidates;
      string best_swap_label1;
      string best_swap_label2;
      for (size_t i1 = 0; i1 < candidates.size() - 1; ++i1) {
        for (size_t i2 = i1 + 1; i2 < candidates.size(); ++i2) {
          const string label1 = gate[candidates[i1]].out;
          const string label2 = gate[candidates[i2]].out;
          swap_labels(label1, label2);
          vector<size_t> new_candidates = get_candidates();
          if (new_candidates.size() < best_next_candidates.size()) {
            best_next_candidates = std::move(new_candidates);
            best_swap_label1 = label1;
            best_swap_label2 = label2;
          }
          swap_labels(label1, label2);
        }
      }
      if (best_next_candidates.size() < candidates.size()) {
        candidates = std::move(best_next_candidates);
        swap_labels(best_swap_label1, best_swap_label2);
        swaps.push_back(best_swap_label1);
        swaps.push_back(best_swap_label2);
      } else {
        assert(false);  // Cannot improve
      }
    }
    sort(swaps.begin(), swaps.end());
    return swaps;
  }

  void swap_labels(const string& label1, const string& label2) {
    for (size_t i = 0; i < gate.size(); ++i) {
      if (gate[i].out == label1) {
        gate[i].out = label2;
      } else if (gate[i].out == label2) {
        gate[i].out = label1;
      }
    }
  }

  // Verifies the following binary adder circuit structure:
  //
  // x[0] XOR y[0] -> z[0]
  // x[0] AND y[0] -> car[1]
  //
  // For 1 <= i < n:
  //   x[i] XOR y[i] -> xny[i]
  //   x[i] AND y[i] -> xay[i]
  //   car[i] XOR xny[i] -> z[i]
  //   car[i] AND xny[i] -> tmp[i]
  //   xay[i] OR  tmp[i] -> car[i+1]  (i < n - 1)
  //
  // xay[n-1] OR tmp[n-1] -> z[n]
  //
  // Returns the indices of all gates breaking this structure.
  vector<size_t> get_candidates() {
    vector<bool> verified_gate(gate.size(), false);
    vector<string> num_to_xny(n);
    vector<string> num_to_xay(n);
    vector<string> num_to_car(n);
    vector<string> num_to_tmp(n);
    unordered_map<string, size_t> xny_to_num;
    unordered_map<string, size_t> xay_to_num;
    unordered_map<string, size_t> car_to_num;
    unordered_map<string, size_t> tmp_to_num;
    vector<bool> num_passed(n, false);
    num_passed[n - 1] = true;
    // Scan and verify the pattern: x[i] XOR / AND y[i]
    // Identify all xny[i] and xay[i] labels (and also car[0] label).
    for (size_t i : xy_gate_indices) {
      const size_t xy_num1 = gate[i].inp1_num;
      const size_t xy_num2 = gate[i].inp2_num;
      assert(xy_num1 == xy_num2);
      if (gate[i].gate == "XOR") {
        num_to_xny[xy_num1] = gate[i].out;
        xny_to_num[gate[i].out] = xy_num1;
        if (xy_num1 == 0 && gate[i].out == "z00") num_passed[xy_num1] = true;
      } else if (gate[i].gate == "AND") {
        num_to_xay[xy_num1] = gate[i].out;
        xay_to_num[gate[i].out] = xy_num1;
        if (xy_num1 == 0) {
          num_to_car[xy_num1] = gate[i].out;
          car_to_num[gate[i].out] = xy_num1;
        }
      }
    }
    // Scan and verify the pattern: car[i] XOR xny[i] -> z[i]
    // Identify all car[i] labels.
    for (size_t i : xor_gate_indices) {
      size_t xny_num = n;
      const string* carry_label = nullptr;
      const auto it1 = xny_to_num.find(gate[i].inp1);
      const auto it2 = xny_to_num.find(gate[i].inp2);
      if (it1 != xny_to_num.end()) {
        xny_num = it1->second;
        carry_label = &gate[i].inp2;
      } else if (it2 != xny_to_num.end()) {
        xny_num = it2->second;
        carry_label = &gate[i].inp1;
      }
      if (xny_num < n && gate[i].out == format_label('z', xny_num)) {
        num_passed[xny_num] = true;
        num_to_car[xny_num] = *carry_label;
        car_to_num[*carry_label] = xny_num;
      }
    }
    // Scan and verify the pattern: car[i] AND xny[i] -> tmp[i]
    // Identify all tmp[i] labels.
    for (size_t i : and_gate_indices) {
      size_t xny_num = n;
      const string* carry_label = nullptr;
      const auto it1 = xny_to_num.find(gate[i].inp1);
      const auto it2 = xny_to_num.find(gate[i].inp2);
      if (it1 != xny_to_num.end()) {
        xny_num = it1->second;
        carry_label = &gate[i].inp2;
      } else if (it2 != xny_to_num.end()) {
        xny_num = it2->second;
        carry_label = &gate[i].inp1;
      }
      if (xny_num < n && num_passed[xny_num] &&
          car_to_num[*carry_label] == xny_num) {
        num_to_tmp[xny_num] = gate[i].out;
        tmp_to_num[gate[i].out] = xny_num;
      } else {
        num_passed[xny_num] = false;
      }
    }
    // Scan and verify the pattern: xay[i] OR tmp[i] -> car[i+1] / z[n]
    for (size_t i : or_gate_indices) {
      size_t xay_num = n;
      const string* tmp_label = nullptr;
      const auto it1 = xay_to_num.find(gate[i].inp1);
      const auto it2 = xay_to_num.find(gate[i].inp2);
      if (it1 != xay_to_num.end()) {
        xay_num = it1->second;
        tmp_label = &gate[i].inp2;
      } else if (it2 != xay_to_num.end()) {
        xay_num = it2->second;
        tmp_label = &gate[i].inp1;
      }
      if (xay_num < n && num_passed[xay_num] &&
          tmp_to_num[*tmp_label] == xay_num &&
          ((xay_num <= n - 3 && gate[i].out == num_to_car[xay_num + 1]) ||
           (xay_num == n - 2 &&
            gate[i].out == format_label('z', xay_num + 1)))) {
        // Passed
      } else {
        num_passed[xay_num] = false;
      }
    }
    // Verify all gates corresponding to individual patterns.
    for (size_t i = 0; i < gate.size(); ++i) {
      if (is_xy_label(gate[i].inp1)) {
        const size_t xy_num1 = gate[i].inp1_num;
        const size_t xy_num2 = gate[i].inp2_num;
        assert(xy_num1 == xy_num2);
        if (num_passed[xy_num1]) {
          verified_gate[i] = true;
        }
      } else if (gate[i].gate == "XOR") {
        size_t xny_num = n;
        const auto it1 = xny_to_num.find(gate[i].inp1);
        const auto it2 = xny_to_num.find(gate[i].inp2);
        if (it1 != xny_to_num.end()) {
          xny_num = it1->second;
        } else if (it2 != xny_to_num.end()) {
          xny_num = it2->second;
        }
        if (xny_num < n && num_passed[xny_num] &&
            gate[i].out == format_label('z', xny_num)) {
          verified_gate[i] = true;
        }
      } else if (gate[i].gate == "AND") {
        size_t xny_num = n;
        const string* carry_label = nullptr;
        const auto it1 = xny_to_num.find(gate[i].inp1);
        const auto it2 = xny_to_num.find(gate[i].inp2);
        if (it1 != xny_to_num.end()) {
          xny_num = it1->second;
          carry_label = &gate[i].inp2;
        } else if (it2 != xny_to_num.end()) {
          xny_num = it2->second;
          carry_label = &gate[i].inp1;
        }
        if (xny_num < n && num_passed[xny_num] &&
            car_to_num[*carry_label] == xny_num) {
          verified_gate[i] = true;
        }
      } else if (gate[i].gate == "OR") {
        size_t xay_num = n;
        const string* tmp_label = nullptr;
        const auto it1 = xay_to_num.find(gate[i].inp1);
        const auto it2 = xay_to_num.find(gate[i].inp2);
        if (it1 != xay_to_num.end()) {
          xay_num = it1->second;
          tmp_label = &gate[i].inp2;
        } else if (it2 != xay_to_num.end()) {
          xay_num = it2->second;
          tmp_label = &gate[i].inp1;
        }
        if (xay_num < n && num_passed[xay_num] &&
            tmp_to_num[*tmp_label] == xay_num &&
            ((xay_num <= n - 3 && gate[i].out == num_to_car[xay_num + 1]) ||
             (xay_num == n - 2 &&
              gate[i].out == format_label('z', xay_num + 1)))) {
          verified_gate[i] = true;
        }
      }
    }
    vector<size_t> candidates;
    candidates.reserve(gate.size() / 2);
    for (size_t i = 0; i < gate.size(); ++i) {
      if (!verified_gate[i]) candidates.push_back(i);
    }
    return candidates;
  }

  static bool is_digit_label(const string& label) {
    assert(label.size() == 3);
    return label[1] >= '0' && label[1] <= '9' &&  // nowrap
           label[2] >= '0' && label[2] <= '9';
  }

  static bool is_xy_label(const string& label) {
    return (label[0] == 'x' || label[0] == 'y') &&  // nowrap
           (is_digit_label(label));
  }

  static bool is_z_label(const string& label) {
    return label[0] == 'z' && is_digit_label(label);
  }

  static size_t get_label_num(const string& label) {
    return 10 * (label[1] - '0') + (label[2] - '0');
  }

  size_t n = 0;
  vector<AdderGate> gate;
  vector<size_t> xy_gate_indices;
  vector<size_t> xor_gate_indices;
  vector<size_t> and_gate_indices;
  vector<size_t> or_gate_indices;
};

void print(const vector<string>& values) {
  if (values.empty()) return;
  cout << values[0];
  for (size_t i = 1; i < values.size(); ++i) {
    cout << "," << values[i];
  }
  cout << "\n";
}

int main() {
  ifstream input_file("../data/day_24_input.txt");
  if (!input_file) {
    cerr << "Error opening file.\n";
    return 1;
  }

  GenericCircuit generic_circuit;
  AdderCircuit adder_circuit;
  string line;
  regex init_pattern(R"raw((.*): (0|1))raw");
  while (getline(input_file, line)) {
    smatch match;
    if (!regex_match(line, match, init_pattern)) break;
    const string wire_label = match[1];
    const string wire_value = match[2];
    generic_circuit.set_wire_value(wire_label, wire_value[0] - '0');
  }
  assert(line.empty());
  regex gate_pattern(R"raw((.*) (AND|OR|XOR) (.*) -> (.*))raw");
  while (getline(input_file, line)) {
    smatch match;
    assert(regex_match(line, match, gate_pattern));
    generic_circuit.set_wire_gate(match[1], match[2], match[3], match[4]);
    adder_circuit.set_wire_gate(match[1], match[2], match[3], match[4]);
  }
  input_file.close();

  cout << generic_circuit.solve() << "\n";  // Part 1
  print(adder_circuit.solve());             // Part 2

  return 0;
}
