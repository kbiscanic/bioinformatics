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
#include <ctime>
#include <numeric>

using namespace std;

class SubmatrixCalculator {

public:

    SubmatrixCalculator(int _dimension, string _alphabet = "ATGC", char _blankCharacter = '-',
                        int _replaceCost = 2,
                        int _deleteCost = 1,
                        int _insertCost = 1) {
        this->dimension = _dimension;
        this->alphabet = _alphabet;
        this->blankCharacter = _blankCharacter;
        this->replaceCost = _replaceCost;
        this->deleteCost = _deleteCost;
        this->insertCost = _insertCost;

        this->initialSteps.reserve(pow(3, _dimension));
        this->initialStrings.reserve(pow(_alphabet.size(), _dimension));

        int startTime = clock();
        this->resultIndex = new pair<string, string>[this->submatrixCountLimit];
        this->times[0] = (clock()-startTime)/double(CLOCKS_PER_SEC);
        cout << "Allocation time: " << this->times[0] << "s" << endl;
    }

    void calculate() {
        printf("Clearing the submatrix data...\n");
        results.clear();

        generateInitialSteps(0, "");
        generateInitialStrings(0, "", false);

        //printDebug();

        // setting up temporary matrix storage
        lastSubH.reserve(this->dimension + 1);
        lastSubV.reserve(this->dimension + 1);
        vector<int> rowVec(this->dimension + 1, 0);
        for (int i = 0; i <= this->dimension; i++) {
            lastSubH.push_back(rowVec);
            lastSubV.push_back(rowVec);
        }

        // all possible initial steps and strings combinations
        int startTime = clock();
        for (int strA = 0; strA < initialStrings.size(); strA++) {
            for (int strB = 0; strB < initialStrings.size(); strB++) {
                for (int stepC = 0; stepC < initialSteps.size(); stepC++) {
                    for (int stepD = 0; stepD < initialSteps.size(); stepD++) {
                        string key = initialStrings[strA];
                        key += initialStrings[strB];
                        key += initialSteps[stepC];
                        key += initialSteps[stepD];
                        // storing the resulting final rows for future reference
                        resultIndex[hash(key)] = calculateFinalSteps(initialStrings[strA], initialStrings[strB],
                                                 initialSteps[stepC], initialSteps[stepD]);
                        //result[hash(key)] = calculateFinalSteps(initialStrings[strA], initialStrings[strB],
                        //                                  initialSteps[stepC], initialSteps[stepD]);
                    }
                }
            }
            //cout << strA + 1 << " / " << initialStrings.size() << " (submatrices: " << results.size() << " )" << endl;
            if (strA % 5 == 0 or strA == initialStrings.size() - 1) {
                cout << strA + 1 << " / " << initialStrings.size() << " (submatrices: "
                     << (strA + 1) * initialStrings.size() * initialSteps.size() * initialSteps.size() << " )" << endl;
            }
        }
        this->times[1] = (clock()-startTime)/double(CLOCKS_PER_SEC);
        cout << "Submatrix calculation time: " << this->times[1] << "s" << endl;
    }

    /*
        Backtracks through the submatrix, represented by the standard 4 provided strings, until it
        leaves the bounds and enters another submatrix. Returns the coordinates of the next submatrix
        entered, the entry cell in the next submatrix, and a vector of operations used to traverse
        the current matrix.
        The initialCost parameter combined with the two initial step vectors are used to calculate
        the edit distance submatrix because it's easier to backtrack through a cost-matrix than
        through a step matrix.
    */
    pair<vector<int>, pair<pair<int, int>, pair<int, int> > > getSubmatrixPath(string strLeft, string strTop,
            string stepLeft, string stepTop, int finalRow, int finalCol, int initialCost) {

        calculateCostSubmatrix(strLeft, strTop, stepLeft, stepTop, initialCost);

        int i = finalRow;
        int j = finalCol;
        vector<int> operations;
        // backtracking - for movement check SubmatrixCalculator::calculateCostSubmatrix
        while (i > 0 && j > 0) {
            operations.push_back(lastSubH[i][j]);
            if (operations[operations.size() - 1] == 1) {
                i--;
            } else if (operations[operations.size() - 1] == 2) {
                j--;
            } else {
                i--;
                j--;
            }
        }

        // calculate the next matrix to enter depending on the initial vector we ended up on
        // if i == 0, go left, if j == 0 go up, if both are 0 go diagonally up-left
        pair<int, int> nextMatrix;
        pair<int, int> nextCell;
        if (i == 0 && j == 0) {
            nextMatrix = make_pair(-1, -1);
            nextCell = make_pair(this->dimension, this->dimension);
        } else if (i == 0) {
            nextMatrix = make_pair(-1, 0);
            nextCell = make_pair(this->dimension, j);
        } else {
            nextMatrix = make_pair(0, -1);
            nextCell = make_pair(i, this->dimension);
        }

        return make_pair(operations, make_pair(nextMatrix, nextCell));
    }

    /*
         Calculates the cost submatrix represented by the provided two strings and two initial vectors.
         The cost matrix has two parts, stored in lastSubV and lastSubH (recycled matrices to save space).
         The lastSubV matrix stores the costs and the lastSubH matrix stores the optimal paths.
         Path codes inside lastSubH:
         0 - initial vector cell, exiting the submatrix
         1 - moving up in the submatrix (deleting)
         2 - moving left in the submatrix (inserting)
         3 - moving diagonally up-left in the submatrix (replacing / matching)
    */
    void calculateCostSubmatrix(string strLeft, string strTop, string stepLeft, string stepTop,
                                int initialCost) {
        lastSubV[0][0] = initialCost;
        lastSubH[0][0] = 0;
        for (int i = 1; i <= this->dimension; i++) {
            lastSubV[0][i] = lastSubV[0][i - 1] + stepTop[i - 1] - '1';
            lastSubV[i][0] = lastSubV[i - 1][0] + stepLeft[i - 1] - '1';
            lastSubH[0][i] = 0;
            lastSubH[i][0] = 0;
        }

        for (int i = 1; i <= this->dimension; i++) {
            for (int j = 1; j <= this->dimension; j++) {
                int R = (strLeft[i - 1] != strTop[j - 1]) * this->replaceCost;
                if (strLeft[i - 1] == blankCharacter or strTop[j - 1] == blankCharacter)
                    R = (strLeft[i - 1] != blankCharacter) * this->deleteCost
                        + (strTop[j - 1] != blankCharacter) * this->insertCost;

                // replace
                lastSubV[i][j] = lastSubV[i - 1][j - 1] + R;
                lastSubH[i][j] = 3;

                // insert
                int alternative = lastSubV[i][j - 1] + this->insertCost * (strTop[j - 1] != blankCharacter);
                if (lastSubV[i][j] > alternative) {
                    lastSubV[i][j] = alternative;
                    lastSubH[i][j] = 2;
                }

                // delete
                alternative = lastSubV[i - 1][j] + this->deleteCost * (strLeft[i - 1] != blankCharacter);
                if (lastSubV[i][j] > alternative) {
                    lastSubV[i][j] = alternative;
                    lastSubH[i][j] = 1;
                }
            }
        }
    }

    /*
        Calculates the step submatrix represented by the provided two strings and two initial vectors.
        The step matrix has two parts, vertical and horizontal steps, stored in lastSubV and lastSubH.
    */
    inline void calculateSubmatrix(string strLeft, string strTop, string stepLeft, string stepTop) {
        for (int i = 1; i <= this->dimension; i++) {
            lastSubV[i][0] = stepLeft[i - 1] - '1';
            lastSubH[0][i] = stepTop[i - 1] - '1';
        }
        lastSubV[0][0] = lastSubH[0][0] = 0;

        for (int i = 1; i <= this->dimension; i++) {
            for (int j = 1; j <= this->dimension; j++) {
                int R = (strLeft[i - 1] != strTop[j - 1]) * this->replaceCost;
                if (strLeft[i - 1] == blankCharacter or strTop[j - 1] == blankCharacter)
                    R = (strLeft[i - 1] != blankCharacter) * this->deleteCost
                        + (strTop[j - 1] != blankCharacter) * this->insertCost;
                int lastV = lastSubV[i][j - 1];
                int lastH = lastSubH[i - 1][j];
                int insCost = this->insertCost * (strTop[j - 1] != blankCharacter);
                int delCost = this->deleteCost * (strLeft[i - 1] != blankCharacter);
                lastSubV[i][j] =
                    mmin(R - lastH, delCost, insCost + lastV - lastH);
                lastSubH[i][j] =
                    mmin(R - lastV, insCost, delCost + lastH - lastV);
              /*  if (strLeft == "A-" && strTop == "G-" && stepLeft == "21" && stepTop == "21"){
                    cout << "lastv " << lastV << " lasth " << lastH << " R " << R << endl;
                    cout << R - lastH << " " << this->deleteCost << " " << this->insertCost + lastV - lastH << endl;
                    cout << R - lastV << " " << this->insertCost << " " << this->deleteCost + lastH - lastV << endl;
                }*/
            }
        }/*
        if (strLeft == "A-" && strTop == "G-" && stepLeft == "21" && stepTop == "21"){
            // DEBUG
            cout << "left " << strLeft << " top " << strTop << " stepleft "
                 << stepsToPrettyString(stepLeft) << " steptop " << stepsToPrettyString(stepTop) << endl;
            cout << "vertical:" << endl;
            for (int i = 0; i <= this->dimension; i++) {
                for (int j = 0; j <= this->dimension; j++)
                    cout << lastSubV[i][j] << " ";
                cout << endl;
            }
            cout << "horizontal:" << endl;
            for (int i = 0; i <= this->dimension; i++) {
                for (int j = 0; j <= this->dimension; j++)
                    cout << lastSubH[i][j] << " ";
                cout << endl;
            }
            system("pause");
        }*/
    }

    /*
        Calculates the final step vectors for a given initial submatrix description.
    */
    pair<string, string> calculateFinalSteps(string strLeft, string strTop, string stepLeft,
            string stepTop) {
        calculateSubmatrix(strLeft, strTop, stepLeft, stepTop);

        vector<int> stepRight(this->dimension + 1, 0);
        for (int i = 1; i <= this->dimension; i++){
            stepRight[i] = lastSubV[i][this->dimension];
        }

        vector<int> stepBot = lastSubH[this->dimension];

        return make_pair(stepsToString(stepRight), stepsToString(stepBot));
    }

    /*
        Returns the precalculated final step vectors for a given initial submatrix description.
    */
    inline pair<string, string> getFinalSteps(string strLeft, string strTop, string stepLeft,
            string stepTop) {
        //return results[hash(strLeft + strTop + stepLeft + stepTop)];
        return resultIndex[hash(strLeft + strTop + stepLeft + stepTop)];
    }

    /*
        Returns the sum of the step values for a given step string.
    */
    inline int sumSteps(string steps) {
        vector<int> stepValues = stepsToVector(steps);
        return accumulate(stepValues.begin(), stepValues.end(), 0);
    }

    /*
        Transforms the step vector to a string. The string characters have no special meaning,
        strings are used for easier mapping. Check SubmatrixCalculator::stepsToVector comments.
    */
    static string stepsToString(vector<int> steps) {
        string ret = "";
        ret.reserve(steps.size());
        for (int i = 0; i < steps.size(); i++) {
            ret += steps[i] + '1';
        }
        return ret;
    }

    /*
        Transforms the string of steps (possibly a result of stepsToString()) to a vector of steps,
        where each step value is expected to be in {-1, 0, 1}. I.e., the steps string characters
        are expected to be in {'0', '1', '2'}.
    */
    static vector<int> stepsToVector(string steps) {
        vector<int> ret;
        ret.reserve(steps.size());
        for (int i = 0; i < steps.size(); i++) {
            ret.push_back(steps[i] - '1');
        }
        return ret;
    }

    /*
        Adds spaces and signs to the step string and transforms the values to real step values.
    */
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

    void generateInitialStrings(int pos, string currString, bool blanks) {
        if (pos == this->dimension) {
            initialStrings.push_back(currString);
            return;
        }

        if (!blanks) {
            for (int i = 0; i < this->alphabet.size(); i++) {
                string tmp = currString;
                tmp.push_back(this->alphabet[i]);
                generateInitialStrings(pos + 1, tmp, false);
            }
        }

        string tmp = currString;
        tmp.push_back(blankCharacter);
        generateInitialStrings(pos + 1, tmp, true);
    }

    // DEBUG
    void printDebug() {
        for (int i = 0; i < initialSteps.size(); i++)
            cout << stepsToPrettyString(initialSteps[i]) << endl;
        for (int i = 0; i < initialStrings.size(); i++)
            cout << initialStrings[i] << endl;
    }

    const long long HASH_BASE = 137;
    inline long long hash(string x) {
        long long ret = 0;
        for (int i = 0; i < x.size(); i++) {
            ret = (ret * HASH_BASE + x[i]);
        }
        ret &= this->submatrixCountLimit - 1; // fancy bit-work
        return ret;
    }

    inline int mmin(int x, int y, int z) {
        return x>y?(y<z?y:z):x<z?x:z;
    }

private:

    int dimension;
    int replaceCost;
    int deleteCost;
    int insertCost;
    // the actual maximum number of submatrices should be much lower
    // in order to decrease the possibility of hash collisions
    // when storing results
    const long long submatrixCountLimit = 16777216; // 2^24, coprime with hash base
    string alphabet;
    char blankCharacter;
    vector<string> initialSteps;
    vector<string> initialStrings;
    vector<vector<int> > lastSubH, lastSubV;
    map<long long, pair<string, string> > results;
    pair<string, string>* resultIndex;
    double times[2]; // allocation time, matrix calculation time
};

/*int main() {
    SubmatrixCalculator calc(3);
    calc.calculate();
    return 0;
}*/
