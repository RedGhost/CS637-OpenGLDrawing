#ifndef BEZIERPATCHPARSER_H
#define BEZIERPATCHPARSER_H

#include <fstream>
#include <list>
#include "Angel.h"
#include "BezierPatch.h"

class BezierPatchParser {
public:
    BezierPatchParser(char * filename);

    virtual BezierPatch* parse();
private:
    void readNumber();
    void nextToken();
    void parseControlPoint(BezierPatch * patch);

    std::ifstream * _file;
    char currentChar;
    char token;
    float fval;
    int ival;
};

#endif
