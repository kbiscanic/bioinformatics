#ifndef SUBMATRIXCALC_HPP
#define SUBMATRIXCALC_HPP

#include <cstdio>
#include <iostream>
#include <vector>
#include <map>
#include <cmath>
#include <string>
#include <cstdlib>
#include <ctime>
#include <numeric>

using namespace std;

class SubmatrixCalculator {
 public:
  SubmatrixCalculator();
  SubmatrixCalculator(int _dimension, string _alphabet = "ATGC",
                      char _blankCharacter = '-', int _replaceCost = 2,
                      int _deleteCost = 1, int _insertCost = 1);
  ~SubmatrixCalculator();
  void calculate();
  pair<vector<int>, pair<pair<int, int>, pair<int, int> > > getSubmatrixPath(
      string strLeft, string strTop, string stepLeft, string stepTop,
      int finalRow, int finalCol, int initialCost);
  void calculateCostSubmatrix(string strLeft, string strTop, string stepLeft,
                              string stepTop, int initialCost);
  inline void calculateSubmatrix(string strLeft, string strTop, string stepLeft,
                                 string stepTop);
  pair<string, string> calculateFinalSteps(string strLeft, string strTop,
                                           string stepLeft, string stepTop);
  void generateInitialSteps(int pos, string currStep);
  void generateInitialStrings(int pos, string currString, bool blanks);
  void printDebug();
  inline int mmin(int x, int y, int z);

  /*
       Returns the sum of the step values for a given step string.
   */
  inline int sumSteps(string steps) {
    vector<int> stepValues = stepsToVector(steps);
    return accumulate(stepValues.begin(), stepValues.end(), 0);
  }

  /*
      Returns the precalculated final step vectors for a given initial submatrix
     description.
  */
  inline pair<string, string> getFinalSteps(string strLeft, string strTop,
                                            string stepLeft, string stepTop) {
    return results[hash(strLeft + strTop + stepLeft + stepTop)];
  }

  /*
      Transforms the step vector to a string. The string characters have no
     special meaning,
      strings are used for easier mapping. Check
     SubmatrixCalculator::stepsToVector comments.
  */
  static string stepsToString(vector<int> steps) {
    string ret = "";
    ret.reserve(steps.size());
    for (unsigned int i = 0; i < steps.size(); i++) {
      ret += steps[i] + '1';
    }
    return ret;
  }

  /*
      Adds spaces and signs to the step string and transforms the values to real
      step values.
  */
  string stepsToPrettyString(string steps) {
    string ret = "";
    for (unsigned int i = 0; i < steps.size(); i++) {
      if (steps[i] == '0') {
        ret += "-1 ";
      } else if (steps[i] == '2') {
        ret += "+1 ";
      } else {
        ret += "0 ";
      }
    }
    return ret;
  }

  int HASH_BASE = 50077;
  inline long long hash(string x) {
    long long ret = 0;
    for (unsigned int i = 0; i < x.size(); i++) {
      ret = (ret * HASH_BASE + x[i]);
    }
    // ret &= this->submatrixCountLimit - 1;  // fancy bit-work
    return ret;
  }

  /*
      Transforms the string of steps (possibly a result of stepsToString()) to a
     vector of steps,
      where each step value is expected to be in {-1, 0, 1}. I.e., the steps
     string characters
      are expected to be in {'0', '1', '2'}.
  */
  static vector<int> stepsToVector(string steps) {
    vector<int> ret;
    ret.reserve(steps.size());
    for (unsigned int i = 0; i < steps.size(); i++) {
      ret.push_back(steps[i] - '1');
    }
    return ret;
  }

 private:
  int dimension;
  int replaceCost;
  int deleteCost;
  int insertCost;
  // the actual maximum number of submatrices should be much lower
  // in order to decrease the possibility of hash collisions
  // when storing results
  string alphabet;
  char blankCharacter;
  vector<string> initialSteps;
  vector<string> initialStrings;
  vector<vector<int> > lastSubH, lastSubV;
  map<long long, pair<string, string> > results;
  double times[2];  // allocation time, matrix calculation time
};
#endif
