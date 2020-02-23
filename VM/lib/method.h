/**
 * @file    method.h
 * @author  Tomas Lapsansky <xlapsa00@stud.fit.vutbr.cz>
 *
 * @section DESCRIPTION
 *
 * The method class represents method or implicit method from objects in Object Oriented Petri Nets
 */

#ifndef BP_METHOD_H
#define BP_METHOD_H

#include "global.h"
#include "place.h"
#include "trans.h"

class method {

public:
    std::string name;
    std::list<std::string> params;
    std::list<place *> places;
    std::list<trans*> transitions;
    bool isSync;

    /**
     * Default constructor that sets properties
     *
     * @param methodName represents name of new method
     * @param sync decides, if method is function or sync port
     */
    explicit method(std::string *methodName, bool sync);

    /**
     * Delete method and his direct children
     */
    void deleteMethod();

};


#endif //BP_METHOD_H
