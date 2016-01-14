#ifndef SEQUENCE_HPP
#define SEQUENCE_HPP

#include<string>

using namespace std;

class Sequence
{
private:
	string identifier_;
	string data_;

public:
	Sequence(const string& identifier, const string& data);
	~Sequence();

	const string getIdentifier() const { return identifier_; }
	const string getData() const { return data_; }
};

#endif
