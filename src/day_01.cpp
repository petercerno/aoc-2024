#include <algorithm>
#include <cstdio>
#include <unordered_map>
#include <vector>

using namespace std;

int main() {
  int num1, num2;
  vector<int> v1, v2;
  unordered_map<int, int> f1, f2;

  FILE* file = fopen("../data/day_01_input.txt", "r");
  if (file == nullptr) {
    perror("Error opening file");
    return 1;
  }

  v1.reserve(1000);
  v2.reserve(1000);
  while (fscanf(file, "%d %d", &num1, &num2) == 2) {
    v1.push_back(num1);
    v2.push_back(num2);
    f1[num1]++;
    f2[num2]++;
  }
  fclose(file);

  // Part 1
  sort(v1.begin(), v1.end());
  sort(v2.begin(), v2.end());
  long long sum1 = 0;
  for (size_t i = 0; i < v1.size(); ++i) {
    sum1 += abs(v1[i] - v2[i]);
  }
  printf("%lld\n", sum1);

  // Part 2
  long long sum2 = 0;
  for (const auto& [num1, c1] : f1) {
    sum2 += static_cast<long long>(num1) * c1 * f2[num1];
  }
  printf("%lld\n", sum2);

  return 0;
}
