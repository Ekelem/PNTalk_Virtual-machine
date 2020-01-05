/**
 * @file    vm.h
 * @author  Tomas Lapsansky <xlapsa00@stud.fit.vutbr.cz>
 *
 * @section DESCRIPTION
 *
 * The VM class represents virtual machine for simulation of Object Oriented Petri Nets
 */

#ifndef BP_VM_H
#define BP_VM_H

#include "global.h"
#include "object.h"
#include "instance.h"
#include "outputgen.h"

#include <getopt.h>
#include <fstream>
#include <sstream>
#include <map>


class vm {

public:
    std::list<object*> objects;
    std::list<instance*> objectInstance;
    std::string mainName;
    std::ifstream srcFile;
    std::string parseLine;

    outputgen archiver;

    std::list<std::string> tempStack;
    bool finished;

    /**
     * Default constructor that sets property finished to false
     */
    explicit vm();

    /**
     * Function that parses input bytecode to simulation structure
     *
     * @param *file is string with name of the imput file
     */
    void parseInput(std::string *file);

    /**
     * Funtion that checks if parseLine contains keywords
     *
     * @return true if parseLine contains keyword, else false
     */
    bool hasKeyword();

    /**
     * Checks first line of the imput bytecode
     */
    void getMain();

    /**
     * Sets main object of the simulation
     */
    void setMain();

    /**
     * Load class to simulation structure
     */
    void getClasses();


    /**
     * Creates new instance of class and allocs place for it
     *
     * @param className sets the name of the created class
     * @return pointer to new class
     */
    object* createClass(std::string *className);

    /**
     * Loads content of the class and add it to it
     *
     * @param newObject is pointer to the class instance
     */
    void fillClass(object *newObject);

    /**
     * Adding new method to the class
     *
     * @param newObject is pointer to the class
     * @param functionName is name of the new created method
     * @param params is list of the names of parameters for new method
     * @param sync depends if new method is function or synchronization port
     */
    void addMethod(object *newObject, std::string *functionName, std::list<std::string> *params, bool sync);


    /**
     * Takes one step in virtual machine,
     * calls function step for every instance object
     */
    void step();

    /**
     * Run steps to the point, where OOPN doesn't evolute more
     */
    void run();

    /**
     * Show details of OOPN at the time to standard output
     */
    void detail();

    /**
     * Show detail for CLI interaction
     */
    void help();

    /**
     * Quit program correctly and clear created structures
     */
    void quit();

};

#endif //BP_VM_H
