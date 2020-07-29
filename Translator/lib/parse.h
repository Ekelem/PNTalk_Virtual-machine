/**
 * @file    parse.h
 * @author  Tomas Lapsansky <xlapsa00@stud.fit.vutbr.cz>
 *
 * @section DESCRIPTION
 *
 * The parse class represents syntax analyzator for input source code
 */

#ifndef BP_PARSE_H
#define BP_PARSE_H

#include "global.h"
#include "lexAnalyzator.h"
#include "semantic.h"
#include <regex>
#include <string>
#include <stack>
#include <list>


class parse {

private:
    /**
     * Function parsing main line of bytecode
     */
    void mainLine();

    /**
     * Function parsing class definition
     */
    void classCheck();

    /**
     * Function parsing class body
     *
     * @return 0 if correct
     */
    int objectsCheck();

    /**
     * Function check method definition
     *
     * @return 0 if correct
     */
    int methodsCheck();

    /**
     * Function check sync method definition
     *
     * @return 0 if correct
     */
    int syncCheck();

    /**
     * Function parsing term in bytecode
     */
    void parseTerm();

    /**
     * Parsing transition or sync definition
     *
     * @param isTrans is true if parsing transition, false if parsing sync
     */
    void parseTrans(bool isTrans);

    /**
     * Parsing conditions
     *
     * @param isPlace is true if parsing place
     * @param guard is true if parsing guard
     */
    void checkConditions(bool isPlace, bool guard);

    /**
     * Checking if token is term
     *
     * @return true if token is term, else false
     */
    bool isTerm();

    /**
     * Parsing conditions in COND, PRECOND and POSTCOND of input string
     *
     * @param str is input string
     * @return 0 if correct
     */
    int conditionCheck(std::string str);

    /**
     * Parsing and calculating of term
     *
     * @param guard if in guard
     */
    void termCalc(bool guard);

    std::tuple<int, std::string> token;
    lexAnalyzator *lex;
    semantic *sem;
    std::ostream * _output = &std::cout;

    /**
     * Check if input string is operator
     *
     * @param str is input string
     * @return true if correct, else false
     */
    bool isOperator(std::string str);

    /**
     * Checking priority of operator in input string
     *
     * @param str is input string
     * @return integer value of operator
     */
    int getPriority(std::string str);

    /**
     * Convert infix operation from input string to postfix notation
     *
     * @param infix is input string in infix notation
     * @return converted operators and operands in queue
     */
    std::queue<std::string> infixToPostfix(std::string infix);

    /**
     * Checking if beginning of the input string if matches regular expression in reg
     *
     * @param str is input string
     * @param reg is input regular expression
     * @return position of last character to match, else -1
     */
    int matchPart(const std::string str, const std::string reg);

    /**
     * Printing input string to standard output
     *
     * @param str is input string
     * @param newLine is true if last line of code
     */
    void outputCode(const std::string str, bool newLine);

public:
    bool secondCheck;

    /**
     * Implicit constructor that sets properties
     *
     * @param file is name of the input file
     */
    explicit parse(std::string file);

    /**
     * Set properties for second analysis
     *
     * @param file is name of the input file
     */
    void secondAnalysis(std::string file);

    /**
     * Loading token by calling lexical analysis
     */
    void getToken();

    /**
     * Starting first analysis
     */
    void start();

    void translate(std::string inputCodePath);

    /**
     * Set output stream
     * 
     * @param output is out stream
     */
    void setOutput(std::ostream & output);

};


#endif //BP_PARSE_H
