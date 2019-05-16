/**
 * @file    method.cpp
 * @author  Tomas Lapsansky <xlapsa00@stud.fit.vutbr.cz>
 */

#include "../lib/method.h"

method::method(std::string *methodName, bool sync) {

    this->name = *methodName;
    this->isSync = sync;

}

void method::deleteMethod() {
    for(trans *obj: transitions) {
        obj->deleteTrans();
    }
    transitions.clear();

    delete this;
}