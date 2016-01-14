#ifndef BASICEDITDISTANCE_HPP
#define BASICEDITDISTANCE_HPP

#define EDIST_MAX_LENGTH 100000
#define EDIST_BLANK 0

#include <string>

using namespace std;

class BasicEditDistance
{
public:
  string first, second;
  int result;
  float cost[256][256];
  float dp[2][EDIST_MAX_LENGTH+1];

  BasicEditDistance(string startingString, string targetString);

  void reset();
  void setCosts(int c1, int c2, float value, bool mirror_cost = false);
  float getResult();
  void calculate();
};

#endif
