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
        return resultIndex[getOffset(strLeft, strTop, stepLeft, stepTop)];
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

    int getOffset(string strLeft, string strTop, string stepLeft, string stepTop) {
        int offset = 0;

        for (unsigned int i = 0; i < this->dimension; i++) {
            offset += charLeftOffset[i][alphabetMap[strLeft[i]]];
            offset += charTopOffset[i][alphabetMap[strTop[i]]];
            offset += stepLeftOffset[i][stepLeft[i] - '0'];
            offset += stepTopOffset[i][stepTop[i] - '0'];
        }

        return offset;
    }

    void calculateOffsets() {
        // calculate the memory offsets for submatrix storage
        int temp_offset = 1;
        for (int i = 0; i < this->dimension; i++) {
            for (int j = 0; j < 3; j++) {
                stepTopOffset[i].push_back(j * temp_offset);
            }
            temp_offset *= 3;
        }
        for (int i = 0; i < this->dimension; i++) {
            for (int j = 0; j < 3; j++) {
                stepLeftOffset[i].push_back(j * temp_offset);
            }
            temp_offset *= 3;
        }
        for (int i = 0; i < this->dimension; i++) {
            for (unsigned int j = 0; j <= this->alphabet.size(); j++) {
                charTopOffset[i].push_back(j * temp_offset);
            }
            temp_offset *= (this->alphabet.size() + 1);
        }
        for (int i = 0; i < this->dimension; i++) {
            for (unsigned int j = 0; j <= this->alphabet.size(); j++) {
                charLeftOffset[i].push_back(j * temp_offset);
            }
            temp_offset *= (this->alphabet.size() + 1);
        }
        /*
                        // DEBUG address offset printing
                        for (int j = this->dimension - 1; j >= 0; j--) {
                            for (int i = 0; i < charLeftOffset[j].size(); i++)
                                cout << charLeftOffset[j][i] << " ";
                            cout << "   ";
                        }
                        cout << endl << endl;

                        for (int j = this->dimension - 1; j >= 0; j--) {
                            for (int i = 0; i < charTopOffset[j].size(); i++)
                                cout << charTopOffset[j][i] << " ";
                            cout << "   ";
                        }
                        cout << endl << endl;

                        for (int j = this->dimension - 1; j >= 0; j--) {
                            for (int i = 0; i < stepLeftOffset[j].size(); i++)
                                cout << stepLeftOffset[j][i] << " ";
                            cout << "   ";
                        }
                        cout << endl << endl;

                        for (int j = this->dimension - 1; j >= 0; j--) {
                            for (int i = 0; i < stepTopOffset[j].size(); i++)
                                cout << stepTopOffset[j][i] << " ";
                            cout << "   ";
                        }
                        cout << endl << endl;
        */
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
    map<char, int> alphabetMap;
    char blankCharacter;
    vector<string> initialSteps;
    vector<string> initialStrings;
    vector<vector<int> > lastSubH, lastSubV;
    vector<int> charTopOffset[3];
    vector<int> charLeftOffset[3];
    vector<int> stepTopOffset[3];
    vector<int> stepLeftOffset[3];
    pair<string, string>* resultIndex;
    double times[2];  // allocation time, matrix calculation time
};
#endif
