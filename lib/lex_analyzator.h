//
// Created by Tomas Lapsansky
//

#ifndef BP_LEXANALYZATOR_H
#define BP_LEXANALYZATOR_H

#include "global.h"

#include <cstring>
#include <tuple>
#include <fstream>
#include <regex>
#include <queue>
#include <climits>

class lex_analyzator {

private:
    std::ifstream srcFile;
    std::queue <std::tuple<int, std::string>> tokens;
    bool brackets;
    std::string keywords[50] = {
            "main",
            "class",
            "object",
            "trans",
            "cond",
            "precond",
            "postcond",
            "action",
            "place",
            "method",
            "sync",
            "state"
    };

    int checkWord(std::string src);

    bool is_keyword(std::string src);
    bool is_number(std::string src);
    bool is_string(std::string src);
    bool is_symbol(std::string src);
    bool is_bool(std::string src);
    bool is_nil(std::string src);
    bool is_variable(std::string src);
    bool is_pseudo_variable(std::string src);
    bool is_object(std::string src);
    std::string trim(std::string str);

public:
    explicit lex_analyzator(std::string file);
    std::tuple<int, std::string> getToken();

    void pushWord(char* token);
    void checkTerm(char* token);
};


#endif //BP_LEXANALYZATOR_H
