//
// Created by Tomas Lapsansky on 2019-02-10.
//

#include "../lib/error.h"
#include <iostream>

void error_message(int code, std::string message) {

    std::cerr << "Exit code " << code << " :\t" << message << std::endl;

    exit(code);
}