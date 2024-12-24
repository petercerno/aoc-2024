#include <deque>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

using namespace std;
using Layout = vector<vector<char>>;

struct KeypadState {
  int i_, j_;
  string out_;

  bool operator==(const KeypadState& o) const {
    return i_ == o.i_ && j_ == o.j_ && out_ == o.out_;
  }
};

namespace std {
template <>
struct hash<KeypadState> {
  size_t operator()(const KeypadState& state) const {
    return (hash<int>()(state.i_)) ^ (hash<int>()(state.j_) << 1) ^
           (hash<size_t>()(state.out_.size()) << 2);
  }
};
}  // namespace std

struct Keypad {
  Keypad(int i, int j, const Layout& layout, Keypad* keypad, bool store_output)
      : i_(i),
        j_(j),
        m_(layout.size()),
        n_(layout[0].size()),
        layout_(layout),
        keypad_(keypad),
        store_output_(store_output) {}

  KeypadState get_state() const { return {i_, j_, out_}; }
  void set_state(KeypadState s) { i_ = s.i_, j_ = s.j_, out_ = s.out_; }

  void push(char c) {
    if (store_output_) out_.push_back(c);
    if (keypad_ == nullptr) return;
    switch (c) {
      case '^':
        --keypad_->i_;
        break;
      case '<':
        --keypad_->j_;
        break;
      case 'v':
        ++keypad_->i_;
        break;
      case '>':
        ++keypad_->j_;
        break;
      case 'A':
        keypad_->push();
        break;
    }
  }

  void push(const string& str) {
    for (char c : str) push(c);
  }

  // Called externally from the chained keypad
  void push() { push(layout_[i_][j_]); }

  bool valid() const {
    return i_ >= 0 && i_ < m_ && j_ >= 0 && j_ < n_ && layout_[i_][j_] != ' ';
  }

  int i_;  // Row
  int j_;  // Column
  int m_;  // Number of rows
  int n_;  // Number of columns
  const Layout& layout_;
  Keypad* keypad_;
  bool store_output_;
  string out_;
};

static const Layout num_layout{
    {'7', '8', '9'}, {'4', '5', '6'}, {'1', '2', '3'}, {' ', '0', 'A'}};
static const Layout dir_layout{{' ', '^', 'A'}, {'<', 'v', '>'}};

struct KeypadChainState {
  vector<KeypadState> states_;

  bool operator==(const KeypadChainState& o) const {
    return states_ == o.states_;
  }
};

namespace std {
template <>
struct hash<KeypadChainState> {
  size_t operator()(const KeypadChainState& chain_state) const {
    size_t keypad_chain_hash = 0;
    for (size_t i = 0; i < chain_state.states_.size(); ++i) {
      keypad_chain_hash ^= hash<KeypadState>()(chain_state.states_[i]) << i;
    }
    return keypad_chain_hash;
  }
};
}  // namespace std

struct KeypadChain {
  vector<unique_ptr<Keypad>> keypads_;

  KeypadChain(size_t num_keypads, bool start_with_numeric_keypad) {
    if (start_with_numeric_keypad) {
      keypads_.emplace_back(std::move(make_unique<Keypad>(
          3, 2, num_layout, /*keypad=*/nullptr, /*store_output=*/true)));
    } else {
      keypads_.emplace_back(std::move(make_unique<Keypad>(
          0, 2, dir_layout, /*keypad=*/nullptr, /*store_output=*/true)));
    }
    for (size_t i = 1; i < num_keypads; ++i) {
      keypads_.emplace_back(std::move(make_unique<Keypad>(
          0, 2, dir_layout, /*keypad=*/keypads_.back().get(),
          /*store_output=*/false)));
    }
  }

  KeypadChainState get_state() const {
    KeypadChainState state;
    state.states_.reserve(keypads_.size());
    for (const unique_ptr<Keypad>& keypad : keypads_) {
      state.states_.push_back(keypad->get_state());
    }
    return state;
  }
  void set_state(KeypadChainState state) {
    for (size_t i = 0; i < state.states_.size(); ++i) {
      keypads_[i]->set_state(state.states_[i]);
    }
  }

  bool valid(const string& target) const {
    for (const unique_ptr<Keypad>& keypad : keypads_) {
      if (!keypad->valid()) return false;
    }
    return target.starts_with(keypads_.front()->out_);
  }

  bool completed(const string& target) const {
    return (keypads_.front()->out_.size() == target.size()) && valid(target);
  }

  string bfs_search(const string& target) {
    static const vector<char> moves{'^', 'A', '<', 'v', '>'};
    deque<KeypadChainState> queue;
    unordered_map<KeypadChainState, int> dist;
    unordered_map<KeypadChainState, pair<char, KeypadChainState>> parent;
    KeypadChainState s0 = get_state();
    queue.push_back(s0);
    dist[s0] = 0;
    while (!queue.empty()) {
      s0 = queue.front();
      queue.pop_front();
      set_state(s0);
      if (completed(target)) break;
      const int d0 = dist[s0];
      for (char c : moves) {
        set_state(s0);
        keypads_.back()->push(c);
        if (!valid(target)) continue;
        const KeypadChainState s1 = get_state();
        if (dist.insert({s1, d0 + 1}).second) {
          parent[s1] = {c, s0};
          queue.push_back(s1);
        }
      }
    }
    string out_rev;
    while (true) {
      auto it = parent.find(s0);
      if (it == parent.end()) break;
      out_rev.push_back(it->second.first);
      s0 = it->second.second;
    }
    return {out_rev.rbegin(), out_rev.rend()};
  }
};

int numeric_part(const string& target) {
  int num = 0;
  for (char c : target) {
    if (c >= '0' && c <= '9') num = 10 * num + (c - '0');
  }
  return num;
}

long long solve_part1(const string& line) {
  return KeypadChain(4, true).bfs_search(line).size() * numeric_part(line);
}

// Splits the input string into A-parts (parts ending with the letter 'A').
vector<string> split_to_a_parts(const string& input) {
  vector<string> parts;
  parts.emplace_back();
  for (const char c : input) {
    parts.back().push_back(c);
    if (c == 'A') parts.emplace_back();
  }
  parts.pop_back();
  return parts;
}

// Finds optimal sequence of button presses for the directional keypad A-part.
const vector<string>& search_dir2dir(const string& a_part) {
  static unordered_map<string, vector<string>> cache;
  if (cache.contains(a_part)) return cache.at(a_part);
  string solution = KeypadChain(5, false).bfs_search(a_part);
  KeypadChain output_chain(4, false);
  output_chain.keypads_.back()->push(solution);
  cache[a_part] = split_to_a_parts(output_chain.keypads_.front()->out_);
  return cache.at(a_part);
}

// Depth represents the number of directional keypads that robots are using.
long long solve_at_depth(size_t depth, const string& a_part) {
  static constexpr int kMaxDepth = 26;
  static vector<unordered_map<string, long long>> cache(
      kMaxDepth, unordered_map<string, long long>());
  if (depth == 0) return a_part.size();
  if (cache[depth].contains(a_part)) return cache[depth].at(a_part);
  long long sum = 0;
  const vector<string>& a_parts = search_dir2dir(a_part);
  for (const string& a_part : a_parts) sum += solve_at_depth(depth - 1, a_part);
  cache[depth][a_part] = sum;
  return sum;
}

long long solve_part2(const string& line) {
  string solution = KeypadChain(5, true).bfs_search(line);
  KeypadChain output_chain(4, false);
  output_chain.keypads_.back()->push(solution);
  vector<string> parts = split_to_a_parts(output_chain.keypads_.front()->out_);
  long long sum = 0;
  for (const string& part : parts) sum += solve_at_depth(25, part);
  return sum * numeric_part(line);
}

int main() {
  ifstream input_file("../data/day_21_input.txt");
  if (!input_file) {
    cerr << "Error opening file.\n";
    return 1;
  }

  long long sum_part1 = 0;
  long long sum_part2 = 0;
  string line;
  while (getline(input_file, line)) {
    sum_part1 += solve_part1(line);
    sum_part2 += solve_part2(line);
  }
  input_file.close();

  cout << sum_part1 << "\n";
  cout << sum_part2 << "\n";

  return 0;
}
