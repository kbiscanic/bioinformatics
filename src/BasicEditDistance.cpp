#include "BasicEditDistance.hpp"

BasicEditDistance:: BasicEditDistance(string startingString, string targetString)
{
    first = startingString;
    second = targetString;

    reset();
}

void BasicEditDistance::reset()
{
    result = -1;

    /* set up basic costs:
    Replace(i, j) = 2 (for i != j)
    Delete = Insert = 1
    */
    for (int i=0; i<255; i++) {
        for (int j=i+1; j<256; j++) {
            setCosts(i, j, !!i + !!j, true);
        }
        setCosts(i, i, 0);
    }
}

void BasicEditDistance::setCosts(int c1, int c2, float value, bool mirror_cost)
{
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

    /* reducing memory to linear
    // deletion cost partial sums (when we have extra characters in the first string
    for (int i=1; i<=first.size(); i++) {
        dp[i][0] = dp[i-1][0] + cost[first[i-1]][EDIST_BLANK];
    }
    */

    // insertion cost partial sums (when we have extra characters in the target string
    for (int i=1; i<=second.size(); i++) {
        dp[0][i] = dp[0][i-1] + cost[EDIST_BLANK][second[i-1]];
    }

    bool alt = 1; // alternating the rows uses O(m) instead of O(n*m) memory
    for (int i=1; i<=first.size(); i++, alt=!alt) {
        for (int j=1; j<=second.size(); j++) {
            dp[alt][j] = min(min(
                                 dp[!alt][j-1] + cost[first[i-1]][second[j-1]], // replace
                                 dp[!alt][j] + cost[first[i-1]][EDIST_BLANK]), // delete
                             dp[alt][j-1] + cost[EDIST_BLANK][second[i-1]]); // insert
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
