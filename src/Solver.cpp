#include <cstdio>
#include <iostream>
#include <vector>
#include <cmath>

#define BLANK_CHAR ' '

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
    string initial_string(submatrix_dim, '1');

    for (int submatrix_i=1; submatrix_i<=row_num; submatrix_i++)
        final_columns[submatrix_i][0] = initial_string;

    for (int submatrix_j=1; submatrix_j<=column_num; submatrix_j++)
        final_rows[0][submatrix_j] = initial_string;
}

void fill_edit_matrix()
{
    for (int submatrix_i=1; submatrix_i<=row_num; submatrix_i++) {
        for (int submatrix_j=1; submatrix_j<=column_num; submatrix_j++) {
            /*
            pair<string, string> final_steps = subm_calc.getFinalSteps(
                                    string_a.substr((submatrix_i-1)*submatrix_dim, submatrix_dim),
                                    string_b.substr((submatrix_j-1)*submatrix_dim, submatrix_dim),
                                    final_columns[submatrix_i][submatrix_j-1],
                                    final_rows[submatrix_i-1][submatrix_j]);

            final_columns[submatrix_i][submatrix_j] = final_steps.second;
            final_rows[submatrix[i][submatrix_j] = final_steps.first;
            */
        }
    }
}

int calc_edit_distance()
{
    int edit_distance = 0;

    for (int submatrix_i=1; submatrix_i<=row_num; submatrix_i++)
        //edit_distance += subm_calc.sumSteps(final_columns[submatrix_i][0]);

    for (int submatrix_j=1; submatrix_j<=column_num; submatrix_j++)
        //edit_distance += subm_calc.sumSteps(final_rows[subs_per_side][submatrix_j]);

    return edit_distance;
}

int main() {
    //read strings a and b
    string_a = "atcggatta";
    string_b = "atccattcc";

    submatrix_dim = ceil(log(string_a.size()) / log(12));

    // pad strings to fit dimension

    // call submatrix calculator

    int row_num = string_a.size() / submatrix_dim;
    int column_num = string_b.size() / submatrix_dim;
    final_columns.resize(row_num+1, vector<string>(row_num+1, ""));
    final_rows.resize(column_num+1, vector<string>(column_num+1, ""));

    initialize_edit_matrix();
    fill_edit_matrix();

    cout << calc_edit_distance();

    return 0;
}
