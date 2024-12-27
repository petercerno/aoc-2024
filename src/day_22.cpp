#include <deque>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>

using namespace std;
using LL = long long;

LL evolve(LL secret) {
  static constexpr LL keep_24_bits = (1ll << 24) - 1;
  secret = (secret ^ (secret << 6)) & keep_24_bits;
  secret = (secret ^ (secret >> 5)) & keep_24_bits;
  secret = (secret ^ (secret << 11)) & keep_24_bits;
  return secret;
}

int get_seq_key(const deque<int>& window) {
  int seq_key = 0;
  for (size_t i = 0; i < window.size() - 1; ++i) {
    seq_key = (seq_key << 5) + 10 + window[i + 1] - window[i];
  }
  return seq_key;
}

unordered_map<int, int> process(LL secret, int n, LL& last_secret) {
  unordered_map<int, int> key2bananas;
  deque<int> window;
  int digit = secret % 10;
  window.push_back(digit);
  for (int i = 0; i < n; ++i) {
    secret = evolve(secret);
    digit = secret % 10;
    window.push_back(digit);
    if (window.size() < 5) continue;
    key2bananas.try_emplace(get_seq_key(window), digit);
    window.pop_front();
  }
  last_secret = secret;
  return key2bananas;
}

void merge(const unordered_map<int, int>& key2bananas,
           unordered_map<int, int>& acc_bananas) {
  for (const auto [key, bananas] : key2bananas) acc_bananas[key] += bananas;
}

int max_bananas(const unordered_map<int, int>& acc_bananas) {
  int max = 0;
  for (const auto [key, bananas] : acc_bananas) {
    if (bananas > max) max = bananas;
  }
  return max;
}

int main() {
  ifstream input_file("../data/day_22_input.txt");
  if (!input_file) {
    cerr << "Error opening file.\n";
    return 1;
  }

  LL sum = 0;
  unordered_map<int, int> acc_bananas;
  string line;
  while (getline(input_file, line)) {
    LL secret = stoll(line);
    merge(process(secret, 2000, secret), acc_bananas);
    sum += secret;
  }
  input_file.close();

  cout << sum << "\n";
  cout << max_bananas(acc_bananas) << "\n";

  return 0;
}
