/**
 * @file    error.cpp
 * @author  Tomas Lapsansky <xlapsa00@stud.fit.vutbr.cz>
 */

#include "../lib/error.h"

void errorMessage(int code, std::string message) {

    std::cerr << message << std::endl;
    exit(code);

}