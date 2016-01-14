#include <iostream>

#include "Parser.hpp"
#include "Writer.hpp"

using namespace std;

static const int MAX_SEQ_LENGTH = 1000000;

int main(int argc, char** argv)
{
  if(argc != 3)
  {
    cout << "Usage: " << argv[0] << " <input file.fa> <output file.maf>" << endl;
    return 1;
  }

  char* in = argv[1];
  char* out = argv[2];

  Parser p(in);
  vector<Sequence*> sequences = p.readSequences();

  vector <Result*> results;
  for (unsigned int i = 0; i < sequences.size() - 1; i++)
  {
    if(sequences[i]->getData().size() > MAX_SEQ_LENGTH)
    {
      cout << "Sequence " << i << " too long; skipping" << endl;
      continue;
    }
    for (unsigned int j = i + 1; j < sequences.size(); j++)
    {
      if (sequences[j]->getData().size() > MAX_SEQ_LENGTH)
        continue;

      Result * result = new Result(sequences[i], sequences[j], 10.0); //TODO calculation
      results.push_back(result);
    }
  }

  Writer w(out);
  w.writeResults(results);
}
