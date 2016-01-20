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
                        char _blankCharacter = '-', int _replaceCost = 1,
                        int _deleteCost = 1, int _insertCost = 1);
    ~SubmatrixCalculator();
    void calculate();
    pair<vector<int>, pair<pair<int, int>, pair<int, int> > > getSubmatrixPath(
        string strLeft, string strTop, int stepLeft, int stepTop,
        int finalRow, int finalCol, int initialCost);
    void calculateCostSubmatrix(string strLeft, string strTop, int stepLeft,
                                int stepTop, int initialCost);
    inline void calculateSubmatrix(string strLeft, string strTop, string stepLeft,
                                   string stepTop);
    pair<int, int> calculateFinalSteps(string strLeft, string strTop,
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
       Unused to reduce function overhead.
    */
   /* inline pair<string, string> getFinalSteps(string strLeft, string strTop,
            string stepLeft, string stepTop) {
        return resultIndex[getOffset(strLeft, strTop, stepLeft, stepTop)];
    }*/

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
        Returns the total memory offset for the matrix represented by the four
        provided parameters (initial step vectors and initial strings).
    */
    inline int getOffset(string strLeft, string strTop, string stepLeft, string stepTop) {
        int offset = 0;

        for (int i = 0; i < this->dimension; i++) {
            offset += charLeftOffset[this->dimension - i - 1][alphabetMap[strLeft[i]]];
            offset += charTopOffset[this->dimension - i - 1][alphabetMap[strTop[i]]];
            offset += stepLeftOffset[this->dimension - i - 1][stepLeft[i] - '0'];
            offset += stepTopOffset[this->dimension - i - 1][stepTop[i] - '0'];
        }

        return offset;
    }

    /*
        Precalculates some offsets for step and string indexing. Saves time
        during any bottleneck involving submatrix retrieval / storage.
    */
    void calculateOffsets() {
        // calculate the memory offsets for submatrix storage for each individual symbol
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

        if (initialSteps.size() == 0){
            cout << "Missing initial steps." << endl;
            return;
        }

        // setting the offset index length
        int stepOffsetMaxAddress = 0;
        for (int i = 0; i < this->dimension; i++){
            stepOffsetMaxAddress = (stepOffsetMaxAddress * 10) + 2;
        }
        stepOffsets[0].resize(++stepOffsetMaxAddress); // left steps
        stepOffsets[1].resize(++stepOffsetMaxAddress); // top steps

        /*// DEBUG step offset locations
        cout << "Reserved (2x) " << stepOffsetMaxAddress << " step offset locations." << endl;*/

        for (unsigned int i = 0; i < initialSteps.size(); i++){
            int stepNumeric = 0;
            int offsetLeft = 0;
            int offsetTop = 0;
            for (int j = 0; j < this->dimension; j++){
                stepNumeric *= 10;
                stepNumeric += initialSteps[i][j] - '0';
                offsetLeft += stepLeftOffset[this->dimension - j - 1][initialSteps[i][j] - '0'];
                offsetTop += stepTopOffset[this->dimension - j - 1][initialSteps[i][j] - '0'];
            }
            stepOffsets[0][stepNumeric] = offsetLeft;
            stepOffsets[1][stepNumeric] = offsetTop;

            /*// DEBUG
            cout << i << ": " << initialSteps[i] << " " << stepNumeric << " -> " << offsetLeft << " " << offsetTop << endl;*/
        }

        /*// DEBUG address offset printing
        for (int j = this->dimension - 1; j >= 0; j--) {
            for (unsigned int i = 0; i < charLeftOffset[j].size(); i++)
                cout << charLeftOffset[j][i] << " ";
            cout << "   ";
        }
        cout << endl << endl;

        for (int j = this->dimension - 1; j >= 0; j--) {
            for (unsigned int i = 0; i < charTopOffset[j].size(); i++)
                cout << charTopOffset[j][i] << " ";
            cout << "   ";
        }
        cout << endl << endl;

        for (int j = this->dimension - 1; j >= 0; j--) {
            for (unsigned int i = 0; i < stepLeftOffset[j].size(); i++)
                cout << stepLeftOffset[j][i] << " ";
            cout << "   ";
        }
        cout << endl << endl;

        for (int j = this->dimension - 1; j >= 0; j--) {
            for (unsigned int i = 0; i < stepTopOffset[j].size(); i++)
                cout << stepTopOffset[j][i] << " ";
            cout << "   ";
        }
        cout << endl << endl;
        */
    }

    /*
        Sums the step values encoded in stepsNumeric to get the
        total difference over that step vector.
    */
    int sumSteps(int stepsNumeric){
        int sum = 0;
        for (int i = 0; i < this->dimension; i++){
            sum += stepsNumeric % 10 - 1;
            stepsNumeric /= 10;
        }
        return sum;
    }

    /*
        Transforms a vector of steps to a single integer. Used for
        addressing using steps as indices.
    */
    static int stepsToInt(vector<int> steps){
        int ret = 0;
        for (unsigned int i = 0; i < steps.size(); i++){
            ret = ret * 10 + steps[i] + 1;
        }
        return ret;
    }

    /*
        Transforms the steps encoded as an integer to a vector of step values.
    */
    vector<int> stepsToVector(int steps){
        vector<int> rev;
        for (int i = 0; i < this->dimension; i++){
            rev.push_back(steps % 10 - 1);
            steps /= 10;
        }

        vector<int> ret;
        for (int i = rev.size() - 1; i >= 0; i--){
            ret.push_back(rev[i]);
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

    pair<int, int>* resultIndex;
    // mapping of step vectors to offsets; 0 - left, 1 - top
    vector<int> stepOffsets[2];
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

    // memory offsets; used when storing/loading submatrices
    vector<int> charLeftOffset[3];
    vector<int> charTopOffset[3];
    vector<int> stepLeftOffset[3];
    vector<int> stepTopOffset[3];

    // allocation time, matrix calculation time
    double times[2];

    // temporary matrices
    vector<vector<int> > lastSubH, lastSubV;
};
#endif
