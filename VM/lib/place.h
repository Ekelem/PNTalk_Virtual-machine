/**
 * @file    place.h
 * @author  Tomas Lapsansky <xlapsa00@stud.fit.vutbr.cz>
 *
 * @section DESCRIPTION
 *
 * The place class represents main object in Object Oriented Petri Nets, places where the tokens are
 */

#ifndef BP_PLACE_H
#define BP_PLACE_H

#include <regex>

#include "global.h"
#include "cereal/types/string.hpp"

class place {

public:
    std::string name;
    std::list<std::pair<int, std::string>> values;

    /**
     * Default constructor for creation of the new place
     *
     * @param placeName is the name of the new place
     * @param is the default value of the new place
     */
    place(std::string *placeName, std::string *value);

    /**
     * Constructor for fully copying place from object to instanceObject
     *
     * @param toCopy is pointer to original place
     */
    place(place *toCopy);


    /**
     * Checks if input string is defined as SET
     *
     * @param str is input string to check
     * @return true if input string is set, else false
     */
    bool isSet(std::string *str);

};

#endif //BP_PLACE_H
