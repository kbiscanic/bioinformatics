#include <cstdio>
#include <iostream>
#include <vector>

using namespace std;

int main() {
    //read strings a and b
    //|a| = |b| for now
    int string_size;

    // calculate submatrix dimension
    int submatrix_dim;

    // call submatrix calculator

    vector<vector<string > > final_columns;
    vector<vector<string > > final_rows;

    int subs_per_side = string_size/submatrix_dim;
    for (int submatrix_i=0; submatrix_i<subs_per_side; submatrix_i++) {
        vector<string> column;
        string temp_string;

        // assuming price for deletion is 1
        for (int string_index=0; string_index<submatrix_dim; string_index++)
            temp_string.push_back('1');

        column.push_back(temp_string);
        final_columns.push_back(column);
    }

    for (int submatrix_j=0; submatrix_j<subs_per_side; submatrix_j++) {
        vector<string> row;
        string temp_string;

        // assuming price for insertion is 1
        for (int string_index=0; string_index<submatrix_dim; string_index++)
            temp_string.push_back('1');

        row.push_back(temp_string);
        final_rows.push_back(row);
    }

    return 0;
}
