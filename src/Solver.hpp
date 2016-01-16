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
  pair<string, string> calculate_alignment(vector<int> edit_path);
  vector<int> get_edit_path();
  int calculate();
  pair<int, pair<string, string> > calculate_with_path();

 private:
  SubmatrixCalculator* subm_calc;

  const char BLANK_CHAR = '-';

  void fill_edit_matrix();
  void fill_edit_matrix_low_memory();
  void calculateStringOffsets();

  vector<int> final_rows[2];
  int final_columns[2];

  string alphabet;
  string string_a, string_b;

  int string_a_real_size;
  int string_b_real_size;

  vector<vector<int> > all_columns;
  vector<vector<int> > all_rows;

  vector<int> str_a_offsets;
  vector<int> str_b_offsets;

  // value of the top left cell for each submatrix
  vector<vector<int> > top_left_costs;

  int submatrix_dim;
  int row_num;
  int column_num;
};

#endif
