/**
 * @file    trans.cpp
 * @author  Tomas Lapsansky <xlapsa00@stud.fit.vutbr.cz>
 */

#include "../lib/trans.h"

trans::trans(std::string *transName) {
    this->name = *transName;
}

void trans::setCond(std::string *condStr, std::pair<std::string, std::list<std::string>> condPair) {

    auto *newPair = (std::pair<std::string, std::list<std::string>>*)malloc(sizeof(std::pair<std::string, std::list<std::string>>));
    new (newPair) std::pair<std::string, std::list<std::string>>;

    auto *newList = (std::list<std::string>*)malloc(sizeof(std::list<std::string>));
    new (newList) std::list<std::string>;

    newPair->first = condPair.first;
    for (std::string item: condPair.second) {
        newPair->second.push_back(item);
    }

    if(*condStr == "COND") {
        this->cond.push_back(*newPair);
    } else if(*condStr == "PRECOND") {
        this->precond.push_back(*newPair);
    } else {
        /* POSTCOND */
        this->postcond.push_back(*newPair);
    }
}

void trans::deleteTrans() {
    /*for(place *obj: places) {
        delete obj;
    }*/
    places.clear();

    delete this;
}