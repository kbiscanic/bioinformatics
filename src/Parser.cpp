#include "Parser.hpp"

#include<iostream>

Parser::Parser(const char* filename)
{
    Parser::in_.open(filename, ifstream::in);
};

Parser::~Parser()
{
    in_.close();
}

const vector<Sequence*> Parser::readSequences()
{

    vector<Sequence*> sequences;

    while(!Parser::in_.eof())
    {
        string line;
        getline(Parser::in_, line);

        if(line.size() == 0)
            continue;

        string identifier;
        string sequence;

        if(line.at(0) == '>'){
            identifier = line.substr(1, line.find_first_of(" \n|")-1);

            char next = Parser::in_.peek();

            while(next != '>' && !Parser::in_.eof()){
                getline(Parser::in_, line);
                sequence += line;

                next = Parser::in_.peek();
            }

        } else {
            continue;
        }

        Sequence * seq = new Sequence(identifier, sequence);
        sequences.push_back(seq);
        cout << seq->getIdentifier() << endl;
    }

    return sequences;
};

int main()
{
    Parser p("test/data/Escherichia_coli.GCA_000967155.1.30.dna.toplevel.fa");
    vector<Sequence*> sequences = p.readSequences();

    cout << sequences.size();
}
