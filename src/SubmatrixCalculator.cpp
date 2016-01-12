/*
Used to calculate the m-dimension submatrices used in the Four Russians algorithm.
Each submatrix is calculated like a Wagner-Fischer edit matrix with cells as follows:
- (i-1, j-1) to (i, j) with 0 cost added if the characters at (i, j) match, replaceCost otherwise
- (i-1, j) to (i, j) with deleteCost added
- (i, j-1) to (i, j) with insertCost added

Notes:
The strings start with a space because the (0, x) and (x, 0) fields refer to one
string being empty.
The costs are not directly stored, they're converted to step vectors as per the
Four Russians algorithm.
*/

#include <cstdio>
#include <iostream>
#include <vector>
#include <map>
#include <cmath>
#include <string>
#include <cstdlib>

using namespace std;

class SubmatrixCalculator {

public:

    SubmatrixCalculator(int _dimension, string _alphabet, int _replaceCost = 2, int _deleteCost = 1, int _insertCost = 1) {
        this->dimension = _dimension;
        this->alphabet = _alphabet;
        this->replaceCost = _replaceCost;
        this->deleteCost = _deleteCost;
        this->insertCost = _insertCost;

        this->initialSteps.reserve(pow(3, _dimension));
        this->initialStrings.reserve(pow(_alphabet.size(), _dimension));
    }

    void calculate() {
        printf("Clearing the submatrix data...\n");
        results.clear();

        generateInitialSteps(0, "1");
        generateInitialStrings(0, " ");

        printDebug();

        // setting up temporary matrix storage
        lastSubH.reserve(this->dimension + 1);
        lastSubV.reserve(this->dimension + 1);
        vector<int> rowVec(this->dimension + 1, 0);
        for (int i = 0; i <= this->dimension; i++) {
            lastSubH.push_back(rowVec);
            lastSubV.push_back(rowVec);
        }

        // all possible initial steps and strings combinations
        for (int strA = 0; strA < initialStrings.size(); strA++) {
            for (int strB = 0; strB < initialStrings.size(); strB++) {
                for (int stepC = 0; stepC < initialSteps.size(); stepC++) {
                    for (int stepD = 0; stepD < initialSteps.size(); stepD++) {
                        pair<string, string> finalSteps = calculateFinalSteps(initialStrings[strA], initialStrings[strB],
                                                          initialSteps[stepC], initialSteps[stepD]);
                        results[initialStrings[strA] + initialStrings[strB] + initialSteps[stepC] + initialSteps[stepD]] =
                            finalSteps;
                    }
                }
            }
            cout << strA + 1 << " / " << initialStrings.size() << " (submatrices: " << results.size() << " )" << endl;
        }
    }

    void calculateSubmatrix(string strLeft, string strTop, string stepLeft, string stepTop) {
        vector<int> leftSteps = stepsToVector(stepLeft);
        vector<int> topSteps = stepsToVector(stepTop);

        lastSubH[0] = topSteps;
        for (int i = 1; i <= this->dimension; i++) {
            lastSubV[i][0] = leftSteps[i];
        }

     //   cout << "       calculating the matrix" << endl;
        for (int i = 1; i <= this->dimension; i++){
            for (int j = 1; j <= this->dimension; j++){
                int R = (strLeft[i] == strTop[j]) * this->replaceCost;
                lastSubV[i][j] =
                    min(min(R - lastSubH[i - 1][j],
                            this->deleteCost),
                            this->insertCost + lastSubV[i][j - 1] - lastSubH[i - 1][j]);
                lastSubH[i][j] =
                    min(min(R - lastSubV[i][j - 1],
                            this->insertCost),
                            this->deleteCost + lastSubH[i - 1][j] - lastSubV[i][j - 1]);
            }
        }

   /*     cout << "left " << strLeft << " top " << strTop << " stepleft "
        << stepsToPrettyString(stepLeft) << " steptop " << stepsToPrettyString(stepTop) << endl;
        cout << "vertical:" << endl;
        for (int i = 0; i <= this->dimension; i++){
            for (int j = 0; j <= this->dimension; j++)
                cout << stepMatrixV[i][j] << " ";
            cout << endl;
        }
        cout << "horizontal:" << endl;
        for (int i = 0; i <= this->dimension; i++){
            for (int j = 0; j <= this->dimension; j++)
                cout << stepMatrixH[i][j] << " ";
            cout << endl;
        }

        system("pause");
*/
    }

    /*
        Args: left string, top string, left steps, top steps
        Returns: bottom steps, right steps
    */
    pair<string, string> getFinalSteps(string strLeft, string strTop, string stepLeft, string stepTop) {
        return results[strLeft + strTop + stepLeft + stepTop];
    }

    static string stepsToString(vector<int> steps) {
        string ret = "";
        ret.reserve(steps.size());
        for (int i = 0; i < steps.size(); i++) {
            ret += steps[i] + '1';
        }
        return ret;
    }

    static vector<int> stepsToVector(string steps) {
        vector<int> ret;
        ret.reserve(steps.size());
        for (int i = 0; i < steps.size(); i++) {
            ret.push_back(steps[i] - '1');
        }
        return ret;
    }

    static string stepsToPrettyString(string steps) {
        string ret = "";
        for (int i = 0; i < steps.size(); i++) {
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

    pair<string, string> calculateFinalSteps(string strLeft, string strTop, string stepLeft,
            string stepTop) {
    //    cout << "   calculating final steps start" << endl;
        calculateSubmatrix(strLeft, strTop, stepLeft, stepTop);
      //  cout << "   matrix retrieved" << endl;
        vector<int> stepRight(this->dimension + 1, 0);
        for (int i = 1; i <= this->dimension; i++)
            stepRight[i] = lastSubV[i][this->dimension];

        vector<int> stepBot = lastSubH[this->dimension];

        return make_pair(stepsToString(stepRight), stepsToString(stepBot));
    }

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

        for (int i = 0; i < this->alphabet.size(); i++) {
            string tmp = currString;
            tmp.push_back(this->alphabet[i]);
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

private:

    int dimension;
    int replaceCost;
    int deleteCost;
    int insertCost;
    string alphabet;
    vector<string> initialSteps;
    vector<string> initialStrings;
    vector<vector<int> > lastSubH, lastSubV;
    map<string, pair<string, string> > results;

};

int main() {
    SubmatrixCalculator calc(3, "ATGC");
    calc.calculate();

    return 0;
}
