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

        int startTime = clock();
        this->resultIndex = new pair<string, string>[this->submatrixCountLimit];
        cout << "Allocation time: " << (clock()-startTime)/double(CLOCKS_PER_SEC) << "s" << endl;
    }

    void calculate() {
        printf("Clearing the submatrix data...\n");
        results.clear();

        generateInitialSteps(0, "1");
        generateInitialStrings(0, " ");

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
            cout << strA + 1 << " / " << initialStrings.size() << " (submatrices: "
                << (strA + 1) * initialStrings.size() * initialSteps.size() * initialSteps.size() << " )" << endl;
        }
        cout << "Submatrix calculation time: " << (clock()-startTime)/double(CLOCKS_PER_SEC) << "s" << endl;
    }

    const long long HASH_BASE = 137;
    inline long long hash(string x){
        long long ret = 0;
        for (int i = 0; i < x.size(); i++){
            ret = (ret * HASH_BASE + x[i]);
        }
        ret %= this->submatrixCountLimit;
        if (ret < 0){
            ret += this->submatrixCountLimit;
        }
        return ret;
    }

    inline int mmin(int x, int y, int z){
        return x>y?(y<z?y:z):x<z?x:z;
    }

    /*
        Calculates the step-matrix determined by the two strings and two initial vectors provided.
        The step matrix has two parts, vertical and horizontal steps, stored in lastSubV and lastSubH.
    */
    inline void calculateSubmatrix(string strLeft, string strTop, string stepLeft, string stepTop) {
        /*
            TODO: CHECK: MIGHT BE WRONG AFTER TRANSPOSING
            have to transpose to keep cache functionality, huge speedup;
            old/new way commented
        */
        for (int i = 1; i <= this->dimension; i++) {
            //lastSubV[i][0] = stepLeft[i] - '1'; // old
            lastSubV[0][i] = stepLeft[i] - '1'; // new
            lastSubH[0][i] = stepTop[i] - '1';
        }

        for (int i = 1; i <= this->dimension; i++){
            for (int j = 1; j <= this->dimension; j++){
                int R = (strLeft[i] == strTop[j]) * this->replaceCost;
                //int lastV = lastSubV[i][j - 1]; // old
                int lastV = lastSubV[i - 1][j]; // new
                int lastH = lastSubH[i - 1][j];
                /*lastSubV[i][j] =
                    min(min(R - lastH,
                            this->deleteCost),
                            this->insertCost + lastV - lastH);
                lastSubH[i][j] =
                    min(min(R - lastV,
                            this->insertCost),
                            this->deleteCost + lastH - lastV);*/
                lastSubV[i][j] =
                    mmin(R - lastH, this->deleteCost, this->insertCost + lastV - lastH);
                lastSubH[i][j] =
                    mmin(R - lastV, this->insertCost, this->deleteCost + lastH - lastV);
            }
        }

/*      // DEBUG
        cout << "left " << strLeft << " top " << strTop << " stepleft "
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

    pair<string, string> calculateFinalSteps(string strLeft, string strTop, string stepLeft,
            string stepTop) {
        calculateSubmatrix(strLeft, strTop, stepLeft, stepTop);
        /*
            // WARNING: check calculateSubmatrix() comments
            // old
        vector<int> stepRight(this->dimension + 1, 0);
        for (int i = 1; i <= this->dimension; i++){
            //stepRight[i] = lastSubV[i][this->dimension];
        }
        */
        vector<int> stepRight = lastSubV[this->dimension];
        vector<int> stepBot = lastSubH[this->dimension];

        return make_pair(stepsToString(stepRight), stepsToString(stepBot));
    }

    /*
        Args: left string, top string, left steps, top steps
        Returns: bottom steps, right steps
    */
    inline pair<string, string> getFinalSteps(string strLeft, string strTop, string stepLeft, string stepTop) {
        //return results[hash(strLeft + strTop + stepLeft + stepTop)];
        return resultIndex[hash(strLeft + strTop + stepLeft + stepTop)];
    }

    /*
        Transforms the step vector to a string. The string characters have no special meaning,
        strings are used for easier mapping.
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
    // the actual maximum number of submatrices should be much lower
    // in order to decrease the possibility of hash collisions
    // when storing results
    const long long submatrixCountLimit = 15485863; // prime, used to % hashes
    string alphabet;
    vector<string> initialSteps;
    vector<string> initialStrings;
    vector<vector<int> > lastSubH, lastSubV;
    map<long long, pair<string, string> > results;
    pair<string, string>* resultIndex;

};

int main() {
    SubmatrixCalculator calc(3, "ATGC");
    calc.calculate();

    return 0;
}
