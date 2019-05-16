/**
 * @file    trans.h
 * @author  Tomas Lapsansky <xlapsa00@stud.fit.vutbr.cz>
 *
 * @section DESCRIPTION
 *
 * The trans class represents transitions in methods of Object Oriented Petri Nets
 */

#ifndef BP_TRANS_H
#define BP_TRANS_H

#include <unordered_map>
#include "global.h"
#include "place.h"

class trans {

public:
    std::string name;
    std::list<place*> places;
    std::list<std::pair<std::string, std::list<std::string>>> cond;
    std::list<std::pair<std::string, std::list<std::string>>> precond;
    std::list<std::string> guard;
    std::list<std::string> action;
    std::list<std::pair<std::string, std::list<std::string>>> postcond;

    /**
     * Default constructor that sets properties
     *
     * @param transName represents name of new transition
     */
    explicit trans(std::string *transName);

    /**
     * SetCond adds special condition to property cond, precond or postcond
     *
     * @param condStr represents name of the condition property (cond, precond or postcond)
     * @param condPair condition pair with name of the place and his properties to be checked in condition
     */
    void setCond(std::string *condStr, std::pair<std::string, std::list<std::string>> condPair);

    /**
     * Delete transition and his direct children
     */
    void deleteTrans();
};


#endif //BP_TRANS_H
