#include <cstdio>
#include <iostream>
#include <string>

using namespace std;

#define EDIST_MAX_LENGTH 1000
#define EDIST_BLANK 0

class BasicEditDistance {
public:
    BasicEditDistance(string from, string to) {
        first = from;
        second = to;

        reset();
    }

    void reset() {
        result = -1;

        for (int i=0; i<255; i++) {
            for (int j=i+1; j<256; j++) {
                setCosts(i, j, !!i + !!j, true);
            }
            setCosts(i, i, 0);
        }
    }

    float getResult() {
        if (!~result) {
            calculate();
        }

        return result;
    }

    void setCosts(char c1, char c2, float value, bool mirror_cost = false) {
        cost[c1][c2] = value;

        if (mirror_cost) {
            cost[c2][c1] = value;
        }
    }

    void calculate() {
        dp[0][0] = 0;
        for (int i=1; i<=first.size(); i++) {
            dp[i][0] = dp[i-1][0] + cost[first[i-1]][EDIST_BLANK];
        }
        for (int i=1; i<=second.size(); i++) {
            dp[0][i] = dp[0][i-1] + cost[EDIST_BLANK][second[i-1]];
        }
        for (int i=1; i<=first.size(); i++) {
            for (int j=1; j<=second.size(); j++) {
                dp[i][j] = min(min(
                                   dp[i-1][j-1] + cost[first[i-1]][second[j-1]], // replace
                                   dp[i-1][j] + cost[first[i-1]][EDIST_BLANK]), // delete
                           dp[i][j-1] + cost[EDIST_BLANK][second[i-1]]); // insert
            }
        }

        result = dp[first.size()][second.size()];
    }

    string first, second;
    int result;
    float cost[256][256]; // EDIST_BLANK is the blank character
    float dp[EDIST_MAX_LENGTH+1][EDIST_MAX_LENGTH+1]; // 0, 0 - empty strings
};

int main() {
    string first = "testing";
    string second = "rofltest";
    BasicEditDistance tester(first, second);
    cout << tester.getResult() << endl;
    return 0;
}
