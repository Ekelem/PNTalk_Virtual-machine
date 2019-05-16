/**
 * @file    object.cpp
 * @author  Tomas Lapsansky <xlapsa00@stud.fit.vutbr.cz>
 */

#include "../lib/object.h"

object::object(std::string *name) {

    this->name = *name;
    this->mainMethod = nullptr;
    this->methods = std::list<method*>();

}

void object::addMethod(std::string *methodName, std::list<std::string> *params, bool sync) {

    method *newMethod = (method*)malloc(sizeof(method));
    new (newMethod) method(methodName, sync);

    this->methods.push_back(newMethod);

    if(*methodName == "object") {
        this->mainMethod = newMethod;
    }

    if(params != nullptr) {
        for(std::string param: *params) {
            newMethod->params.push_back(param);
        }
    }
}

void object::addPlace(std::string *placeName, std::string *value, std::string *function) {

    place *newPlace = (place*)malloc(sizeof(place));
    new (newPlace) place(placeName, value);

    method *foundMethod;
    method *objectMethod;

    /* Get method */
    for(method *searchMethod: methods) {
        if(searchMethod->name == *function) {
            foundMethod = searchMethod;
        }

        if(searchMethod->name == "object") {
            objectMethod = searchMethod;
        }
    }

    foundMethod->places.push_back(newPlace);

    if(foundMethod != objectMethod) {

        objectMethod->places.push_back(newPlace);
    }

}

trans* object::addTrans(std::string *transName, std::string *function) {

    trans *newTrans = (trans*)malloc(sizeof(trans));
    new (newTrans) trans(transName);

    method *foundMethod;

    /* */
    for(method *searchMethod: methods) {
        if(searchMethod->name == *function) {
            foundMethod = searchMethod;
        }
    }

    foundMethod->transitions.push_back(newTrans);

    return newTrans;
}

void object::deleteObject() {
    for(method *obj: methods) {
        obj->deleteMethod();
    }
    methods.clear();
    delete this;
}