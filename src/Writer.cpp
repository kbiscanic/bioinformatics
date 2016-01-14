#include "Writer.hpp"

using namespace std;

Writer::Writer(const char* filename)
{
    out_.open(filename, ofstream::out);
};

Writer::~Writer()
{
    out_.close();
}

const string toStr(Sequence seq)
{
    ostringstream out;
    string data = seq.getData();
    out << "s " << seq.getIdentifier() << " 0 " << count_if(data.begin(), data.end(), [](char c){return c != '-';}) << " + " << data.size() << " " << data;
    return out.str();
}

void Writer::writeResults(vector<Result*> results)
{
    for(Result* result : results)
    {
        Writer::out_ << "a score=" << result->getScore() << endl;
        Writer::out_ << toStr(result->getA()) << endl;
        Writer::out_ << toStr(result->getB()) << endl << endl;
    }
};

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
