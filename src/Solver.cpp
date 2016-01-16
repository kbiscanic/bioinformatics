#include "Solver.hpp"
//#include "SubmatrixCalculator.hpp"
//#include "SubmatrixCalculator.cpp"

/*
    Used to compute the edit distance and alignment between two strings
    of genome sequences.

    Function calculate_with_path() will return both the edit distance and
    the two aligned sequences, while function calculate() will return just
    the edit distance and thus use less memory.
*/
Solver::Solver(string str_a, string str_b, string _alphabet,
               int _submatrix_dim) {
    /*
        We want the calculation matrix columns to represent the shorter string
        because both the time complexity and the space complexity in the path-less
        version depend mostly on the number of columns (in the scope of
        submatrix-combining complexities; the submatrix-combining space complexity
       is
        "negligible" in total).
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

    // pad the strings to fit dimension
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

    calculateStringOffsets();
}

/*
    Calculates the submatrix address offset for each initial string that can
    appear during calculation, i.e. sequentially for each substring of
    string_a and string_b of length submatrix_dim. Saves time when combining
    submatrix results.
*/
void Solver::calculateStringOffsets(){
    this->str_a_offsets.resize(row_num + 1);
    this->str_b_offsets.resize(column_num + 1);

    string tempBlank = "";
    string tempBlank2 = "";
    for (int i = 0; i < this->submatrix_dim; i++){
        tempBlank += alphabet[0];
        tempBlank2 += '0';
    }

    for (int i = 1; i <= row_num; i++){
        str_a_offsets[i] = subm_calc->getOffset(string_a.substr((i - 1) * submatrix_dim, submatrix_dim), tempBlank, tempBlank2, tempBlank2);
    }

    for (int i = 1; i <= column_num; i++){
        str_b_offsets[i] = subm_calc->getOffset(tempBlank, string_b.substr((i - 1) * submatrix_dim, submatrix_dim), tempBlank2, tempBlank2);
    }
}

/*
    Calculates sequence alignments based on an edit path.
    Integers in the edit path represent:
    1 - moving down in the submatrix
    2 - moving right in the submatrix
    3 - moving diagonally in the submatrix
*/
pair<string, string> Solver::calculate_alignment(vector<int> path) {
    string a_aligned;
    string b_aligned;

    int a_cnt = 0, b_cnt = 0;
    for (int i = path.size() - 1; i >= 0; i--) {
        if (path[i] == 1) {
            a_aligned += string_a[a_cnt++];
            b_aligned += '-';
        } else if (path[i] == 2) {
            a_aligned += '-';
            b_aligned += string_b[b_cnt++];
        } else if (path[i] == 3) {
            a_aligned += string_a[a_cnt++];
            b_aligned += string_b[b_cnt++];
        }
    }

    return make_pair(a_aligned, b_aligned);
}

/*
    Backtracks through the submatrices until it reaches the
    starting cell. Edit operations in the vector will be ordered backwards
    and the integers represent:
    1 - moving down in the submatrix
    2 - moving right in the submatrix
    3 - moving diagonally in the submatrix
*/
vector<int> Solver::get_edit_path() {
    pair<vector<int>, pair<pair<int, int>, pair<int, int> > > ret;
    vector<int> edit_path;

    int x = string_a_real_size / submatrix_dim;
    if ((string_a_real_size % submatrix_dim) != 0) x++;
    int y = string_b_real_size / submatrix_dim;
    if ((string_b_real_size % submatrix_dim) != 0) y++;
    int sub_x = string_a_real_size % submatrix_dim;
    int sub_y = string_b_real_size % submatrix_dim;

    if (sub_x == 0) sub_x = submatrix_dim;
    if (sub_y == 0) sub_y = submatrix_dim;

    while (x != 0 && y != 0) {
        ret = subm_calc->getSubmatrixPath(
                  string_a.substr((x - 1) * submatrix_dim, submatrix_dim),
                  string_b.substr((y - 1) * submatrix_dim, submatrix_dim),
                  all_columns[x][y - 1], all_rows[x - 1][y], sub_x, sub_y,
                  top_left_costs[x][y]);
        for (int i = 0; i < ret.first.size(); i++) {
            edit_path.push_back(ret.first[i]);
        }

        x += ret.second.first.first;
        y += ret.second.first.second;
        sub_x = ret.second.second.first;
        sub_y = ret.second.second.second;
    }

    /*
        Once we have reached the submatrix in the first row (or the first column)
        of the full matrix, we may still need to move along the row (or column)
        towards the starting cell in position 0, 0.
    */
    if (x == 0) {
        for (int i = 0; i < (y - 1) * submatrix_dim + sub_y; i++) {
            edit_path.push_back(2);
        }
    }

    if (y == 0) {
        for (int i = 0; i < (x - 1) * submatrix_dim + sub_x; i++) {
            edit_path.push_back(1);
        }
    }

    return edit_path;
}

/*
    Computes and returns the edit distance. This function will use less
    memory than it's calculate_with_path() counterpart since it doesn't
    need to backtrack for the edit path.
*/
int Solver::calculate() {
    fill_edit_matrix_low_memory();

    int edit_distance = string_a_real_size;

    for (int submatrix_j = 1; submatrix_j <= column_num; submatrix_j++) {
        edit_distance +=
            subm_calc->sumSteps(final_rows[row_num % 2][submatrix_j]);
    }

    return edit_distance;
}

/*
    Computes and returns the edit distance and sequence alignments. Use function
    calculate() if sequence alignments are not needed as it is more memory
   efficient.
*/
pair<int, pair<string, string> > Solver::calculate_with_path() {
    fill_edit_matrix();

    int edit_distance = string_a_real_size;

    for (int submatrix_j = 1; submatrix_j <= column_num; submatrix_j++) {
        edit_distance +=
            subm_calc->sumSteps(all_rows[row_num][submatrix_j]);
    }

    return make_pair(edit_distance, calculate_alignment(get_edit_path()));
}

/*
    Uses the precalculated submatrices from SubmatrixCalculator to determine
    the values in the edit matrix. Keeps all the final rows and columns of
    each submatrix in memory.
*/
void Solver::fill_edit_matrix() {
    all_columns.resize(row_num + 1, vector<int>(column_num + 1, 0));
    all_rows.resize(row_num + 1, vector<int>(column_num + 1, 0));
    top_left_costs.resize(row_num + 1, vector<int>(column_num + 1, 0));

    int initialVector = 0;
    for (int i = 0; i < submatrix_dim; i++){
        initialVector = initialVector * 10 + 2; // 222 == "222" == (1, 1, 1)
    }

    // padding string b step vectors
    for (int submatrix_j = 1; submatrix_j <= column_num; submatrix_j++) {
        if ((submatrix_j * submatrix_dim - 1) >= string_b_real_size) {
            vector<int> temp_vec(submatrix_dim, 0);
            for (int i = 0;
                    i < (string_b_real_size - ((submatrix_j - 1) * submatrix_dim));
                    i++)
                temp_vec[i] = 1;
            all_rows[0][submatrix_j] = SubmatrixCalculator::stepsToInt(temp_vec);
        } else {
            all_rows[0][submatrix_j] = initialVector;
        }
    }

    // padding string a step vectors
    for (int submatrix_i = 1; submatrix_i <= row_num; submatrix_i++) {
        if ((submatrix_i * submatrix_dim - 1) >= string_a_real_size) {
            vector<int> temp_vec(submatrix_dim, 0);
            for (int i = 0;
                    i < (string_a_real_size - ((submatrix_i - 1) * submatrix_dim));
                    i++)
                temp_vec[i] = 1;
            all_columns[submatrix_i][0] =
                SubmatrixCalculator::stepsToInt(temp_vec);
        } else {
            all_columns[submatrix_i][0] = initialVector;
        }
    }

    for (int submatrix_i = 1; submatrix_i <= row_num; submatrix_i++) {
        if ((submatrix_i - 1) * submatrix_dim > string_a_real_size)
            top_left_costs[submatrix_i][1] = string_a_real_size;
        else
            top_left_costs[submatrix_i][1] = (submatrix_i - 1) * submatrix_dim;

        for (int submatrix_j = 1; submatrix_j <= column_num; submatrix_j++) {

            pair<int, int> final_steps = subm_calc->resultIndex[
                // offset calculation
                str_a_offsets[submatrix_i] +                                // left string
                str_b_offsets[submatrix_j] +                                // top string
                // left steps
                subm_calc->stepOffsets[0][all_columns[submatrix_i][submatrix_j - 1]] +
                // top steps
                subm_calc->stepOffsets[1][all_rows[submatrix_i - 1][submatrix_j]]
            ];

            all_columns[submatrix_i][submatrix_j] = final_steps.first;
            all_rows[submatrix_i][submatrix_j] = final_steps.second;

            if (submatrix_j != 1) {
                top_left_costs[submatrix_i][submatrix_j] =
                    top_left_costs[submatrix_i][submatrix_j - 1];
                top_left_costs[submatrix_i][submatrix_j] += subm_calc->sumSteps(
                    all_rows[submatrix_i - 1][submatrix_j - 1]);
            }
        }
    }
}

/*
    Uses the precalculated submatrices from SubmatrixCalculator to determine
    the values in the edit matrix. Only the two columns and rows that were
    last retrieved are kept in memory.
*/
void Solver::fill_edit_matrix_low_memory() {
    for (int i = 0; i < 2; i++) {
        final_rows[i].resize(column_num + 1);
    }

    int initialVector = 0;
    for (int i = 0; i < submatrix_dim; i++){
        initialVector = initialVector * 10 + 2; // 222 == "222" == (1, 1, 1)
    }

    // padding string b step vectors
    for (int submatrix_j = 1; submatrix_j <= column_num; submatrix_j++) {
        if ((submatrix_j * submatrix_dim - 1) >= string_b_real_size) {
            vector<int> temp_vec(submatrix_dim, 0);
            for (int i = 0; i < (string_b_real_size - ((submatrix_j - 1) * submatrix_dim));
                i++)
                temp_vec[i] = 1;
            final_rows[0][submatrix_j] = SubmatrixCalculator::stepsToInt(temp_vec);
        } else {
            final_rows[0][submatrix_j] = initialVector;
        }
    }

    for (int submatrix_i = 1, alti = 1; submatrix_i <= row_num;
            submatrix_i++, alti = !alti) {
        // padding string a step vectors
        if ((submatrix_i * submatrix_dim - 1) >= string_a_real_size) {
            vector<int> temp_vec(submatrix_dim, 0);
            for (int i = 0; i < (string_a_real_size - ((submatrix_i - 1) * submatrix_dim));
                    i++)
                temp_vec[i] = 1;
            final_columns[0] = SubmatrixCalculator::stepsToInt(temp_vec);
        } else {
            final_columns[0] = initialVector;
        }

        for (int submatrix_j = 1, altj = 1; submatrix_j <= column_num;
                submatrix_j++, altj = !altj) {

            pair<int, int> final_steps = subm_calc->resultIndex[
                // offset calculation
                str_a_offsets[submatrix_i] +                                // left string
                str_b_offsets[submatrix_j] +                                // top string
                subm_calc->stepOffsets[0][final_columns[!altj]] +           // left steps
                subm_calc->stepOffsets[1][final_rows[!alti][submatrix_j]]   // top steps
            ];

            final_columns[altj] = final_steps.first;
            final_rows[alti][submatrix_j] = final_steps.second;
        }
    }
}
/*
int main() {
    // Solver solver ("ATTACG", "ATTAG", "ATGC", 2);
    Solver solver("ACCGGTTGCCCGCTACATGCTCCAACCATCCGGCGATGGTTACCTGCTGCCGGACTGGTATAGCGCAGAGCCGCGTCGACACCGCGTATCCGTGCCCCCC",
           "TGGGGATTGCCAGTCCGTCCGGGGAGGTATTCAGAAAGGTACACCGGTCTGTTGATATTCATGTAACAGGTATTAATGATGAAGAAAGGAATGGCAAACA",
           "ATGC", 2);
    int sol_ed = solver.calculate();
    cout << "Edit distance: " << sol_ed << endl;

    pair<int, pair<string, string> > sol = solver.calculate_with_path();
    cout << sol.first << endl << sol.second.first << endl << sol.second.second
        << endl;

    return 0;
}
*/
