//
// Created by Tomas Lapsansky on 2019-01-22.
//

#ifndef BP_LEXANALYZATOR_H
#define BP_LEXANALYZATOR_H

#include "global.h"

#include <tuple>
#include <fstream>

class lex_analyzator {

private:
    ofstream srcFile;
public:
    lex_analyzator(string file);
    tuple<int, string> getToken();
};


#endif //BP_LEXANALYZATOR_H
