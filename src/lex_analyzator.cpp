//
// Created by Tomas Lapsansky on 2019-01-22.
//

#include "../lib/lex_analyzator.h"

lex_analyzator::lex_analyzator(std::string file) {

    srcFile.open(file);

    if(!srcFile.is_open()) {
        exit(1);    // TODO error message
    }
}

tuple<int, string> lex_analyzator::getToken() {

}