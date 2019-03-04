//
// Created by Tomas Lapsansky
//

using namespace std;

#include "../lib/lex_analyzator.h"

lex_analyzator::lex_analyzator(std::string file) {

    brackets = false;
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
 *  13  symbol
 *  14  bool
 *  15  nil
 *  16  pseudo-variable
 *  17  variable/function name
 *  18  object
 *  20  term
 *
 *  99  undefined
 */

tuple<int, string> lex_analyzator::getToken() {

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

                    /* Maybe TODO extract function */
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

    tokens.push(tuple<int, string>(2, "\tENDL"));

    tuple<int, string> ret = tokens.front();

    if(get<0>(ret) != 0) {
        tokens.pop();
    }

    return ret;
}

int lex_analyzator::checkWord(std::string src) {

    if(is_keyword(src)) {
        return 1;
    } else if(is_number(src)) {
        return 11;
    } else if(is_string(src)) {
        return 12;
    } else if(is_symbol(src)) {
        return 13;
    } else if(is_bool(src)) {
        return 14;
    } else if(is_nil(src)) {
        return 15;
    } else if(is_pseudo_variable(src)) {
        return 16;
    } else if(is_variable(src)) {
        return 17;
    } else if(is_object(src)) {
        return 18;
    }

    return 99;
}

void lex_analyzator::pushWord(char *token) {

    //TODO analyze keyword

    std::string str = token;

    if(str == ",") {
        tokens.push(tuple<int, string>(3, token));
    } else {
        tokens.push(tuple<int, string>(checkWord(token), token));
    }

}

void lex_analyzator::checkTerm(char *token) {

    //TODO analyze term

    tokens.push(tuple<int, string>(20, token));
}

std::string lex_analyzator::trim(string str)
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

    return str.substr(first, last+1);
}

bool lex_analyzator::is_keyword(std::string src) {

    for(const std::string word : lex_analyzator::keywords) {
        if(src == word) {
            return true;
        }
    }

    return false;
}

bool lex_analyzator::is_number(string src) {

    return regex_match (src, regex(R"(( ([0-9]+(\.[0-9]*)?) | (\.[0-9]+) )( (e|E)(\+|\-)[0-9]+(\.[0-9])* ))"));
}

bool lex_analyzator::is_string(string src) {

    return regex_match (src, regex("\'.*\'"));
}

bool lex_analyzator::is_symbol(string src) {

    return regex_match (src, regex("$.+") );
}

bool lex_analyzator::is_bool(string src) {
    return src == "true" || src == "false";
}

bool lex_analyzator::is_nil(string src) {
    return src == "nil";
}

bool lex_analyzator::is_pseudo_variable(string src) {
    return src == "self" || src == "super";
}

bool lex_analyzator::is_variable(string src) {
    return regex_match (src, regex("[a-z]([a-z]|[A-Z]|[0-9])*") );
}

bool lex_analyzator::is_object(string src) {
    return regex_match (src, regex("[A-Z]([a-z]|[A-Z]|[0-9])*") );
}