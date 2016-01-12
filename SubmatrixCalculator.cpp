/*
Used to calculate the m-dimension submatrices used in the Four Russians algorithm.
Each submatrix is calculated like a Wagner-Fischer edit matrix with cells as follows:
- (i-1, j-1) to (i, j) with +1 score added if the characters at (i, j) match, 0 otherwise
- (i-1, j) to (i, j) with -1 score added
- (i, j-1) to (i, j) with -1 score added

Notes:
The strings start with a space because the (0, x) and (x, 0) fields refer to one
string being empty.
The scores are not directly stored, they're converted to step vectors as per the
Four Russians algorithm.
*/

#include <cstdio>
#include <iostream>
#include <vector>
#include <map>
#include <cmath>

using namespace std;

class SubmatrixCalculator {

public:

    SubmatrixCalculator(int _dimension, int _alphabet) {
        this->dimension = _dimension;
        this->alphabet = _alphabet;

        this->initialSteps.reserve(pow(3, _dimension));
        this->initialStrings.reserve(pow(_alphabet, _dimension));
    }

    void calculate() {
        printf("Clearing the submatrix data...\n");
        results.clear();

        generateInitialSteps(0, "");
        generateInitialStrings(0, " ");

        // all possible initial steps and strings combinations
        for (int strA = 0; strA < initialStrings.size(); strA++) {
            for (int strB = 0; strB < initialStrings.size(); strB++) {
                for (int stepC = 0; stepC < initialSteps.size(); stepC++) {
                    for (int stepD = 0; stepD < initialSteps.size(); stepD++) {
                        pair<string, string> finalSteps = calculateFinalSteps(initialStrings[strA], initialStrings[strB],
                                                          initialSteps[stepC], initialSteps[stepD]);
                        results[initialStrings[strA] + initialStrings[strB] + initialSteps[stepC] + initialSteps[stepD]] =
                            finalSteps.first + finalSteps.second;
                    }
                }
            }
        }
    }

    vector<int, int> calculateSubmatrix(string strLeft, string strTop, string stepLeft, string stepTop){
        vector<int> leftSteps = stepsToVector(stepLeft);
        vector<int> topSteps = stepsToVector(stepTop);
        //TODO
    }

    /*
        Args: left string, top string, left steps, top steps
        Returns: bottom steps, right steps
    */
    pair<string, string> getFinalSteps(string strLeft, string strTop, string stepLeft, string stepTop) {
        string resultingSteps = results[strLeft + strTop + stepLeft + stepTop];
        return make_pair(
                   resultingSteps.substr(0, resultingSteps.size() / 2),
                   resultingSteps.substr(resultingSteps.size() / 2, npos));
    }

    static string stepsToString(vector<int> steps) {
        string ret = "";
        ret.reserve(steps.size());
        for (int i = 0; i < steps.size(); i++)
            ret += steps[i] + '1';
        return ret;
    }

    static vector<int> stepsToVector(string steps){
        vector<int> ret;
        ret.reserve(steps.size());
        for (int i = 0; i < steps.size(); i++)
            ret.push_back(steps[i] - '1');
        return ret;
    }

    static string stepsToPrettyString(string steps) {
        string ret = "";
        for (int i = 0; i < steps.size(); i++) {
            if (steps[i] == '0')
                ret += "-1 ";
            else if (steps[i] == '2')
                ret += "+1 ";
            else
                ret += "0 ";
        }
        return ret;
    }

private:

    pair<string, string> calculateFinalSteps(string strLeft, string strTop, string stepLeft,
            string stepTop) {
        vector<int, int> stepMatrix = calculateSubmatrix(strLeft, strTop, stepLeft, stepTop);

        vector<int> stepBot;
        stepBot.reserve(this->dimension);
        for (int i = 0; i < this->dimension; i++)
            stepBot.push_back(stepMatrix[this->dimension - 1][i]);

        vector<int> stepRight;
        stepRight.reserve(this->dimension);
        for (int i = 0; i < this->dimension; i++)
            stepRight.push_back(stepMatrix[i][this->dimension - 1]);

        return make_pair(stepsToString(stepBot), stepsToString(stepRight));
    }

    // TODO: first step has to be 0?
    void generateInitialSteps(int pos, string currStep) {
        if (pos == this->dimension) {
            initialSteps.push_back(currStep);
            return;
        }

        for (int i = -1; i <= 1; i++) {
            string tmp = currStep;
            tmp.push_back('1' + i);
            generateInitialSteps(pos + 1, tmp);
        }
    }

    void generateInitialStrings(int pos, string currString) {
        if (pos == this->dimension) {
            initialStrings.push_back(currString);
            return;
        }

        for (int i = 0; i < this->alphabet; i++) {
            string tmp = currString;
            tmp.push_back('a' + i);
            generateInitialStrings(pos + 1, tmp);
        }
    }

    // DEBUG
    void printDebug() {
        for (int i = 0; i < initialSteps.size(); i++)
            cout << stepsToPrettyString(initialSteps[i]) << endl;
        for (int i = 0; i < initialStrings.size(); i++)
            cout << initialStrings[i] << endl;
    }

    int dimension;
    int alphabet; // ['a', 'a' + alphabet>
    vector<string> initialSteps;
    vector<string> initialStrings;
    map<string, string> results;

};

int main() {
    SubmatrixCalculator calc(4, 4);
    calc.calculate();
    //calc.printDebug();
    return 0;
}
