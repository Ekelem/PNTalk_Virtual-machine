/**
 * @file    error.cpp
 * @author  Tomas Lapsansky <xlapsa00@stud.fit.vutbr.cz>
 */

#include "../lib/error.h"
#include <iostream>

void error_message(int code, std::string message) {

    std::cerr << "Exit code " << code << " :\t" << message << std::endl;

    exit(code);
}

void syntax_error(int line) {

    std::cerr << "Syntax error on line " << line << std::endl;

    exit(3);
}

void semanticError(int line) {

    std::cerr << "Semantic error on line " << line << std::endl;

    exit(4);
}