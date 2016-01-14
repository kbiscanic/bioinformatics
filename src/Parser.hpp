#ifndef PARSER_HPP
#define PARSER_HPP

#include <fstream>
#include <vector>

#include "Sequence.hpp"

class Parser {
 private:
  ifstream in_;

 public:
  Parser(const char* filename);
  ~Parser();

  const vector<Sequence*> readSequences();
};

#endif
