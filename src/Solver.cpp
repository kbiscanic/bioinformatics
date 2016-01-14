#include <cstdio>
#include <iostream>
#include <vector>
#include <cmath>
#include "SubmatrixCalculator.cpp"

#define BLANK_CHAR '-'

using namespace std;

string final_columns[2];
vector<string> final_rows[2];

const string ALPHABET = "ATGC";
string string_a, string_b;

int submatrix_dim;
int row_num;
int column_num;


void fill_edit_matrix(SubmatrixCalculator subm_calc)
{
    for (int i = 0; i < 2; i++){
        final_rows[i] = vector<string>(column_num+1, "");
    }

    string initial_string = SubmatrixCalculator::stepsToString(vector<int>(submatrix_dim+1, 1));
    for (int submatrix_j=1; submatrix_j<=column_num; submatrix_j++)
        final_rows[0][submatrix_j] = initial_string;

    for (int submatrix_i=1, alti = 1; submatrix_i<=row_num; submatrix_i++, alti = !alti) {
        final_columns[0] = initial_string;
        for (int submatrix_j=1, altj = 1; submatrix_j<=column_num; submatrix_j++, altj = !altj) {

            pair<string, string> final_steps = subm_calc.getFinalSteps(
                                    string_a.substr((submatrix_i-1)*submatrix_dim, submatrix_dim),
                                    string_b.substr((submatrix_j-1)*submatrix_dim, submatrix_dim),
                                    final_columns[!altj].substr(1),
                                    final_rows[!alti][submatrix_j].substr(1));

            final_columns[altj] = final_steps.first;
            final_rows[alti][submatrix_j] = final_steps.second;
            //final_columns[submatrix_i][submatrix_j] = final_steps.first;
            //final_rows[submatrix_i][submatrix_j] = final_steps.second;
        }
    }
}

int calc_edit_distance(SubmatrixCalculator subm_calc)
{
    int edit_distance = string_a.size();

    for (int submatrix_j=1; submatrix_j<=column_num; submatrix_j++) {
        edit_distance += subm_calc.sumSteps(final_rows[row_num % 2][submatrix_j].substr(1));
    }

    return edit_distance;
}

int main() {
    //read strings a and b
    string_a = "ATTACC";
    string_b = "ATTACG";
    // uzmi max od 2 stringa za racunat, još bolje, zamijeni da je prvi max/min? uvijek?
    //submatrix_dim = ceil(log(string_a.size()) / log(3 * ALPHABET.size()) / 2);
    submatrix_dim = 2;

    cout << "Submatrix dimension: " << submatrix_dim << endl;
    cout << "String A size: " << string_a.size() << endl;
    cout << "String B size: " << string_b.size() << endl;

    // pad strings to fit dimension
    if (string_a.size() % submatrix_dim){
        int cnt = submatrix_dim - string_a.size() % submatrix_dim;
        while (cnt--){
            string_a += BLANK_CHAR;
        }
    }
    if (string_b.size() % submatrix_dim){
        int cnt = submatrix_dim - string_b.size() % submatrix_dim;
        while (cnt--){
            string_b += BLANK_CHAR;
        }
    }

    SubmatrixCalculator subm_calc(submatrix_dim, ALPHABET, BLANK_CHAR);
    subm_calc.calculate();

    row_num = string_a.size() / (submatrix_dim);
    column_num = string_b.size() / (submatrix_dim);
    cout << "Submatrices in edit table: " << row_num << "x" << column_num << endl;

    fill_edit_matrix(subm_calc);

    cout << "Edit distance: " << calc_edit_distance(subm_calc) << endl;

    return 0;
}
