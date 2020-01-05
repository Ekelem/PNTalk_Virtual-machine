/**
 * @file    instance.h
 * @author  Tomas Lapsansky <xlapsa00@stud.fit.vutbr.cz>
 *
 * @section DESCRIPTION
 *
 * The instance class represents created object for simulations in Object Oriented Petri Nets
 */

#ifndef BP_INSTANCE_H
#define BP_INSTANCE_H

#include "global.h"
#include "object.h"
#include "place.h"
#include "outputgen.h"

#include <regex>
#include <sstream>

class vm;   /* Only for pointer */
class instance {

public:
    std::string name;
    object *reference;
    std::list<place*> places;
    int referenceCounter;
    vm *virtualMachine;

    /**
     * Default constructor that sets properties
     *
     * @param var is name of variable for future CLI representation
     * @param VM is retroactive pointer to virtual machine for creating new instances
     */
    explicit instance(std::string *var, vm *VM);

    /**
     * Creates new instance from reference object
     *
     * @param obj is pointer to reference object
     */
    void create(object *obj);

    /**
     * Do one step in Petri Net evolution
     *
     * @param tempStack is global variable for temporary stack
     */
    void step(std::list<std::string> *tempStack);

    /**
     * Delete instance and his direct children
     */
    void deleteInstance();

private:
    /**
     * Check condition of COND property in reference object
     *
     * @param transition is pointer to the transition to check
     * @param variables is hashmap of variables in transition
     * @return true if condition is true, else false
     */
    bool checkCond(trans *transition, std::unordered_map<std::string, std::pair<int, std::string>> *variables);

    /**
     * Check conitions of PRECOND property in reference object
     *
     * @param transition is pointer to transition to check
     * @param variables is hashmap of variables in transition
     * @return true if precondition is true, else false
     */
    bool checkPrecond(trans *transition, std::unordered_map<std::string, std::pair<int, std::string>> *variables);

    /**
     * Check GUARD of transition in reference object
     *
     * @param transition is pointer to transition to check
     * @param variables is hashmap of variables in transition
     * @param tempStack is reference to global temporary stack
     * @return true if guard is true, else false
     */
    bool checkGuard(trans *transition, std::unordered_map<std::string, std::pair<int, std::string>> *variables, std::list<std::string> *tempStack);

    /**
     * Doing ACTION from reference object
     *
     * @param transition is pointer to transition to check
     * @param variables is hashmap of variables in transition
     * @param tempStack is reference to global temporary stack
     */
    void doAction(trans *transition, std::unordered_map<std::string, std::pair<int, std::string>> *variables, std::list<std::string> *tempStack);

    /**
     * Delete properties defined in precondition and sets new defined in postcondition from reference object
     *
     * @param transition is pointer to transition to check
     * @param variables is hashmap of variables in transition
     * @param tempStack is reference to global temporary stack
     */
    void doneStep(trans *transition, std::unordered_map<std::string, std::pair<int, std::string>> *variables, std::list<std::string> *tempStack);

    /**
     * Check and do instruction from ACTION
     *
     * @param instruction is string of instruction
     * @param variables is hashmap of variables in transition
     * @param tempStack is reference to global temporary stack
     */
    void doInstruction(std::string *instruction, std::unordered_map<std::string, std::pair<int, std::string>> *variables, std::list<std::string> *tempStack);

    /**
     * Get variable value
     *
     * @param var is name of variable to get
     * @param variables is hashmap of variables in transition
     * @return pair representing value of variable
     */
    std::pair<int, std::string> getVariable(std::string *var, std::unordered_map<std::string, std::pair<int, std::string>> *variables);

    /**
     * Check if string could be variable
     *
     * @param str is string to check
     * @return true if could be variable, else false
     */
    bool isVariable(std::string *str);

    /**
     * Call function of this instance
     *
     * @param str is function name
     * @param tempStack is reference to global temporary stack
     */
    void callFunction(std::string *str, std::list<std::string> *tempStack);
};


#endif //BP_INSTANCE_H
