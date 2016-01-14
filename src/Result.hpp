#include "Sequence.hpp"

class Result
{
private:
    Sequence a_;
    Sequence b_;
    double score_;

public:
    Result(Sequence a, Sequence b, double score);
    ~Result();

    double getScore() const { return score_; }
    Sequence getA() { return a_; }
    Sequence getB() { return b_; }
};
