#include "BasicEditDistance.hpp"

BasicEditDistance::BasicEditDistance(string startingString,
                                     string targetString) {
  first = startingString;
  second = targetString;

  reset();
}

void BasicEditDistance::reset() {
  result = -1;

  /* set up basic costs:
  Replace(i, j) = 2 (for i != j)
  Delete = Insert = 1
  */
  for (int i = 0; i < 255; i++) {
    for (int j = i + 1; j < 256; j++) {
      setCosts(i, j, !!(!!i + !!j), true);
    }
    setCosts(i, i, 0);
  }
}

void BasicEditDistance::setCosts(int c1, int c2, float value,
                                 bool mirror_cost) {
  cost[c1][c2] = value;

  if (mirror_cost) {
    cost[c2][c1] = value;
  }
}

float BasicEditDistance::getResult() {
  if (!~result) {
    calculate();
  }

  return result;
}

void BasicEditDistance::calculate() {
  // the starting state
  dp[0][0] = 0;

  // insertion cost partial sums (when we have extra characters in the target
  // string
  for (unsigned int i = 1; i <= second.size(); i++) {
    dp[0][i] = dp[0][i - 1] + cost[EDIST_BLANK][int(second[i - 1])];
  }

  int alt = 1;  // alternating the rows uses O(m) instead of O(n*m) memory
  for (unsigned int i = 1; i <= first.size(); i++, alt = !alt) {
    dp[alt][0] = cost[int(first[i - 1])][EDIST_BLANK] + dp[!alt][0];
    for (unsigned int j = 1; j <= second.size(); j++) {
      dp[alt][j] = min(
          min(dp[!alt][j - 1] + cost[int(first[i - 1])][int(second[j - 1])],  // replace
              dp[!alt][j] + cost[int(first[i - 1])][EDIST_BLANK]),       // delete
          dp[alt][j - 1] + cost[EDIST_BLANK][int(second[i - 1])]);       // insert
    }
  }

  result = dp[!alt][second.size()];
}

/* 'Unit' test
int main() {
    string first = "testing";
    string second = "rofltest";
    BasicEditDistance tester(first, second);
    cout << tester.getResult() << endl;
    return 0;
}
*/
