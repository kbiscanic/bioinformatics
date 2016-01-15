#ifndef PARSER_HPP
#define PARSER_HPP

#include <fstream>
#include <vector>

#include "Sequence.hpp"

/*
Parser for .fa files in FASTA format.
*/
class Parser {
 private:
  ifstream in_;

 public:
  // contructor for parser; takes string filename which should be full path to .fa file
  Parser(const char* filename);
  ~Parser();

  // reads sequences from file and returns them in a vector
  const vector<Sequence*> readSequences();
};

#endif
