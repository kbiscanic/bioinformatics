#include "Writer.hpp"

using namespace std;

// Constructor; take single argument filename which should be path to file. Overwrites existing file or creates a new one.
Writer::Writer(const char* filename) { out_.open(filename, ofstream::out); };

// destructor; close output stream on destruction
Writer::~Writer() { out_.close(); }

// converts a single sequence to 's' line of MAF format
const string toStr(Sequence* seq) {
  ostringstream out;
  string data = seq->getData();
  out << "s " << seq->getIdentifier() << " 0 "
      << count_if(data.begin(), data.end(), [](char c) { return c != '-'; })
      << " + " << data.size() << " " << data;
  return out.str();
}

// method for writing vector of results to output file; creates an alignemnt block for every result in vector
void Writer::writeResults(vector<Result*> results) {
  for (Result* result : results) {
    Writer::out_ << "a score=" << result->getScore() << endl;
    Writer::out_ << toStr(result->getA()) << endl;
    Writer::out_ << toStr(result->getB()) << endl
                 << endl;
  }
};

/* 'Unit' test
int main()
{
    Sequence s1("test1", "ATG-TT");
    Sequence s2("test2", "-TGAT-");

    Result r(s1, s2, 522.3);
    vector<Result*> results;
    results.push_back(&r);
    results.push_back(&r);
    results.push_back(&r);

    Writer w("test.out");
    w.writeResults(results);
}
*/
