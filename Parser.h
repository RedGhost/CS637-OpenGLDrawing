#ifndef PARSER_H
#define PARSER_H

#include <list>
#include "Model.h"

class Parser {
public:
    virtual std::list<Model*> parse() = 0;
};

#endif
