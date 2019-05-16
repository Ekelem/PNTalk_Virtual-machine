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

void error_message(int code, std::string message);
void syntax_error(int line);
void semanticError(int line);

#endif //BP_ERROR_H
