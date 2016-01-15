#ifndef RESULT_HPP
#define RESULT_HPP

#include "Sequence.hpp"

/*
Class representing a result of single sequence alignment. Consists of a score
(edit distance) and aligned sequences.
*/
class Result {
 private:
  Sequence* a_;
  Sequence* b_;
  double score_;

 public:
  // construct Result object from sequences and score
  Result(Sequence* a, Sequence* b, double score);
  ~Result();

  // getter for score
  double getScore() const { return score_; }
  // getter for first sequence
  Sequence* getA() { return a_; }
  // getter for second sequence
  Sequence* getB() { return b_; }
};

#endif
