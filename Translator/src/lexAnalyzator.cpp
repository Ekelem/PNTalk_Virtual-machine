/**
 * @file    lexAnalyzator.cpp
 * @author  Tomas Lapsansky <xlapsa00@stud.fit.vutbr.cz>
 */

using namespace std;

#include "../lib/lexAnalyzator.h"

lexAnalyzator::lexAnalyzator(std::string file) {

    line = 0;
    srcFile.open(file);

    if(!srcFile.is_open()) {
        error_message(10, "Can't open file");
    }
}

/*
 *
 * @return
 *  0   EOF
 *  1   keyword
 *  2   new line
 *  3   delimeter (,)
 *  11  number
 *  12  string
 *  13  char
 *  14  symbol
 *  15  bool
 *  16  nil
 *  17  pseudo-variable
 *  18  function
 *  19  variable/function name
 *  20  object
 *  30  term
 *  31  empty-term
 *  32  list
 *  33  multiset
 *
 *  99  undefined
 */

tuple<int, string> lexAnalyzator::getToken() {

    string line;

    /* Pop loaded tokens */
    if(!tokens.empty()) {
        tuple<int, string> ret = tokens.front();

        if(get<0>(ret) != 0) {
            tokens.pop();
        }

        return ret;
    }

    /* Load line */
    do {
        getline(srcFile, line);
        line = trim(line);

        if(srcFile.eof()) {
            tokens.push(tuple<int, string>(0, "EOF"));
            break;
        }
        this->line++;
    } while(line.empty());

    /* Parse line */

    /* No brackets == only keywords */
    if((line.find('(') == string::npos) && (line.find('{') == string::npos)) {

        char *copy = (char*)malloc(line.size());
        const char* str = line.c_str();
        strcpy(copy, str);

        char *token = std::strtok(copy, " ");

        while (token != nullptr) {
            pushWord(token);
            token = std::strtok(nullptr, " ");
        }

        free(copy);

    /* with brackets parsing */
    } else if(line.find('{') != string::npos) {

        unsigned long pos = line.find('{');

        string keywords = line.substr(0, pos);
        string brackets = line.substr(pos);

        /* Push keywords */
        char *copy = (char*)malloc(line.size());

        const char* str = keywords.c_str();
        strcpy(copy, str);

        char *token = std::strtok(copy, " ");

        while (token != nullptr) {
            pushWord(token);
            token = std::strtok(nullptr, " ");
        }

        /* Push brackets */
        const char *strBrackets = brackets.c_str();
        strcpy(copy, strBrackets);

        checkTerm(copy);

        free(copy);

    } else {

        bool inBrackets = false;
        unsigned long brackets = 0;
        unsigned long lastChar = 0;

        /* Old school */
        for(unsigned long i = 0; i < line.size(); i++) {

            if(!inBrackets) {

                if(line[i] == '(') {            /* Bracket */

                    char *copy = (char*)malloc(line.size());

                    string sub = line.substr(lastChar, i-lastChar);
                    const char* str = sub.c_str();
                    strcpy(copy, str);

                    pushWord(copy);

                    free(copy);

                    inBrackets = true;
                    brackets++;
                    lastChar = i;

                } else if(line[i] == ' ') {     /* Space */

                    /* Duplicity = more clean code */

                    string sub = line.substr(lastChar, i-lastChar);

                    sub = trim(sub);

                    /* Check if only space */
                    if(sub.empty()) {
                        break;
                    }

                    char *copy = (char*)malloc(line.size());

                    const char* str = sub.c_str();
                    strcpy(copy, str);

                    pushWord(copy);

                    free(copy);

                    lastChar = i+1;

                }

            } else {

                if(line[i] == '(') {
                    brackets++;
                } else if(line[i] == ')') {
                    brackets--;
                }

                /* Extract */
                if(brackets == 0) {

                    /* Possible extract function */
                    char *copy = (char*)malloc(line.size());

                    string sub = line.substr(lastChar, i-lastChar+1);
                    const char* str = sub.c_str();
                    strcpy(copy, str);

                    checkTerm(copy);

                    free(copy);

                    inBrackets = false;
                    lastChar = i+1;

                }
            }
        }
    }

    tokens.push(tuple<int, string>(2, "ENDL"));

    tuple<int, string> ret = tokens.front();

    if(get<0>(ret) != 0) {
        tokens.pop();
    }

    return ret;
}

int lexAnalyzator::checkWord(std::string src) {

    if(isKeyword(src)) {
        return 1;
    } else if(isNumber(src)) {
        return 11;
    } else if(isString(src)) {
        return 12;
    } else if(isChar(src)) {
        return 13;
    } else if(isSymbol(src)) {
        return 14;
    } else if(isBool(src)) {
        return 15;
    } else if(isNil(src)) {
        return 16;
    } else if(isPseudoVariable(src)) {
        return 17;
    } else if(isFunction(src)) {
        return 18;
    } else if(isVariable(src)) {
        return 19;
    } else if(isObject(src)) {
        return 20;
    }

    return 99;
}

void lexAnalyzator::pushWord(char *token) {

    std::string str = token;

    if(str == ",") {
        tokens.push(tuple<int, string>(3, token));
    } else {
        tokens.push(tuple<int, string>(checkWord(token), token));
    }

}

void lexAnalyzator::checkTerm(char *token) {

    std::string str = token;

    if(str[0] == '(' && str[str.size() - 1] == ')') {
        /* This can be analyzed */

        /* Erase brackets */
        str.erase(0, 1);
        str.erase(str.size() - 1, 1);

        str = trim(str);

        if(str.find("‘") != string::npos || (str.find(',') != string::npos && str.find('(') == string::npos)) {    /* Multiset */

            tokens.push(tuple<int, string>(33, "set(" + str + ")"));
            return;
        }

        if(str[0] == '(') {     /* List */

            /* Erase brackets */
            str.erase(0, 1);
            str.erase(str.size() - 1, 1);

            tokens.push(tuple<int, string>(32, token));

            char *copy = (char*)malloc(str.size());

            const char* str_c = str.c_str();
            strcpy(copy, str_c);

            char *listItem = std::strtok(copy, ",");

            while (listItem != nullptr) {

                std::string strg = listItem;
                strg = trim(strg);
                int ret = checkWord(strg);

                tokens.push(tuple<int, string>(ret, strg));

                listItem = std::strtok(nullptr, ",");

                if(listItem != nullptr)
                    tokens.push(tuple<int, string>(3, ","));
            }

            return;
        }

        if(str.empty()) {   /* Empty brackets */
            tokens.push(tuple<int, string>(31, token));
            return;
        }

        int tokenAnalyze = checkWord(str);
        if(tokenAnalyze != 99) {    /* Push if you can */
            tokens.push(tuple<int, string>(tokenAnalyze, token));
            return;
        }

        /* Check term */

    } else if(str[0] == '{' && str[str.size() - 1] == '}') {

        tokens.push(tuple<int, string>(30, token));

    } else {

        tokens.push(tuple<int, string>(99, token));
        return;
    }

}

std::string lexAnalyzator::trim(string str)
{
    int i = 0;
    int first = 0;
    int last = (int)str.size() - 1;

    while(i < str.size()) {
        if(isspace(str[i])) {
            if(first == i)
                first++;
            else
                i = INT_MAX;
        }

        i++;
    }

    i = str.empty() ? - 1 : (int)str.size() - 1;

    while(i >= 0) {
        if(isspace(str[i])) {
            if(last == i)
                last--;
            else
                i = INT_MIN + 1;
        }

        i--;
    }

    string *retStr = new string;
    *retStr = str.substr(first, last+1);

    return *retStr;
}

bool lexAnalyzator::isKeyword(std::string src) {

    for(const std::string word : lexAnalyzator::keywords) {
        if(src == word) {
            return true;
        }
    }

    return false;
}

bool lexAnalyzator::isNumber(string src) {

    return regex_match (src, regex(R"((\+|\-)?([0-9]+(\.[0-9]*)?(e(\+|\-)?[0-9]+)?))"));
}

bool lexAnalyzator::isString(string src) {

    return regex_match (src, regex("\'.*\'"));
}

bool lexAnalyzator::isChar(string src) {

    return regex_match (src, regex("#.+") );
}

bool lexAnalyzator::isSymbol(string src) {

    return regex_match (src, regex("$.+") );
}

bool lexAnalyzator::isBool(string src) {
    return src == "true" || src == "false";
}

bool lexAnalyzator::isNil(string src) {
    return src == "nil";
}

bool lexAnalyzator::isPseudoVariable(string src) {
    return src == "self" || src == "super";
}

bool lexAnalyzator::isFunction(std::string src) {
    return regex_match (src, regex("[a-z]([a-z]|[A-Z]|[0-9])*:") );
}

bool lexAnalyzator::isVariable(string src) {
    return regex_match (src, regex("[a-z]([a-z]|[A-Z]|[0-9])*") );
}

bool lexAnalyzator::isObject(string src) {
    return regex_match (src, regex("[A-Z]([a-z]|[A-Z]|[0-9])*") );
}