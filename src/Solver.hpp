#ifndef SOLVER_HPP
#define SOLVER_HPP

#include <cstdio>
#include <iostream>
#include <vector>
#include <cmath>

#include "SubmatrixCalculator.hpp"

using namespace std;

class Solver {
 public:
  Solver(string str_a, string str_b, string _alphabet = "ATGC",
         int _submatrix_dim = 0);
  int calculate();
  pair<int, pair<string, string>> calculate_with_path();

 private:
  SubmatrixCalculator* subm_calc;

  const char BLANK_CHAR = '-';
  string final_columns[2];

  void fill_edit_matrix();
  void fill_edit_matrix_low_memory();
  vector<string> final_rows[2];
  string alphabet;
  string string_a, string_b;

  int string_a_real_size;
  int string_b_real_size;

  vector<vector<string > > all_columns;
  vector<vector<string > > all_rows;

  // value of the top left cell for each submatrix
  vector<vector<int> > top_left_costs;

  int submatrix_dim;
  int row_num;
  int column_num;
};

#endif
