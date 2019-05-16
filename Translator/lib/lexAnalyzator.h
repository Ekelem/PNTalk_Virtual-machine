/**
 * @file    lexAnalyzator.h
 * @author  Tomas Lapsansky <xlapsa00@stud.fit.vutbr.cz>
 *
 * @section DESCRIPTION
 *
 * The lexAnalyzator class represents lexical analyzator for parsing input code to tokens
 */

#ifndef BP_LEXANALYZATOR_H
#define BP_LEXANALYZATOR_H

#include "global.h"

#include <fstream>
#include <regex>
#include <queue>
#include <climits>

class lexAnalyzator {

public:
    std::ifstream srcFile;
    std::queue <std::tuple<int, std::string>> tokens;
    std::string keywords[50] = {
            "main",
            "class",
            "object",
            "trans",
            "cond",
            "precond",
            "postcond",
            "action",
            "guard",
            "place",
            "method",
            "sync",
            "is_a"
    };

    /**
     * Sets input string number representation from set
     *
     * @param src is input string
     * @return number representation of string from set
     */
    int checkWord(std::string src);

    /**
     * Check if input string is keyword
     *
     * @param src is input string
     * @return true if is, else false
     */
    bool isKeyword(std::string src);

    /**
     * Check if input string is number
     * @param src is input string
     * @return true if is, else false
     */
    bool isNumber(std::string src);

    /**
     * Check if input string is string
     *
     * @param src is input string
     * @return true if is, else false
     */
    bool isString(std::string src);

    /**
     * Check if input string is char
     *
     * @param src is input string
     * @return true if is, else false
     */
    bool isChar(std::string src);

    /**
     * Check if input string is symbol
     *
     * @param src is input string
     * @return true if is, else false
     */
    bool isSymbol(std::string src);

    /**
     * Check if input string is boolean
     *
     * @param src is input string
     * @return true if is, else false
     */
    bool isBool(std::string src);

    /**
     * Check if input string is nil
     *
     * @param src is input string
     * @return true if is, else false
     */
    bool isNil(std::string src);

    /**
     * Check if input string could be function name
     *
     * @param src is input string
     * @return true if could, else false
     */
    bool isFunction(std::string src);

    /**
     * Check if input string could be variable
     *
     * @param src is input string
     * @return true if could, else false
     */
    bool isVariable(std::string src);

    /**
     * Check if input string is pseudo-variable
     *
     * @param src is input string
     * @return true if is, else false
     */
    bool isPseudoVariable(std::string src);

    /**
     * Check if input string is object
     *
     * @param src is input string
     * @return true if is, else false
     */
    bool isObject(std::string src);

    /**
     * Cuts white symbols from end and beginning of the input string
     *
     * @param str is input string
     * @return edited string
     */
    std::string trim(std::string str);

    /**
     * Implicit constructor that sets properties
     *
     * @param file is name of the input file
     */
    explicit lexAnalyzator(std::string file);

    /**
     * Main function of lexical analyzator that cuts input code to tokens and cut
     * it to tokens
     *
     * @return tuple with number representation of token and token string
     */
    std::tuple<int, std::string> getToken();
    int line;

    /**
     * Push token to queue
     *
     * @param token is cString representation of token
     */
    void pushWord(char* token);

    /**
     * Checking terms of source code and parse it to output
     *
     * @param token is cString representation of token
     */
    void checkTerm(char* token);
};


#endif //BP_LEXANALYZATOR_H
