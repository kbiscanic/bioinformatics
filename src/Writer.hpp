#include<fstream>
#include <sstream>
#include<vector>
#include <algorithm>
#include<iostream>

#include "Result.hpp"

class Writer
{
private:
    ofstream out_;

public:
    Writer(const char* filename);
    ~Writer();

    void writeResults(vector<Result*> results);
};
