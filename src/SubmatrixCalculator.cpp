/*
Used to calculate the m-dimension submatrices used in the Four Russians
algorithm.
Each submatrix is calculated like a Wagner-Fischer edit matrix with cells as
follows:
- (i-1, j-1) to (i, j) with 0 cost added if the characters at (i, j) match,
replaceCost otherwise
- (i-1, j) to (i, j) with deleteCost added
- (i, j-1) to (i, j) with insertCost added

Notes:
The strings start with a space because the (0, x) and (x, 0) fields refer to one
string being empty.
The costs are not directly stored, they're converted to step vectors as per the
Four Russians algorithm.
*/

#include "SubmatrixCalculator.hpp"

SubmatrixCalculator::SubmatrixCalculator() {}
SubmatrixCalculator::~SubmatrixCalculator() {
    delete this->resultIndex;
}

SubmatrixCalculator::SubmatrixCalculator(int _dimension, string _alphabet,
                                         char _blankCharacter, int _replaceCost,
                                         int _deleteCost, int _insertCost) {
  this->dimension = _dimension;
  this->alphabet = _alphabet;
  this->blankCharacter = _blankCharacter;
  this->replaceCost = _replaceCost;
  this->deleteCost = _deleteCost;
  this->insertCost = _insertCost;

  // map the provided alphabet to indices to simplify addressing
  for (unsigned int i = 0; i < this->alphabet.size(); i++){
    this->alphabetMap[this->alphabet[i]] = i;
  }
  this->alphabetMap[this->blankCharacter] = this->alphabet.size();

  this->initialSteps.reserve(pow(3, _dimension));
  this->initialStrings.reserve(pow(_alphabet.size(), _dimension));
}

void SubmatrixCalculator::calculate() {
  generateInitialSteps(0, "");
  generateInitialStrings(0, "", false);
  calculateOffsets();

  // allocate the memory locations required to store the submatrices
  int startTime = clock();
  int memoryRequired = charLeftOffset[this->dimension - 1][this->alphabet.size()] + charLeftOffset[this->dimension - 1][1] + 5;
  cout << "Allocating " << memoryRequired << " locations." << endl;
  this->resultIndex = new pair<int, int>[memoryRequired];
  this->times[0] = (clock() - startTime) / double(CLOCKS_PER_SEC);
  cout << "Allocation time: " << this->times[0] << "s" << endl;

  // setting up temporary matrix storage
  lastSubH.reserve(this->dimension + 1);
  lastSubV.reserve(this->dimension + 1);
  vector<int> rowVec(this->dimension + 1, 0);
  for (int i = 0; i <= this->dimension; i++) {
    lastSubH.push_back(rowVec);
    lastSubV.push_back(rowVec);
  }

  // all possible initial steps and strings combinations
  startTime = clock();
  for (unsigned int strA = 0; strA < initialStrings.size(); strA++) {
    for (unsigned int strB = 0; strB < initialStrings.size(); strB++) {
      for (unsigned int stepC = 0; stepC < initialSteps.size(); stepC++) {
        for (unsigned int stepD = 0; stepD < initialSteps.size(); stepD++) {
          // storing the resulting final rows for future reference
          int offset = getOffset(initialStrings[strA], initialStrings[strB], initialSteps[stepC], initialSteps[stepD]);
        //  cout << stepOffsets[0][stepsToInt(stepsToVector(initialSteps[stepC]))] << " " << stepOffsets[1][stepsToInt(stepsToVector(initialSteps[stepD]))] << endl;
         // cout << initialSteps[stepC] << " " << initialSteps[stepD] << " to " << stepsToInt(stepsToVector(initialSteps[stepC])) << " " << stepsToInt(stepsToVector(initialSteps[stepD])) << endl;
         // system("pause");
          resultIndex[offset] =
              calculateFinalSteps(initialStrings[strA], initialStrings[strB],
                                  initialSteps[stepC], initialSteps[stepD]);
        }
      }
    }
    if (strA % 5 == 0 or strA == initialStrings.size() - 1) {
      cout << strA + 1 << " / " << initialStrings.size() << " (submatrices: "
           << (strA + 1) * initialStrings.size() * initialSteps.size() *
                  initialSteps.size() << " )" << endl;
    }
  }
  this->times[1] = (clock() - startTime) / double(CLOCKS_PER_SEC);
  cout << "Submatrix calculation time: " << this->times[1] << "s" << endl;
}

/*
    Backtracks through the submatrix, represented by the standard 4 provided
   strings, until it
    leaves the bounds and enters another submatrix. Returns the coordinates of
   the next submatrix
    entered, the entry cell in the next submatrix, and a vector of operations
   used to traverse
    the current matrix.
    The initialCost parameter combined with the two initial step vectors are
   used to calculate
    the edit distance submatrix because it's easier to backtrack through a
   cost-matrix than
    through a step matrix.
*/
pair<vector<int>, pair<pair<int, int>, pair<int, int> > >
SubmatrixCalculator::getSubmatrixPath(string strLeft, string strTop,
                                      int stepLeft, int stepTop,
                                      int finalRow, int finalCol,
                                      int initialCost) {
  calculateCostSubmatrix(strLeft, strTop, stepLeft, stepTop, initialCost);

  int i = finalRow;
  int j = finalCol;
  vector<int> operations;
  // backtracking - for movement check
  // SubmatrixCalculator::calculateCostSubmatrix
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

  // calculate the next matrix to enter depending on the initial vector we ended
  // up on
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
     Calculates the cost submatrix represented by the provided two strings and
   two initial vectors.
     The cost matrix has two parts, stored in lastSubV and lastSubH (recycled
   matrices to save space).
     The lastSubV matrix stores the costs and the lastSubH matrix stores the
   optimal paths.
     Path codes inside lastSubH:
     0 - initial vector cell, exiting the submatrix
     1 - moving up in the submatrix (deleting)
     2 - moving left in the submatrix (inserting)
     3 - moving diagonally up-left in the submatrix (replacing / matching)
*/
void SubmatrixCalculator::calculateCostSubmatrix(string strLeft, string strTop,
                                                 int stepLeft, int stepTop,
                                                 int initialCost) {

  vector <int> stepLeftVec = stepsToVector(stepLeft);
  vector <int> stepTopVec = stepsToVector(stepTop);

  lastSubV[0][0] = initialCost;
  lastSubH[0][0] = 0;
  for (int i = 1; i <= this->dimension; i++) {
    lastSubV[0][i] = lastSubV[0][i - 1] + stepTopVec[i - 1];
    lastSubV[i][0] = lastSubV[i - 1][0] + stepLeftVec[i - 1];
    lastSubH[0][i] = 0;
    lastSubH[i][0] = 0;
  }

  for (int i = 1; i <= this->dimension; i++) {
    for (int j = 1; j <= this->dimension; j++) {
      if (strLeft[i - 1] == blankCharacter) {
        lastSubV[i][j] = lastSubV[i - 1][j];
        lastSubH[i][j] = 1;
      } else if (strTop[j - 1] == blankCharacter) {
        lastSubV[i][j] = lastSubV[i][j - 1];
        lastSubH[i][j] = 2;
      } else {

        // replace
        int R = (strLeft[i - 1] != strTop[j - 1]) * this->replaceCost;
        lastSubV[i][j] = lastSubV[i - 1][j - 1] + R;
        lastSubH[i][j] = 3;

        // insert
        int alternative = lastSubV[i][j - 1] + this->insertCost;
        if (lastSubV[i][j] > alternative) {
          lastSubV[i][j] = alternative;
          lastSubH[i][j] = 2;
        }

        // delete
        alternative = lastSubV[i - 1][j] + this->deleteCost;
        if (lastSubV[i][j] > alternative) {
          lastSubV[i][j] = alternative;
          lastSubH[i][j] = 1;
        }
      }
    }
  }
}

/*
    Calculates the step submatrix represented by the provided two strings and
   two initial vectors.
    The step matrix has two parts, vertical and horizontal steps, stored in
   lastSubV and lastSubH.
*/
inline void SubmatrixCalculator::calculateSubmatrix(string strLeft,
                                                    string strTop,
                                                    string stepLeft,
                                                    string stepTop) {
  for (int i = 1; i <= this->dimension; i++) {
    lastSubV[i][0] = stepLeft[i - 1] - '1';
    lastSubH[0][i] = stepTop[i - 1] - '1';
  }
  lastSubV[0][0] = lastSubH[0][0] = 0;

  for (int i = 1; i <= this->dimension; i++) {
    for (int j = 1; j <= this->dimension; j++) {
      if (strLeft[i - 1] == blankCharacter or strTop[j - 1] == blankCharacter) {
        lastSubV[i][j] = lastSubV[i][j - 1];
        lastSubH[i][j] = lastSubH[i - 1][j];
        continue;
      }

      int R = (strLeft[i - 1] != strTop[j - 1]) * this->replaceCost;
      int lastV = lastSubV[i][j - 1];
      int lastH = lastSubH[i - 1][j];
      lastSubV[i][j] =
          mmin(R - lastH, this->deleteCost, this->insertCost + lastV - lastH);
      lastSubH[i][j] =
          mmin(R - lastV, this->insertCost, this->deleteCost + lastH - lastV);
    }
  }
}

/*
    Calculates the final step vectors for a given initial submatrix description.
*/
pair<int, int> SubmatrixCalculator::calculateFinalSteps(string strLeft,
                                                        string strTop,
                                                        string stepLeft,
                                                        string stepTop) {
  calculateSubmatrix(strLeft, strTop, stepLeft, stepTop);

  vector<int> stepRight(this->dimension, 0);
  for (int i = 1; i <= this->dimension; i++) {
    stepRight[i - 1] = lastSubV[i][this->dimension];
  }

  vector<int> stepBot(this->dimension, 0);
  for (int i = 1; i <= this->dimension; i++) {
    stepBot[i - 1] = lastSubH[this->dimension][i];
  }
  return make_pair(stepsToInt(stepRight), stepsToInt(stepBot));
}

/*
    Generates all possible initial step vectors and stores them
    into initialSteps. The arguments are recursion parameters,
    call with (0, ""). The length of the generated step vectors depends
    on the dimension of the submatrices.
*/
void SubmatrixCalculator::generateInitialSteps(int pos, string currStep) {
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
/*
    Generates all possible string vectors and stores them
    into initialStrings. The arguments are recursion parameters,
    call with (0, "", 0). The length of the generated strings depends
    on the dimension of the submatrices.
*/
void SubmatrixCalculator::generateInitialStrings(int pos, string currString,
                                                 bool blanks) {
  if (pos == this->dimension) {
    initialStrings.push_back(currString);
    return;
  }

  if (!blanks) {
    for (unsigned int i = 0; i < this->alphabet.size(); i++) {
      string tmp = currString;
      tmp.push_back(this->alphabet[i]);
      generateInitialStrings(pos + 1, tmp, false);
    }
  }

  string tmp = currString;
  tmp.push_back(blankCharacter);
  generateInitialStrings(pos + 1, tmp, true);
}

/*
    Custom minimum function with 3 arguments to reduce function call
    overhead.
*/
inline int SubmatrixCalculator::mmin(int x, int y, int z) {
  return x > y ? (y < z ? y : z) : x < z ? x : z;
}

// DEBUG
void SubmatrixCalculator::printDebug() {
  for (unsigned int i = 0; i < initialSteps.size(); i++)
    cout << stepsToPrettyString(initialSteps[i]) << endl;
  for (unsigned int i = 0; i < initialStrings.size(); i++)
    cout << initialStrings[i] << endl;
}

/*int main() {
    SubmatrixCalculator calc(3);
    calc.calculate();
    return 0;
}*/
