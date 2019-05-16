/**
 * @file    place.cpp
 * @author  Tomas Lapsansky <xlapsa00@stud.fit.vutbr.cz>
 */

#include "../lib/place.h"

place::place(std::string *placeName, std::string *value) {

    this->name = *placeName;

    /* Convert to list */

    if(isSet(value)) {
        std::string str = *value;

        str.erase(0, 4);
        str.erase(str.size() - 1, 1);

        std::string element;
        int pos1;
        int pos2;
        bool cycle = true;

        while(cycle) {
            pos1 = str.find("‘");
            pos2 = str.find(',');

            if(pos1 == std::string::npos && pos2 == std::string::npos) {
                cycle = false;
            }

            if(cycle) {
                if((pos2 == std::string::npos) || (pos1 != std::string::npos && (pos1 < pos2))) {
                    /* Delimeter ‘ */
                    element = str.substr(0, pos1);
                    str.erase(0, pos1 + 3);
                } else {
                    /* Delimeter , */
                    element = str.substr(0, pos2);
                    str.erase(0, pos2 + 2);
                }

                this->values.emplace_back(std::pair<int, std::string>(0, element));
            } else {
                this->values.emplace_back(std::pair<int, std::string>(0, str));
            }
        }

    } else {
        if(!value->empty()) {
            this->values.emplace_back(std::pair<int, std::string>(0, *value));
        }
    }
}

place::place(place *toCopy) {

    this->name = toCopy->name;
    this->values = toCopy->values;
}

bool place::isSet(std::string *str) {
    return regex_match(*str, std::regex("set(.*)"));
}