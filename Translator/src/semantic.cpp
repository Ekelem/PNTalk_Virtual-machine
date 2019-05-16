/**
 * @file    semantic.cpp
 * @author  Tomas Lapsansky <xlapsa00@stud.fit.vutbr.cz>
 */

#include "../lib/semantic.h"
#include <iostream>

semantic::semantic() = default;

bool semantic::addClass(std::string *name, bool secondCheck) {

    classNameCheck = *name;

    if(!secondCheck) {
        map.insert({*name, classMap()});
    } else {
        return checkClass(name);
    }

    return true;
}

bool semantic::addFunction(std::string *functionName, std::list<std::string> params, bool secondCheck) {

    functionNameCheck = *functionName;

    if(!secondCheck) {

        if(functionNameCheck[functionNameCheck.size() - 1] == ':') {
            functionNameCheck.erase(functionNameCheck.size() - 1, 1);
        }

        classMap map1 = map[classNameCheck];
        map1.insert({functionNameCheck, functionSet()});

        /* Update */
        map[classNameCheck] = map1;

        if(!params.empty()) {

            std::string name = classNameCheck + "." + functionNameCheck;

            /* Add to functions */

            functionSet set = map1[functionNameCheck];

            /* Add insert vars to function variables */
            for(std::string param : params) {
                set.insert(param);
            }
            functions.insert({name, params.size()});

            map1[functionNameCheck] = set;
            map[classNameCheck] = map1;
        }

    }

    return true;
}

void semantic::addVariable(std::string *varName, bool secondCheck) {

    if(!secondCheck) {
        classMap map1 = map[classNameCheck];
        functionSet set = map1[functionNameCheck];

        set.insert(*varName);

        /* Update */
        map1[functionNameCheck] = set;
        map[classNameCheck] = map1;
    }

}

void semantic::addPlace(std::string *place) {

    if(functionNameCheck == "object") {
        places.insert({classNameCheck, *place});
    } else {
        places.insert({classNameCheck + "." + functionNameCheck, *place});
    }

}

bool semantic::checkClass(std::string *name) {

    return !(map.find(*name) == map.end());

}

bool semantic::checkFunction(std::string *functionName, std::list<std::string> params) {

    classMap map1 = map[classNameCheck];

    for (std::pair<std::string, classMap> elementMap : map) {
        if(std::get<1>(elementMap).find(*functionName) != map1.end()) {
            int functionParams = functions[std::get<0>(elementMap) + "." + *functionName];

            return (functionParams == params.size());
        }
    }

    return false;
}

bool semantic::checkVariable(std::string *varName, bool secondCheck) {

    if(!secondCheck) {
        classMap map1 = map[classNameCheck];
        functionSet set = map1[functionNameCheck];

        return !(set.find(*varName) == set.end());
    }

    return true;
}

bool semantic::checkPlace(std::string *place, bool secondCheck) {

    if(secondCheck) {
        /* Search in object */
        for(std::pair<std::string, std::string> checker: places) {
            if(std::get<0>(checker) == classNameCheck) {
                if(std::get<1>(checker) == *place) {
                    return true;
                }
            } else if(std::get<0>(checker) == (classNameCheck + "." + functionNameCheck)) {
                if(std::get<1>(checker) == *place) {
                    return true;
                }
            }
        }

        return false;
    }

    return true;
}