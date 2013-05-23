#include <cstring>
#include <cstdlib>
#include "BezierPatchParser.h"
#include "Angel.h"

#include <iostream>
#include <cmath>

using namespace std;

enum { INTEGER, FLOAT, CONTROL_POINT, END };

 BezierPatchParser :: BezierPatchParser(char * filename) {
    _file = new ifstream(filename, ifstream::in);
    currentChar = '\0';
}

void BezierPatchParser :: nextToken() {
    while(_file->good()) {
        int c = _file->get();
        currentChar = (char) c;
        if(c <= '9' && c >= '0' || c == '-') {
            readNumber();
            return;
        }
        else if(c == '\n') {
            token = CONTROL_POINT;
            return;
        }
    }
    token = END;
}

void BezierPatchParser :: readNumber() {
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

void BezierPatchParser :: parseControlPoint(BezierPatch * patch) {
    float vals[3];
    for(int i = 0; i < 3; i++) {
        nextToken();
        switch(token) {
            case FLOAT:
                vals[i] = fval;
            break;
            default:
                token = END;
                return;
                
        }
    }
    token = CONTROL_POINT;
    patch->addControlPoint(vec3(vals[0], vals[1], vals[2]));
}

BezierPatch* BezierPatchParser :: parse() {
    BezierPatch * patch = new BezierPatch();
    token = CONTROL_POINT;
    while(token != END) {
        switch(token) {
            case CONTROL_POINT:
            {
                parseControlPoint(patch);
            }
            break;
        }
      //  nextToken();
    }
    return patch;
}
