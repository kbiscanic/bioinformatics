#ifndef WRITER_HPP
#define WRITER_HPP

#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <iostream>

#include "Result.hpp"

/*
Writer for writing results to MAF file format.
*/
class Writer {
 private:
  ofstream out_;

 public:
  // Constructor; take single argument filename which should be path to file.
  // Overwrites existing file or creates a new one.
  Writer(const char* filename);
  ~Writer();

  // method for writing vector of results to output file
  void writeResults(vector<Result*> results);
};

#endif
