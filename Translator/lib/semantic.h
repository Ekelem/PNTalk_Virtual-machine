/**
 * @file    semantic.h
 * @author  Tomas Lapsansky <xlapsa00@stud.fit.vutbr.cz>
 *
 * @section DESCRIPTION
 *
 * The semantic class represents semantic analyzator for input source code
 */

#ifndef BP_SEMANTIC_H
#define BP_SEMANTIC_H

#include <unordered_map>
#include <unordered_set>
#include <list>
#include <string>
#include <algorithm>
#include <iterator>

typedef std::unordered_set<std::string> functionSet;
typedef std::unordered_map<std::string, functionSet> classMap;
typedef std::unordered_map<std::string, classMap> semanticMap;

class semantic {

private:
    std::string classNameCheck;
    std::string functionNameCheck;

public:
    semanticMap map;
    std::unordered_map<std::string, int> functions;
    std::unordered_multimap<std::string, std::string> places;

    /**
     * Implicit constructor that sets properties
     */
    semantic();

    /**
     * Function adds class to semantic analysis
     *
     * @param name is name of the class
     * @param secondCheck is true if second analysis
     * @return true if correct check
     */
    bool addClass(std::string *name, bool secondCheck);

    /**
     * Function adds method to semantic analysis
     *
     * @param functionName is method name
     * @param params is list of parameters of method
     * @param secondCheck is true if second analysis
     * @return true if correct check
     */
    bool addFunction(std::string *functionName, std::list<std::string> params, bool secondCheck);

    /**
     * Function adds variable to semantic analysis
     *
     * @param varName is variable name
     * @param secondCheck is true if second analysis
     */
    void addVariable(std::string *varName, bool secondCheck);

    /**
     * Check existence of class in semantic analysis
     *
     * @param name is name of the class
     * @return true if correct, else false
     */
    bool checkClass(std::string *name);

    /**
     * Check existence of method and its parameters
     *
     * @param functionName is function name
     * @param params is list of parameters of method
     * @return true if correct, else false
     */
    bool checkFunction(std::string *functionName, std::list<std::string> params);

    /**
     * Check existence of variable
     *
     * @param varName is variable name
     * @param secondCheck is true if second analysis
     * @return true if correct, else false
     */
    bool checkVariable(std::string *varName, bool secondCheck);

    /**
     * Function adds place to semantic analysis
     *
     * @param place is name of the place
     */
    void addPlace(std::string *place);

    /**
     * Check existence of place in semantic analysis
     *
     * @param place is name of the place
     * @param secondCheck is true if second analysis
     * @return true if correct, else false
     */
    bool checkPlace(std::string *place, bool secondCheck);

};

#endif //BP_SEMANTIC_H
