/**
 * @file    error.h
 * @author  Tomas Lapsansky <xlapsa00@stud.fit.vutbr.cz>
 *
 * @section DESCRIPTION
 *
 * Represents function for error logging
 */

#ifndef BP_ERROR_H
#define BP_ERROR_H

#include <string>
#include <iostream>

/**
 * General method for displaying error logs
 *
 * @param code represents exit code of program
 * @param message is message to be printed to standard error output
 */
void errorMessage(int code, std::string message);

#endif //BP_ERROR_H
