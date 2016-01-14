#include "Solver.hpp"

Solver::Solver(string str_a, string str_b, string _alphabet,
               int _submatrix_dim) {
  /*
      We want the calculation matrix columns to represent the shorter string
     because
      both the time complexity and the space complexity in the path-less version
      depend mostly on the number of columns (in the scope of
     submatrix-combining
      complexities; the submatrix-combining space complexity is "negligible" in
     total).
  */
  if (str_a.size() >= str_b.size()) {
    this->string_a = str_a;
    this->string_b = str_b;
  } else {
    this->string_a = str_b;
    this->string_b = str_a;
  }

  this->alphabet = _alphabet;

  if (_submatrix_dim > 0) {
    this->submatrix_dim = _submatrix_dim;
  } else {
    // calculate the submatrix dimension using the longer string to reduce
    // complexity
    this->submatrix_dim =
        ceil(log(this->string_a.size()) / log(3 * _alphabet.size()) / 2);
  }

  cout << "Submatrix dimension: " << submatrix_dim << endl;
  cout << "String A size: " << string_a.size() << endl;
  cout << "String B size: " << string_b.size() << endl;

  string_a_real_size = string_a.size();
  string_b_real_size = string_b.size();

  // pad strings to fit dimension
  if (string_a.size() % submatrix_dim) {
    int cnt = submatrix_dim - string_a.size() % submatrix_dim;
    while (cnt--) {
      string_a += this->BLANK_CHAR;
    }
  }
  if (string_b.size() % submatrix_dim) {
    int cnt = submatrix_dim - string_b.size() % submatrix_dim;
    while (cnt--) {
      string_b += this->BLANK_CHAR;
    }
  }

  // calculate the dimensions of the edit matrix (the number of submatrices)
  this->row_num = string_a.size() / (submatrix_dim);
  this->column_num = string_b.size() / (submatrix_dim);
  cout << "Submatrices in edit table: " << row_num << "x" << column_num << endl;

  // generate all possible submatrices for the given alphabet and dimension
  this->subm_calc = new SubmatrixCalculator(this->submatrix_dim, this->alphabet,
                                            this->BLANK_CHAR);
  subm_calc->calculate();
}

int Solver::calculate() {
  fill_edit_matrix_low_memory();

  int edit_distance = string_a_real_size;

  for (int submatrix_j = 1; submatrix_j <= column_num; submatrix_j++) {
    edit_distance +=
        subm_calc->sumSteps(final_rows[row_num % 2][submatrix_j].substr(1));
  }

  return edit_distance;
}

pair<int, pair<string, string> > Solver::calculate_with_path() {
    fill_edit_matrix();

    int edit_distance = string_a_real_size;

    for (int submatrix_j=1; submatrix_j<=column_num; submatrix_j++) {
        edit_distance += subm_calc->sumSteps(all_rows[row_num][submatrix_j].substr(1));
    }

    return make_pair(edit_distance, make_pair("", ""));
}

void Solver::fill_edit_matrix() {
    all_columns.resize(row_num+1, vector<string>(column_num+1, ""));
    all_rows.resize(row_num+1, vector<string>(column_num+1, ""));
    top_left_costs.resize(row_num+1, vector<int>(column_num+1, 0));

    string initial_string = SubmatrixCalculator::stepsToString(vector<int>(submatrix_dim+1, 1));
    for (int submatrix_j=1; submatrix_j<=column_num; submatrix_j++) {

        if ((submatrix_j*submatrix_dim-1) >= string_b_real_size) {
            vector<int> temp_vec (submatrix_dim+1, 0);
            for (int i=0; i<(string_b_real_size-((submatrix_j-1)*submatrix_dim-1)); i++)
                temp_vec[i] = 1;
            all_rows[0][submatrix_j] = SubmatrixCalculator::stepsToString(temp_vec);
        }
        else {
            all_rows[0][submatrix_j] = initial_string;
        }
    }

    for (int submatrix_i=1; submatrix_i<=row_num; submatrix_i++) {

        if ((submatrix_i*submatrix_dim-1) >= string_a_real_size) {
            vector<int> temp_vec (submatrix_dim+1, 0);
            for (int i=0; i<(string_a_real_size-((submatrix_i-1)*submatrix_dim-1)); i++)
                temp_vec[i] = 1;
            all_columns[submatrix_i][0] = SubmatrixCalculator::stepsToString(temp_vec);
        }
        else {
            all_columns[submatrix_i][0] = initial_string;
        }
    }

    for (int submatrix_i=1; submatrix_i<=row_num; submatrix_i++) {

    if ((submatrix_i-1)*submatrix_dim > string_a_real_size)
    top_left_costs[submatrix_i][1] = string_a_real_size;
    else
    top_left_costs[submatrix_i][1] = (submatrix_i-1)*submatrix_dim;

        for (int submatrix_j=1; submatrix_j<=column_num; submatrix_j++) {

            pair<string, string> final_steps = subm_calc->getFinalSteps(
                                    string_a.substr((submatrix_i-1)*submatrix_dim, submatrix_dim),
                                    string_b.substr((submatrix_j-1)*submatrix_dim, submatrix_dim),
                                    all_columns[submatrix_i][submatrix_j-1].substr(1),
                                    all_rows[submatrix_i-1][submatrix_j].substr(1));

            all_columns[submatrix_i][submatrix_j] = final_steps.first;
            all_rows[submatrix_i][submatrix_j] = final_steps.second;

    if (submatrix_j == 1)
        top_left_costs[submatrix_i][submatrix_j] = top_left_costs[submatrix_i][1];
    else
        top_left_costs[submatrix_i][submatrix_j] = top_left_costs[submatrix_i][submatrix_j-1];

    top_left_costs[submatrix_i][submatrix_j] += subm_calc->sumSteps(all_rows[submatrix_i-1][submatrix_j].substr(1));
        }
    }
}

void Solver::fill_edit_matrix_low_memory() {
  for (int i = 0; i < 2; i++) {
    final_rows[i] = vector<string>(column_num + 1, "");
  }

  string initial_string =
      SubmatrixCalculator::stepsToString(vector<int>(submatrix_dim + 1, 1));
  for (int submatrix_j = 1; submatrix_j <= column_num; submatrix_j++) {
    if ((submatrix_j * submatrix_dim - 1) >= string_b_real_size) {
      vector<int> temp_vec(submatrix_dim + 1, 0);
      for (int i = 0;
           i < (string_b_real_size - ((submatrix_j - 1) * submatrix_dim - 1));
           i++)
        temp_vec[i] = 1;
      final_rows[0][submatrix_j] = SubmatrixCalculator::stepsToString(temp_vec);
    } else {
      final_rows[0][submatrix_j] = initial_string;
    }
  }

  for (int submatrix_i = 1, alti = 1; submatrix_i <= row_num;
       submatrix_i++, alti = !alti) {
    if ((submatrix_i * submatrix_dim - 1) >= string_a_real_size) {
      vector<int> temp_vec(submatrix_dim + 1, 0);
      for (int i = 0;
           i < (string_a_real_size - ((submatrix_i - 1) * submatrix_dim - 1));
           i++)
        temp_vec[i] = 1;
      final_columns[0] = SubmatrixCalculator::stepsToString(temp_vec);
    } else {
      final_columns[0] = initial_string;
    }

    for (int submatrix_j = 1, altj = 1; submatrix_j <= column_num;
         submatrix_j++, altj = !altj) {
      pair<string, string> final_steps = subm_calc->getFinalSteps(
          string_a.substr((submatrix_i - 1) * submatrix_dim, submatrix_dim),
          string_b.substr((submatrix_j - 1) * submatrix_dim, submatrix_dim),
          final_columns[!altj].substr(1),
          final_rows[!alti][submatrix_j].substr(1));

      final_columns[altj] = final_steps.first;
      final_rows[alti][submatrix_j] = final_steps.second;
    }
  }
}

/* 'Unit' test
int main() {
    Solver solver("A", "G", "ATGC", 2);
    int sol = solver.calculate();
    //pair<int, vector<int>> sol = solver.calculateWithPath();
    cout << "Edit distance: " << sol << endl;
    return 0;
}
*/
