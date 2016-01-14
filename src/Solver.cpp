#include <cstdio>
#include <iostream>
#include <vector>
#include <cmath>
#include "SubmatrixCalculator.cpp"

#define BLANK_CHAR '-'

using namespace std;

vector<vector<string > > final_columns;
vector<vector<string > > final_rows;

string string_a, string_b;

int submatrix_dim;
int row_num;
int column_num;

void pad_string()
{

}

void initialize_edit_matrix()
{
    final_columns.resize(row_num+1, vector<string>(row_num+1, ""));
    final_rows.resize(column_num+1, vector<string>(column_num+1, ""));

    string initial_string = SubmatrixCalculator::stepsToString(vector<int>(submatrix_dim+1, 1));

    for (int submatrix_i=1; submatrix_i<=row_num; submatrix_i++)
        final_columns[submatrix_i][0] = initial_string;

    for (int submatrix_j=1; submatrix_j<=column_num; submatrix_j++)
        final_rows[0][submatrix_j] = initial_string;

    final_columns[1][0][0] -= 1;
    final_rows[0][1][0] -= 1;
}

void fill_edit_matrix(SubmatrixCalculator subm_calc)
{
    for (int submatrix_i=1; submatrix_i<=row_num; submatrix_i++) {
        for (int submatrix_j=1; submatrix_j<=column_num; submatrix_j++) {

            pair<string, string> final_steps = subm_calc.getFinalSteps(
                                    string_a.substr((submatrix_i-1)*submatrix_dim, submatrix_dim),
                                    string_b.substr((submatrix_j-1)*submatrix_dim, submatrix_dim),
                                    final_columns[submatrix_i][submatrix_j-1].substr(1),
                                    final_rows[submatrix_i-1][submatrix_j].substr(1));

            final_columns[submatrix_i][submatrix_j] = final_steps.first;
            final_rows[submatrix_i][submatrix_j] = final_steps.second;
        }
    }
}

int calc_edit_distance(SubmatrixCalculator subm_calc)
{
    int edit_distance = 0;

    for (int submatrix_i=1; submatrix_i<=row_num; submatrix_i++) {
        edit_distance += subm_calc.sumSteps(final_columns[submatrix_i][0].substr(1));
            cout<<edit_distance<<endl;

    }
    cout<<endl;
    for (int submatrix_j=1; submatrix_j<=column_num; submatrix_j++) {
        edit_distance += subm_calc.sumSteps(final_rows[row_num][submatrix_j].substr(1));
        cout<<final_rows[row_num][submatrix_j].substr(1)<<endl;
    }

    return edit_distance;
}

int main() {
    //read strings a and b
    string_a = "TAATCC";
    string_b = "ATTACC";

    //submatrix_dim = ceil(log(string_a.size()) / log(12));
    submatrix_dim = 2;

    cout << "Submatrix dimension: " << submatrix_dim << endl;
    cout << "String A size: " << string_a.size() << endl;
    cout << "String B size: " << string_b.size() << endl;

    // pad strings to fit dimension

    SubmatrixCalculator subm_calc(submatrix_dim, "ATGC");
    subm_calc.calculate();

    row_num = string_a.size() / (submatrix_dim);
    column_num = string_b.size() / (submatrix_dim);
    cout << "Submatrices in edit table: " << row_num << "x" << column_num << endl;

    initialize_edit_matrix();
    fill_edit_matrix(subm_calc);

    cout << "Edit distance: " << calc_edit_distance(subm_calc) << endl;

    return 0;
}
