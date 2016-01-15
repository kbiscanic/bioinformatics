#include <iostream>

#include "BasicEditDistance.hpp"
#include "Solver.hpp"
#include "Parser.hpp"
#include "Writer.hpp"

using namespace std;

static const int MAX_SEQ_LENGTH = 1000000;

/* Main program
 Usage: <algorithm>  <input file.fa> <output file.maf>
*/
int main(int argc, char** argv) {
  if (argc != 4) {
    cout << "Usage: " << argv[0]
         << " <algorithm>  <input file.fa> <output file.maf>" << endl;
    return 1;
  }

  char algorithm = argv[1][0];
  char* in = argv[2];
  char* out = argv[3];

  Parser p(in);
  vector<Sequence*> sequences = p.readSequences();

  vector<Result*> results;
  for (unsigned int i = 0; i < sequences.size() - 1; i++) {
    if (sequences[i]->getData().size() > MAX_SEQ_LENGTH) {
      cout << "Sequence " << i << " too long; skipping" << endl;
      continue;
    }
    for (unsigned int j = i + 1; j < sequences.size(); j++) {
      if (sequences[j]->getData().size() > MAX_SEQ_LENGTH) continue;

      if (algorithm == 'b') {
        BasicEditDistance bed(sequences[i]->getData(), sequences[j]->getData());

        int startTime = clock();
        int score = bed.getResult();
        cout << "Edit distance calculation (Needleman-Wunsch): " <<
          (clock() - startTime) / double(CLOCKS_PER_SEC) << endl;

        Result* result =
            new Result(sequences[i], sequences[j], score);
        results.push_back(result);
      } else if (algorithm == 'd') {
        Solver solver(sequences[i]->getData(), sequences[j]->getData());

        int startTime = clock();
        int score = solver.calculate();
        cout << "Edit distance calculation (Masek-Paterson): " << 
            (clock() - startTime) / double(CLOCKS_PER_SEC) << endl;

        Result* result =
            new Result(sequences[i], sequences[j], score);
        results.push_back(result);
      } else {
        Solver solver(sequences[i]->getData(), sequences[j]->getData());

        int startTime = clock();
        pair<int,pair<string,string>> res = solver.calculate_with_path();
        cout << "Edit path calculation (Masek-Paterson): " << 
            (clock() - startTime) / double(CLOCKS_PER_SEC) << endl;

        Result* result = new Result(new Sequence(sequences[i]->getIdentifier()
         , res.second.first), new Sequence(sequences[j]->getIdentifier()
         , res.second.second), res.first);

        results.push_back(result);
      }
    }
  }

  Writer w(out);
  w.writeResults(results);
}
