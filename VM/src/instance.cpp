/**
 * @file    instance.cpp
 * @author  Tomas Lapsansky <xlapsa00@stud.fit.vutbr.cz>,
 *          Erik Kelemen <xkelem01@stud.fit.vutbr.cz>
 */

#include "../lib/instance.h"
#include "../lib/vm.h"

instance::instance(std::string *var, vm *VM) {
    this->name = *var;
    this->reference = nullptr;
    this->referenceCounter = 0;     /* Garbage collector will collect it when value will be 0 */
    this->virtualMachine = VM;      /* Neccesary for creating new instances */
}

void instance::create(object *obj) {

    this->reference = obj;

    for(method* call: obj->methods) {
        if(call->name == "object") {
            for(place* plc: call->places) {
                auto *newPlace = (place*)malloc(sizeof(place));
                new (newPlace) place(plc);


                places.push_back(std::shared_ptr<place>(new place(plc)));
            }
        }
    }
}

void instance::recordInitial() {
    virtualMachine->archiver.record(archiveInitial(name, reference->name, places));
}

void instance::step(std::list<std::string> *tempStack) {

    for(trans *transition: reference->mainMethod->transitions) {

        std::unordered_map<std::string, std::pair<int, std::string>> variables;

        virtualMachine->archiver.startTrans(stackTransition(transition->name, name, reference->name, places));

        if(!checkCond(transition, &variables)) {
            continue;
        }

        if(!checkPrecond(transition, &variables)) {
            continue;
        }

        if(!checkGuard(transition, &variables, tempStack)) {
            continue;
        }
        doAction(transition, &variables, tempStack);
        doneStep(transition, &variables, tempStack);
        virtualMachine->archiver.stopTrans(stackTransition(transition->name, name, reference->name, places));
        this->virtualMachine->finished = false;
    }
}

bool instance::isVariable(std::string *str) {
    if(regex_match(*str, std::regex("TEMP.*"))) {
        /* For terms */
        return true;
    } else {
        return regex_match(*str, std::regex("[a-z]([a-z]|[A-Z]|[0-9])*"));
    }
}

bool instance::checkCond(trans *transition, std::unordered_map<std::string, std::pair<int, std::string>> *variables) {

    for(std::pair<std::string, std::list<std::string>> trans: transition->cond) {
        /* Check every COND in transition */

        for(auto & toCheck: this->places) {
            if(toCheck->name == trans.first) {
                /* If this is the searched place, check it */
                if((toCheck->values).empty()) {
                    /* Has no value */
                    return false;
                }

                int idx = 0;

                for(std::string check: trans.second) {
                    if(isVariable(&check)) {
                        /* Save to var */

                        std::list<std::pair<int, std::string>>::iterator it;
                        if(toCheck->values.size() > idx) {
                            it = std::next(toCheck->values.begin(), idx);
                        } else {
                            errorMessage(14, "Bad array sizes");
                        }

                        (*variables)[check] = {it->first, it->second};

                    } else {
                        /*std::list<std::pair<int, std::string>>::iterator it;
                        if(toCheck->values.size() > idx) {
                            it = std::next(toCheck->values.begin(), idx);
                        } else {
                            errorMessage(14, "Bad array sizes");
                        }

                         Compare values
                        if(check != it->second) {
                            return false;
                        }*/
                        bool ret = false;
                        for(std::pair<int, std::string> it: toCheck->values) {
                            if(it.second == check) {
                                ret = true;
                            }
                        }
                        if(!ret)
                            return false;
                    }

                    idx++;
                }

                break;
            }
        }
    }

    return true;
}

bool instance::checkPrecond(trans *transition, std::unordered_map<std::string, std::pair<int, std::string>> *variables) {

    for(std::pair<std::string, std::list<std::string>> trans: transition->precond) {
        /* Check every PRECOND in transition */

        for(auto & toCheck: this->places) {
            if(toCheck->name == trans.first) {
                /* If this is the searched place, check it */
                if((toCheck->values).empty()) {
                    /* Has no value */
                    return false;
                }

                int idx = 0;

                for(std::string check: trans.second) {
                    if(isVariable(&check)) {

                        /* Save to var */
                        std::list<std::pair<int, std::string>>::iterator it;
                        if(toCheck->values.size() > idx) {
                            it = std::next(toCheck->values.begin(), idx);
                        } else {
                            errorMessage(14, "Bad array sizes");
                        }

                        (*variables)[check] = {it->first, it->second};

                    } else {
                        /*std::list<std::pair<int, std::string>>::iterator it;
                        if(toCheck->values.size() > idx) {
                            it = std::next(toCheck->values.begin(), idx);
                        } else {
                            errorMessage(14, "Bad array sizes");
                        }

                         Compare values
                        if(check != it->second) {
                            return false;
                        }*/
                        bool ret = false;
                        for(std::pair<int, std::string> it: toCheck->values) {
                            if(it.second == check) {
                                ret = true;
                            }
                        }
                        if(!ret)
                            return false;
                    }

                    idx++;
                }

                break;
            }
        }
    }

    return true;
}

void instance::doInstruction(std::string *instruction,
                             std::unordered_map<std::string, std::pair<int, std::string>> *variables,
                             std::list<std::string> *tempStack) {
    std::string inst = *instruction;

    int pos = 0;
    if((pos = inst.find("PUSHTEMP")) != std::string::npos) {
        inst.erase(pos, 9);

        std::string val;

        if(isVariable(&inst)) {

            std::pair<int, std::string> var = getVariable(&inst, variables);

            if(var.first != 0) {
                errorMessage(12, "Can't push object instance");
            }

            val = var.second;

        } else {
            val = inst;
        }

        tempStack->push_front(val);
    } else if((pos = inst.find("POPTEMP")) != std::string::npos) {
        inst.erase(pos, 8);

        std::string val;
        bool obj;

        if(!tempStack->empty() && (tempStack->front() == "NEWOBJ")) {    /* Internal check */
            /* This is new object */
            tempStack->pop_front();
            val = tempStack->front();
            tempStack->pop_front();
            instance* ref = (instance*)(std::stoul(val, nullptr, 16));

            // record creating instance of object
            virtualMachine->archiver.record(archiveInstruction("create instance", name, reference->name, ref->name, ref->reference->name));
            virtualMachine->archiver.record(archiveInitial(ref->name, ref->reference->name, ref->places, virtualMachine->archiver.getTrans().id));
            obj = true;
        } else {
            if(!tempStack->empty()) {
                val = tempStack->front();
                tempStack->pop_front();
            } else {
                val = "";   // implicit value
            }
            obj = false;
        }

        if(obj) {
            (*variables)[inst] = {1, val};
        } else {
            (*variables)[inst] = {0, val};
        }
    } else if((pos = inst.find("NEW")) != std::string::npos) {
        inst.erase(pos, 4);

        std::string className = inst;

        instance *newInstance = (instance*)malloc(sizeof(instance));

        for(object *newObj: virtualMachine->objects) {
            if(newObj->name == className) {

                static size_t id = 0;
                std::string name;
                while (virtualMachine->getInstance(name = "instance-" + std::to_string(id)) != nullptr) {
                    id++;
                }

                new (newInstance) instance(&name, virtualMachine);

                newInstance->create(newObj);

                virtualMachine->objectInstance.push_back(newInstance);
                break;
            }
        }

        std::ostringstream sAddress;
        sAddress << newInstance;
        std::string address =  sAddress.str();
        std::string keyword = "NEWOBJ";     /* Internal check for saving */

        tempStack->push_front(address);
        tempStack->push_front(keyword);
        /* FU */

    } else if(inst.find("CALLTEMP") != std::string::npos) {

        std::string temp2 = tempStack->front();
        tempStack->pop_front();

        std::string temp1 = tempStack->front();
        tempStack->pop_front();

        double tmp1 = std::stod(temp1);
        double tmp2 = std::stod(temp2);

        std::string output;

        /* Check operation */
        if(inst.find('1') != std::string::npos) {

            if(tmp1 < tmp2) {
                output = "true";
            } else {
                output = "false";
            }

        } else if(inst.find('2') != std::string::npos) {

            if(tmp1 > tmp2) {
                output = "true";
            } else {
                output = "false";
            }

        } else if(inst.find('3') != std::string::npos) {

            if(tmp1 <=tmp2) {
                output = "true";
            } else {
                output = "false";
            }

        } else if(inst.find('4') != std::string::npos) {

            if(tmp1 >= tmp2) {
                output = "true";
            } else {
                output = "false";
            }

        } else if(inst.find("ADD") != std::string::npos) {

            double result = tmp1 + tmp2;

            std::ostringstream strs;
            strs << result;
            output = strs.str();

        } else if(inst.find("DEC") != std::string::npos) {

            double result = tmp1 - tmp2;

            std::ostringstream strs;
            strs << result;
            output = strs.str();

        } else if(inst.find("MUL") != std::string::npos) {

            double result = tmp1 * tmp2;

            std::ostringstream strs;
            strs << result;
            output = strs.str();

        } else if(inst.find("DIV") != std::string::npos) {

            double result = tmp1 / tmp2;

            std::ostringstream strs;
            strs << result;
            output = strs.str();

        } else {
            errorMessage(11, "Internal error");
        }

        tempStack->push_front(output);
    } else if((pos = inst.find("CALL")) != std::string::npos) {

        inst.erase(pos, 5);
        std::string call = inst;

        pos = call.find('.');

        std::string varName = call.substr(0, pos);
        std::string functionName = call.substr(pos + 1, std::string::npos);

        /* Find variable in variables */
        std::pair<int, std::string> element = (*variables)[varName];

        if(element.first != 1) {
            errorMessage(20, "Can't access non-object as object");
        }

        /* Get instance */
        long address = std::strtol(element.second.c_str(), nullptr, 16);
        
        /* Call function */
        auto *obj = (instance*)address;

        // record message between instances
        struct archiveInstruction instruction = archiveInstruction(functionName, name, reference->name, obj->name, obj->reference->name);
        virtualMachine->archiver.record(instruction);

        size_t head = tempStack->size();
        obj->callFunction(&functionName, tempStack);

        method *actualMethod = nullptr;

        for(method * check: reference->methods) {
            if(check->name == functionName) {
                actualMethod = check;
                break;
            }
        }

        // Difference on stack will determine number of returned values
        size_t offset = tempStack->size() + actualMethod->params.size() > head ? tempStack->size() + actualMethod->params.size() - head : 0;
        std::vector<std::string> response = std::vector<std::string>();

        
        for (size_t i = 1; i <= offset; i++) {
            response.push_back(*std::next(tempStack->begin(), tempStack->size()-i));
        }
        if (offset)
            // if method returned value on stack record response
            virtualMachine->archiver.record(archiveInstruction(instruction.getId(), response));
    }
}

std::pair<int, std::string> instance::getVariable(std::string *var, std::unordered_map<std::string, std::pair<int, std::string>> *variables) {

    for(std::pair<std::string, std::pair<int, std::string>> check: *variables) {
        if(check.first == *var) {
            return check.second;
        }
    }

    errorMessage(11, "Internal error");
    exit(1);
}

bool instance::checkGuard(trans *transition, std::unordered_map<std::string, std::pair<int, std::string>> *variables, std::list<std::string> *tempStack) {

    int pos = 0;
    bool in = false;

    for(std::string action: transition->guard) {

        /* for check */
        in = true;

        if((pos = action.find("CALL ")) != std::string::npos) {
            //virtualMachine->archiver.startTrans(stackTransition(transition->name, name, reference->name,places));
            doInstruction(&action, variables, tempStack);
            //virtualMachine->archiver.stopTrans(stackTransition(transition->name, name, reference->name,places));

            if(tempStack->front() != "true") {
                return false;
            }
            tempStack->pop_front();

            std::string call = action;
            call.erase(pos, 5);

            pos = call.find('.');

            std::string varName = call.substr(0, pos);
            std::string functionName = call.substr(pos + 1, std::string::npos);

            /* Find variable in variables */
            std::pair<int, std::string> element = (*variables)[varName];

            if(element.first != 1) {
                errorMessage(20, "Can't access non-object as object");
            }

            /* Get instance */
            long address = std::strtol(element.second.c_str(), nullptr, 16);

            /* Check function */
            auto *obj = (instance*)address;

            method *actualMethod = nullptr;
            for(method * check: obj->reference->methods) {
                if(check->name == functionName) {
                    actualMethod = check;
                    break;
                }
            }

            auto iter = actualMethod->params.end();
            while (iter != actualMethod->params.begin()) {
                iter--;
                std::string varN = *iter;
                std::string front = tempStack->front();
                (*variables)[varN].first = 0;
                (*variables)[varN].second = tempStack->front();
                tempStack->pop_front();
            }

        } else {
            if(action.find("PUSHTEMP") != std::string::npos) {
                std::string val = action.substr(9, std::string::npos);
                if(isVariable(&val)) {
                    if((*variables).find(val) == (*variables).end()) {
                        /* non existing var, only for guard */
                        (*variables)[val] = {0, ""};    /* Implicit value */
                    }
                }
            }

            doInstruction(&action, variables, tempStack);
        }
    }

    if(in) {
        if(tempStack->front() == "true") {
            tempStack->pop_front();
            return true;
        } else {
            tempStack->pop_front();
            return false;
        }
    }

    return true;
}

void instance::doAction(trans *transition, std::unordered_map<std::string, std::pair<int, std::string>> *variables, std::list<std::string> *tempStack) {

    uint counter = 0;

    //virtualMachine->archiver.startTrans(stackTransition(transition->name, name, reference->name,places));
    for(std::string action: transition->action) {
        doInstruction(&action, variables, tempStack);
        counter++;
    }
    //virtualMachine->archiver.stopTrans(stackTransition(transition->name, name, reference->name,places));
    if (counter) {
        struct archiveTransition trans = archiveTransition(transition->name, name, reference->name);
        //virtualMachine->archiver.record(trans);
    }

}

void instance::doneStep(trans *transition, std::unordered_map<std::string, std::pair<int, std::string>> *variables, std::list<std::string> *tempStack) {

    /* FU */
    bool end = false;

    /* Clear preconds */
    for(std::pair<std::string, std::list<std::string>> trans: transition->precond) {
        for(auto & toCheck: this->places) {
            if(toCheck->name == trans.first) {
                for(std::pair<int, std::string> pairVal: toCheck->values) {
                    for(std::string toDelete: trans.second) {
                        if(isVariable(&toDelete)) {
                            toCheck->values.pop_front();
                            if (pairVal.first == 1) {
                                /* Get instance */
                                long address = std::strtol(pairVal.second.c_str(), nullptr, 16);

                                /* Call function */
                                auto *inst = (instance *) address;

                                inst->referenceCounter--;
                            }
                            end = true;
                        }
                        if(toDelete == pairVal.second) {
                            toCheck->values.remove(pairVal);
                            if (pairVal.first == 1) {
                                /* Get instance */
                                long address = std::strtol(pairVal.second.c_str(), nullptr, 16);

                                /* Call function */
                                auto *inst = (instance *) address;

                                inst->referenceCounter--;
                            }
                            end = true;
                        }

                        if(end)
                            break;
                    }

                    if(end)
                        break;
                }
            }

            if(end)
                break;
        }
    }

    /* Set postconds */
    for(std::pair<std::string, std::list<std::string>> trans: transition->postcond) {
        for(auto & toCheck: this->places) {
            if(toCheck->name == trans.first) {
                for(std::string val: trans.second) {
                    if(isVariable(&val)) {
                        std::pair<int, std::string> pairVal = (*variables)[val];
                        toCheck->values.emplace_back(pairVal.first, pairVal.second);
                        if(pairVal.first == 1) {
                            /* Get instance */
                            long address = std::strtol(pairVal.second.c_str(), nullptr, 16);

                            /* Call function */
                            auto *inst = (instance*)address;

                            inst->referenceCounter++;
                        }
                    } else {
                        toCheck->values.emplace_back(0, val);
                    }
                }

                /* Output values for return */
                if(toCheck->name == "return") {
                    for(std::pair<int, std::string> val: toCheck->values) {
                        tempStack->push_front(val.second);
                    }
                }
            }
        }
    }
}

void instance::callFunction(std::string *str, std::list<std::string> *tempStack) {

    method *actualMethod = nullptr;

    for(method * check: reference->methods) {
        if(check->name == *str) {
            actualMethod = check;
            break;
        }
    }

    if(actualMethod == nullptr) {
        errorMessage(20, "Calling of non-existing method");
        exit(1);
    }

    /* Load input parameters */
    std::unordered_map<std::string, std::pair<int, std::string>> enterVariables;

    auto iter = actualMethod->params.rend();
    while (iter != actualMethod->params.rbegin()) {
        iter--;
        enterVariables[*iter] = {0, tempStack->front()};
        tempStack->pop_front();
    }

    for(trans *transition: actualMethod->transitions) {

        std::unordered_map<std::string, std::pair<int, std::string>> variables;

        /* Add input parameters to variables */
        for(std::pair<std::string, std::pair<int, std::string>> var: enterVariables) {
            variables[std::get<0>(var)] = std::get<1>(var);
        }


        if(!checkCond(transition, &variables)) {
            continue;
        }

        if(!checkPrecond(transition, &variables)) {
            continue;
        }

        if(!checkGuard(transition, &variables, tempStack)) {
            continue;
        }

        doAction(transition, &variables, tempStack);

        //virtualMachine->archiver.startTrans(stackTransition(transition->name, name, reference->name,places));

        doneStep(transition, &variables, tempStack);

        //virtualMachine->archiver.stopTrans(stackTransition(transition->name, name, reference->name,places));

        if(actualMethod->isSync) {
            /* Push temp every input variable */
            for(std::string par: actualMethod->params) {
                tempStack->push_front(variables[par].second);
            }

            tempStack->push_front("true");
        }

    }

}

void instance::deleteInstance() {
    /*for(place *obj: places) {
        delete obj;
    }*/
    places.clear();

    delete this;
}
