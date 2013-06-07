#ifndef SMFPARSER_H
#define SMFPARSER_H

#include <fstream>
#include <list>
#include "Angel.h"
#include "Parser.h"
#include "Model.h"

class SMFParser : public Parser {
public:
    SMFParser(char * filename);

    virtual std::list<Model*> parse();
private:
    void readNumber();
    void nextToken();
    void parseVertex(Model * model);
    void parseFace(Model * model);

    std::ifstream * _file;
    char currentChar;
    char token;
    float fval;
    int ival;
};

#endif
