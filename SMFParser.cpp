#include <cstring>
#include <cstdlib>
#include "SMFParser.h"
#include "Angel.h"

#include <iostream>
#include <cmath>

using namespace std;

enum { INTEGER, FLOAT, VERTEX, FACE, END };

SMFParser :: SMFParser(char * filename) {
    _file = new ifstream(filename, ifstream::in);
    currentChar = '\0';
}

void SMFParser :: nextToken() {
    while(_file->good()) {
        int c = _file->get();
        currentChar = (char) c;
        if(c <= '9' && c >= '0' || c == '-') {
            readNumber();
            return;
        }
        else if(c == 'v') {
            token = VERTEX;
            return;
        }
        else if(c == 'f') {
            token = FACE;
            return;
        }
    }
    token = END;
}

void SMFParser :: readNumber() {
    int integerPart = 0;
    int sign = 1;
    if(currentChar == '-') {
        sign = -1;
    }
    else {
        integerPart += (currentChar - '0');
    }

    // reading before decimal
    while(_file->good()) {
        int i = _file->get();
        currentChar = (char) i;
        if(i == '.') {
            break;
        }
        else if(i > '9' || i < '0') {
            token = INTEGER;
            ival = sign * integerPart;
            return;
        }
        integerPart = (10*integerPart) + (i - '0');
    }

    // reading after decimal
    float decimalPart = 0.0f;
    int place = 1;
    while(_file->good()) {
        int i = _file->get();
        currentChar = (char) i;
        if(i > '9' || i < '0') {
            token = FLOAT;
            fval = (float)sign * ((float)integerPart + decimalPart);
            return;
        }
        decimalPart += ( (float) (i - '0') /  pow( (float) 10, (float) place) );
        place++;
    }
    
    token = FLOAT;    
    fval = (float)sign * ((float)integerPart + decimalPart);
}

void SMFParser :: parseVertex(Model * model) {
    float vals[3];
    for(int i = 0; i < 3; i++) {
        nextToken();
        switch(token) {
            case INTEGER:
                vals[i] = ival;
            break;
            case FLOAT:
                vals[i] = fval;
            break;
        }
    }
    model->addVertex(vec3(vals[0], vals[1], vals[2]));
}

void SMFParser :: parseFace(Model * model) {
    int vals[3];
    for(int i = 0; i < 3; i++) {
        nextToken();
        switch(token) {
            case INTEGER:
                vals[i] = ival;
            break;
        }
    }
    model->addFace(vals[0], vals[1], vals[2]);
}

list<Model*> SMFParser :: parse() {
    list<Model*> objects;
    Model * model = new Model();
    while(token != END) {
        nextToken();
        switch(token) {
            case VERTEX:
            {
                parseVertex(model);
            }
            break;
            case FACE:
            {
                parseFace(model);
            }
            break;
        }
    }
    objects.push_back(model);
    return objects;
}
