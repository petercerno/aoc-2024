#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

struct Graph {
  void maybe_add(const string& label) {
    if (!label_to_index.insert({label, labels.size()}).second) return;
    labels.emplace_back(label);
    adj.emplace_back();
  }

  void add_edge(const string& u, const string& v) {
    maybe_add(u);
    maybe_add(v);
    const size_t a = label_to_index[u];
    const size_t b = label_to_index[v];
    add_edge(a, b);
    add_edge(b, a);
  }

  void add_edge(size_t a, size_t b) {
    auto it_a = lower_bound(adj[a].begin(), adj[a].end(), b);
    if (it_a == adj[a].end() || *it_a != b) adj[a].insert(it_a, b);
  }

  bool starts_with_t(size_t a) const { return labels[a][0] == 't'; }

  size_t count_3_cliques() const {
    size_t count = 0;
    const size_t n = adj.size();
    for (size_t a = 0; a < n; ++a) {
      const bool a_t = starts_with_t(a);
      for (auto it_b = upper_bound(adj[a].begin(), adj[a].end(), a);
           it_b != adj[a].end(); ++it_b) {
        const size_t b = *it_b;
        const bool b_t = starts_with_t(b);
        for (auto it_c = it_b + 1; it_c != adj[a].end(); ++it_c) {
          const size_t c = *it_c;
          auto it_b_c = lower_bound(adj[b].begin(), adj[b].end(), c);
          if (it_b_c == adj[b].end() || *it_b_c != c) continue;
          const bool c_t = starts_with_t(c);
          if (a_t || b_t || c_t) ++count;
        }
      }
    }
    return count;
  }

  void build_clique(vector<size_t>& clique, vector<size_t>& top_clique) const {
    if (clique.size() > top_clique.size()) top_clique = clique;
    const size_t a = clique.front();
    for (auto it_c = upper_bound(adj[a].begin(), adj[a].end(), clique.back());
         it_c != adj[a].end(); ++it_c) {
      if (clique.size() + adj[a].end() - it_c <= top_clique.size()) break;
      const size_t c = *it_c;
      if (adj[c].size() + 1 <= top_clique.size()) continue;
      bool connected = true;
      for (size_t i = 1; connected && (i < clique.size()); ++i) {
        const size_t b = clique[i];
        const auto it_b_c = lower_bound(adj[b].begin(), adj[b].end(), c);
        connected &= it_b_c != adj[b].end() && *it_b_c == c;
      }
      if (!connected) continue;
      clique.push_back(c);
      build_clique(clique, top_clique);
      clique.pop_back();
    }
  }

  vector<size_t> find_top_clique() const {
    vector<size_t> top_clique;
    vector<size_t> clique;
    const size_t n = adj.size();
    for (size_t a = 0; a < n; ++a) {
      if (adj[a].size() + 1 <= top_clique.size()) continue;
      clique.push_back(a);
      build_clique(clique, top_clique);
      clique.pop_back();
    }
    return top_clique;
  }

  void print_clique(const vector<size_t>& clique) const {
    vector<string> c_labels;
    c_labels.reserve(clique.size());
    for (const size_t a : clique) c_labels.push_back(labels[a]);
    sort(c_labels.begin(), c_labels.end());
    cout << c_labels[0];
    for (size_t i = 1; i < c_labels.size(); ++i) cout << "," << c_labels[i];
    cout << "\n";
  }

  vector<string> labels;
  unordered_map<string, size_t> label_to_index;
  vector<vector<size_t>> adj;
};

int main() {
  ifstream input_file("../data/day_23_input.txt");
  if (!input_file) {
    cerr << "Error opening file.\n";
    return 1;
  }

  Graph graph;
  string line;
  while (getline(input_file, line)) {
    const string u = line.substr(0, 2);
    const string v = line.substr(3, 2);
    graph.add_edge(u, v);
  }
  input_file.close();

  cout << graph.count_3_cliques() << "\n";
  graph.print_clique(graph.find_top_clique());

  return 0;
}
