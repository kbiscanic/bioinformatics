#include "Result.hpp"

// construct Result object from sequences and score
Result::Result(Sequence* a, Sequence* b, double score)
    : a_(a),
      b_(b),
      score_(score){

      };

Result::~Result(){

};
