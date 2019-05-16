/**
 * @file    object.h
 * @author  Tomas Lapsansky <xlapsa00@stud.fit.vutbr.cz>
 *
 * @section DESCRIPTION
 *
 * The object class represents the reference object for instances in Object Oriented Petri Nets
 */

#ifndef BP_OBJECT_H
#define BP_OBJECT_H

#include "global.h"
#include "method.h"
#include "trans.h"

class object {

public:
    std::string name;
    method *mainMethod;
    std::list<method*> methods;

    /**
     * Default constructor that sets properties
     *
     * @param name represents name of new object
     */
    explicit object(std::string *name);

    /**
     * Funtion adds method to the parent object
     *
     * @param methodName is name of the new method
     * @param params are parameters of the new method
     * @param sync decides, if method is function or sync port
     */
    void addMethod(std::string *methodName, std::list<std::string> *params, bool sync);

    /**
     * Function adds place pointer from the method to the parent object
     *
     * @param placeName is the name of the place
     * @param value is the implicit value of place
     * @param function is the name of child function
     */
    void addPlace(std::string *placeName, std::string *value, std::string *function);

    /**
     * Function adds transition to the child method
     *
     * @param transName is the name of the new transition
     * @param function is the name of child function
     * @return pointer to the new transition
     */
    trans* addTrans(std::string *transName, std::string *function);

    /**
     * Delete object and his direct children
     */
    void deleteObject();
};


#endif //BP_OBJECT_H
