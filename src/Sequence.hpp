#ifndef SEQUENCE_HPP
#define SEQUENCE_HPP

#include <string>

using namespace std;

/*
Class representing a sequence of chromosome. Consists of sequence identifier and
string representation.
*/
class Sequence {
 private:
  string identifier_;
  string data_;

 public:
  // Constructor for Sequence object from identifier and string representation
  Sequence(const string& identifier, const string& data);
  ~Sequence();

  // getter for identifier
  const string getIdentifier() const { return identifier_; }
  // getter for string representation of sequence
  const string getData() const { return data_; }
};

#endif
