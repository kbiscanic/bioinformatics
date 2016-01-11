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

class SubmatrixCalculator{
    public:
        SubmatrixCalculator(int _dimension, int _alphabet){
            this->dimension = _dimension;
            this->alphabet = _alphabet;

            this->initialSteps.reserve(pow(3, _dimension));
            this->initialStrings.reserve(pow(_alphabet, _dimension));
        }

        map<string, string> calculate(){
            generateInitialSteps(0, "");
            generateInitialStrings(0, " ");

            // all possible initial steps and strings combinations
            for (int strA = 0; strA < initialStrings.size(); strA++){
                for (int strB = 0; strB < initialStrings.size(); strB++){
                    for (int stepC = 0; stepC < initialSteps.size(); stepC++){
                        for (int stepD = 0; stepD < initialSteps.size(); stepD++){
                            // TODO calculate results
                        }
                    }
                }
            }

            return this->results;
        }

        x finalSteps(string, string, string, string){
        }

        string stepsToString(vector<int> steps){
            string ret = "";
            ret.reserve(steps.size());
            for (int i = 0; i < steps.size(); i++)
                ret += steps[i] + '1';
            return ret;
        }

        string stepsToPrettyString(string steps){
            string ret = "";
            for (int i = 0; i < steps.size(); i++){
                if (steps[i] == '0')
                    ret += "-1 ";
                else if (steps[i] == '2')
                    ret += "+1 ";
                else
                    ret += "0 ";
            }
            return ret;
        }

        // DEBUG
        void printDebug(){
            for (int i=0; i<initialSteps.size(); i++)
                cout << stepsToPrettyString(initialSteps[i]) << endl;
            for (int i=0; i<initialStrings.size(); i++)
                cout << initialStrings[i] << endl;
        }

    private:
        // TODO: first step has to be 0?
        void generateInitialSteps(int pos, string currStep){
            if (pos == this->dimension){
                initialSteps.push_back(currStep);
                return;
            }

            for (int i=-1; i<=1; i++){
                string tmp = currStep;
                tmp.push_back('1' + i);
                generateInitialSteps(pos + 1, tmp);
            }
        }

         void generateInitialStrings(int pos, string currString){
            if (pos == this->dimension){
                initialStrings.push_back(currString);
                return;
            }

            for (int i=0; i<this->alphabet; i++){
                string tmp = currString;
                tmp.push_back('a' + i);
                generateInitialStrings(pos + 1, tmp);
            }
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
