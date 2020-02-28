/**
 * @file    vm.cpp
 * @author  Tomas Lapsansky <xlapsa00@stud.fit.vutbr.cz>
 */

#include "../lib/vm.h"

vm::vm() {
    finished = false;
}

void vm::parseInput(std::string *file) {

    srcFile.open(*file);

    if(!srcFile.is_open()) {
        errorMessage(10, "Can't open file");
    }

    getMain();
    getClasses();
    setMain();
}

void vm::registerScenario(std::string *file) {

    try {
        scenario = YAML::LoadFile(*file);
    } catch (const YAML::BadFile e){
        errorMessage(10, "Can't open file: " + *file);
    }

}

bool vm::hasKeyword() {
    if(parseLine.find("COND") != std::string::npos) {
        return true;
    } else if(parseLine.find("PRECOND") != std::string::npos) {
        return true;
    } else if(parseLine.find("ACTION") != std::string::npos) {
        return true;
    } else if(parseLine.find("GUARD") != std::string::npos) {
        return true;
    } else if(parseLine.find("POSTCOND") != std::string::npos) {
        return true;
    } else if(parseLine.find("FUNC") != std::string::npos) {
        return true;
    } else if(parseLine.find("SYNC") != std::string::npos) {
        return true;
    } else if(parseLine.find("CLASS") != std::string::npos) {
        return true;
    } else if(parseLine.find("TRANS") != std::string::npos) {
        return true;
    } else if(parseLine.find("PLACE") != std::string::npos) {
        return true;
    } else if(parseLine.empty()) {
        return true;
    }

    return false;
}

object* vm::createClass(std::string *className) {

    object *newObject = (object*)malloc(sizeof(object));
    new (newObject) object(className);

    this->objects.push_back(newObject);

    return newObject;
}

void vm::getMain() {

    getline(srcFile, parseLine);
    mainName = parseLine.substr(9);    /* POS of name */
}

struct forced_object {
    std::string instance_name;
    std::string class_name;
    std::map<std::string, std::string> places;
};

void vm::setMain() {
    for(object *main: objects) {
        if(main->name == this->mainName) {

            std::string str = "main";

            instance *newInstance = (instance*)malloc(sizeof(instance));
            new (newInstance) instance(&str, this);

            newInstance->create(main);
            newInstance->referenceCounter = INT32_MAX;    /* Only for main */

            objectInstance.push_back(newInstance);
            newInstance->recordInitial();
        }
    }

    for (YAML::const_iterator it = scenario.begin(); it != scenario.end(); it++)
    {

        std::string instance_name = it->first.Scalar();
        std::string class_name = it->second["class"].Scalar();
        YAML::Node places = it->second["places"];

        for(object *new_instance: objects) {
            if (new_instance->name == class_name) {
                instance * newInstance = getInstance(instance_name);
                if (newInstance == nullptr) {
                    newInstance = (instance *) malloc(sizeof(instance));
                    new(newInstance) instance(&instance_name, this);
                    newInstance->create(new_instance);
                    newInstance->referenceCounter = INT32_MAX;
                    objectInstance.push_back(newInstance);
                }

                if (places.IsDefined() && places.IsMap())
                    for (YAML::const_iterator plit = places.begin(); plit != places.end(); plit++) {
                        std::string place_name = plit->first.Scalar();

                        for (auto & place: newInstance->places) {
                            if (place->name == place_name) {
                                place->values = std::list<std::pair<int, std::string>>();
                                place->values.emplace_back(std::pair<int, std::string>(0, plit->second.Scalar()));
                                break;
                            }
                        }
                    }
                newInstance->recordInitial();
            }
        }
    }
    return;
}

void vm::getClasses() {
    getline(srcFile, parseLine);

    while(!parseLine.empty() ) {
        /* Create class with name */
        std::string delimeter = "CLASS";

        std::string className = parseLine.substr(parseLine.find(delimeter) + delimeter.size() + 1, std::string::npos);
        className = className.erase(className.size() - 1, 1);

        object *newObject = createClass(&className);

        getline(srcFile, parseLine);

        fillClass(newObject);
    }
}

instance * vm::getInstance(std::string name) {
    for (auto it = objectInstance.begin(); it != objectInstance.end(); it++) {
        if ((*it)->name == name)
            return *it;
    }
    return nullptr;
}

void vm::fillClass(object *newObject) {

    while((parseLine.find("CLASS") == std::string::npos) && !parseLine.empty()) {

        if(parseLine.find("OBJ") != std::string::npos) {
        /* Object */

            std::string functionName = "object";
            addMethod(newObject, &functionName, nullptr, false);

        } else {
        /* Function */
            std::string delimeter;
            bool sync = false;
            if(parseLine.find("FUNC") != std::string::npos) {
                delimeter = "FUNC ";
            } else {
                delimeter = "SYNC ";
                sync = true;
            }
            std::string functionName = parseLine.substr(parseLine.find(delimeter) + delimeter.size(), std::string::npos);
            std::list<std::string> params;

            if(functionName.find(':') != functionName.size() - 1) {
                /* With params */
                std::string paramStr = functionName.substr(functionName.find(':') + 2, std::string::npos);

                std::string par;
                for(char &c: paramStr) {
                    if(c == ' ') {
                        params.push_back(par);
                    } else {
                        par += c;
                    }
                }
                params.push_back(par);  /* Push last parameter */
            }

            functionName = functionName.substr(0, functionName.find(':'));

            addMethod(newObject, &functionName, &params, sync);

        }

    }
}

void vm::addMethod(object *newObject, std::string *functionName, std::list<std::string> *params, bool sync) {

    newObject->addMethod(functionName, params, sync);

    getline(srcFile, parseLine);

    while(parseLine.find("CLASS") == std::string::npos
        && parseLine.find("FUNC") == std::string::npos
        && parseLine.find("OBJ") == std::string::npos
        && parseLine.find("SYNC") == std::string::npos
        && !parseLine.empty()){

        if(!sync) {
            if (parseLine.find("TRANS") != std::string::npos) {

                /* Check details of trans */
                std::string delimeter = "TRANS";

                std::string transName = parseLine.substr(parseLine.find(delimeter) + delimeter.size() + 1,
                                                         std::string::npos);

                trans *newTrans = newObject->addTrans(&transName, functionName);

                getline(srcFile, parseLine);

                bool loop = true;
                while (loop) {
                    /*
                     * COND
                     * PRECOND
                     * ACTION
                     * GUARD
                     * POSTCOND
                     * */

                    if (parseLine.find("PRECOND") != std::string::npos) {

                        getline(srcFile, parseLine);

                        while (!hasKeyword()) {

                            std::string condStr = parseLine.substr(parseLine.find('\t') + 1, std::string::npos);

                            std::string place = condStr.substr(0, condStr.find(':'));
                            condStr.erase(0, condStr.find(':') + 2);

                            std::pair<std::string, std::list<std::string>> precond;

                            precond.first = place;

                            if (condStr.find(' ') != std::string::npos) {
                                /* List */
                                std::string paramVar;
                                for (char &c: condStr) {
                                    if (c == ' ') {
                                        precond.second.push_back(paramVar);
                                        paramVar = "";
                                    } else {
                                        paramVar += c;
                                    }
                                }
                                if (!paramVar.empty()) {
                                    precond.second.push_back(paramVar);
                                }
                            } else {
                                /* One param */
                                precond.second.push_back(condStr);
                            }

                            std::string str = "PRECOND";
                            newTrans->setCond(&str, precond);

                            getline(srcFile, parseLine);
                        }

                    } else if (parseLine.find("ACTION") != std::string::npos) {

                        getline(srcFile, parseLine);
                        /* Push code to stack */

                        while (!hasKeyword()) {
                            std::string toPush = parseLine.substr(parseLine.find('\t') + 1, std::string::npos);
                            newTrans->action.push_back(toPush);

                            getline(srcFile, parseLine);
                        }

                    } else if (parseLine.find("GUARD") != std::string::npos) {

                        getline(srcFile, parseLine);
                        /* Push code to stack */

                        while (!hasKeyword()) {
                            std::string toPush = parseLine.substr(parseLine.find('\t') + 1, std::string::npos);
                            newTrans->guard.push_back(toPush);

                            getline(srcFile, parseLine);
                        }

                    } else if (parseLine.find("POSTCOND") != std::string::npos) {

                        getline(srcFile, parseLine);

                        while (!hasKeyword()) {

                            std::string condStr = parseLine.substr(parseLine.find('\t') + 1, std::string::npos);

                            std::string place = condStr.substr(0, condStr.find(':'));
                            condStr.erase(0, condStr.find(':') + 2);

                            std::pair<std::string, std::list<std::string>> postcond;

                            postcond.first = place;

                            if (condStr.find(' ') != std::string::npos) {
                                /* List */
                                std::string paramVar;
                                for (char &c: condStr) {
                                    if (c == ' ') {
                                        postcond.second.push_back(paramVar);
                                        paramVar = "";
                                    } else {
                                        paramVar += c;
                                    }
                                }
                                if (!paramVar.empty()) {
                                    postcond.second.push_back(paramVar);
                                }
                            } else {
                                /* One param */
                                postcond.second.push_back(condStr);
                            }

                            std::string str = "POSTCOND";
                            newTrans->setCond(&str, postcond);

                            getline(srcFile, parseLine);
                        }

                    } else if (parseLine.find("COND") != std::string::npos) {

                        getline(srcFile, parseLine);

                        while (!hasKeyword()) {

                            std::string condStr = parseLine.substr(parseLine.find('\t') + 1, std::string::npos);

                            std::string place = condStr.substr(0, condStr.find(':'));
                            condStr.erase(0, condStr.find(':') + 2);

                            std::pair<std::string, std::list<std::string>> cond;

                            cond.first = place;

                            if (condStr.find(' ') != std::string::npos) {
                                /* List */
                                std::string paramVar;
                                for (char &c: condStr) {
                                    if (c == ' ') {
                                        cond.second.push_back(paramVar);
                                        paramVar = "";
                                    } else {
                                        paramVar += c;
                                    }
                                }
                                if (!paramVar.empty()) {
                                    cond.second.push_back(paramVar);
                                }
                            } else {
                                /* One param */
                                cond.second.push_back(condStr);
                            }

                            std::string str = "COND";
                            newTrans->setCond(&str, cond);

                            getline(srcFile, parseLine);
                        }

                    } else {
                        loop = false;
                    }
                }


            } else if (parseLine.find("PLACE") != std::string::npos) {

                std::string placeDetail = parseLine.substr(parseLine.find("PLACE") + 6, std::string::npos);

                std::string placeName = placeDetail.substr(0, placeDetail.find(':'));

                std::string placeValues = placeDetail.substr(placeDetail.find(':') + 2, std::string::npos);

                newObject->addPlace(&placeName, &placeValues, functionName);

                getline(srcFile, parseLine);

            }
        } else {
        /* For sync */

            /* Create sync "trans" */
            std::string transName = "sync";
            trans *newTrans = newObject->addTrans(&transName, functionName);

            bool loop = true;
            while (loop) {
                /*
                 * COND
                 * PRECOND
                 * ACTION
                 * GUARD
                 * POSTCOND
                 * */

                if (parseLine.find("PRECOND") != std::string::npos) {

                    getline(srcFile, parseLine);

                    while (!hasKeyword()) {

                        std::string condStr = parseLine.substr(parseLine.find('\t') + 1, std::string::npos);

                        std::string place = condStr.substr(0, condStr.find(':'));
                        condStr.erase(0, condStr.find(':') + 2);

                        std::pair<std::string, std::list<std::string>> precond;

                        precond.first = place;

                        if (condStr.find(' ') != std::string::npos) {
                            /* List */
                            std::string paramVar;
                            for (char &c: condStr) {
                                if (c == ' ') {
                                    precond.second.push_back(paramVar);
                                    paramVar = "";
                                } else {
                                    paramVar += c;
                                }
                            }
                            if (!paramVar.empty()) {
                                precond.second.push_back(paramVar);
                            }
                        } else {
                            /* One param */
                            precond.second.push_back(condStr);
                        }

                        std::string str = "PRECOND";
                        newTrans->setCond(&str, precond);

                        getline(srcFile, parseLine);
                    }

                } else if (parseLine.find("GUARD") != std::string::npos) {

                    getline(srcFile, parseLine);
                    /* Push code to stack */

                    while (!hasKeyword()) {
                        std::string toPush = parseLine.substr(parseLine.find('\t') + 1, std::string::npos);
                        newTrans->guard.push_back(toPush);

                        getline(srcFile, parseLine);
                    }

                } else if (parseLine.find("POSTCOND") != std::string::npos) {

                    getline(srcFile, parseLine);

                    while (!hasKeyword()) {

                        std::string condStr = parseLine.substr(parseLine.find('\t') + 1, std::string::npos);

                        std::string place = condStr.substr(0, condStr.find(':'));
                        condStr.erase(0, condStr.find(':') + 2);

                        std::pair<std::string, std::list<std::string>> postcond;

                        postcond.first = place;

                        if (condStr.find(' ') != std::string::npos) {
                            /* List */
                            std::string paramVar;
                            for (char &c: condStr) {
                                if (c == ' ') {
                                    postcond.second.push_back(paramVar);
                                    paramVar = "";
                                } else {
                                    paramVar += c;
                                }
                            }
                            if (!paramVar.empty()) {
                                postcond.second.push_back(paramVar);
                            }
                        } else {
                            /* One param */
                            postcond.second.push_back(condStr);
                        }

                        std::string str = "POSTCOND";
                        newTrans->setCond(&str, postcond);

                        getline(srcFile, parseLine);
                    }

                } else if (parseLine.find("COND") != std::string::npos) {

                    getline(srcFile, parseLine);

                    while (!hasKeyword()) {

                        std::string condStr = parseLine.substr(parseLine.find('\t') + 1, std::string::npos);

                        std::string place = condStr.substr(0, condStr.find(':'));
                        condStr.erase(0, condStr.find(':') + 2);

                        std::pair<std::string, std::list<std::string>> cond;

                        cond.first = place;

                        if (condStr.find(' ') != std::string::npos) {
                            /* List */
                            std::string paramVar;
                            for (char &c: condStr) {
                                if (c == ' ') {
                                    cond.second.push_back(paramVar);
                                    paramVar = "";
                                } else {
                                    paramVar += c;
                                }
                            }
                            if (!paramVar.empty()) {
                                cond.second.push_back(paramVar);
                            }
                        } else {
                            /* One param */
                            cond.second.push_back(condStr);
                        }

                        std::string str = "COND";
                        newTrans->setCond(&str, cond);

                        getline(srcFile, parseLine);
                    }

                } else {
                    loop = false;
                }
            }
        }
    }
}

void vm::step() {
    if(!finished) {
        finished = true;
        for(instance *obj: objectInstance) {
            if(obj->referenceCounter == 0) {
                /* Garbage collector */
                objectInstance.remove(obj);
                obj->deleteInstance();
            } else {
                obj->step(&tempStack);
            }
        }
    }
}

void vm::run() {

    if(!finished) {
        do {
            step();
        } while(!finished);
    }
}

void vm::detail() {

    for(instance *inst: objectInstance) {
        std::cout << inst->reference->name << '\t';

        if(inst->referenceCounter == INT32_MAX) {
            std::cout << "- main" << std::endl;
        } else {
            std::cout << " - " << inst << std::endl;
        }

        for(auto & element: inst->places) {
            std::cout << "\t place " << element->name << "\t = ";
            for(std::pair<int, std::string> output: element->values) {
                std::cout << output.second << " ";
            }
            std::cout << std::endl;
        }
    }

}

void vm::help() {
    std::cout << std::endl;
    std::cout << "Commands for controling of OOPN virtual machine:" << std::endl;
    std::cout << "s | step\tOne step of the virtual machine" << std::endl;
    std::cout << "r | run\t\tRun virtual machine to the end of the virtual machine" << std::endl;
    std::cout << "d | detail\tShow detail of created objects in simulation" << std::endl;
    std::cout << "c\t\t\tQuiting of the virtual machine" << std::endl;
    std::cout << "[others]\tShows help dialog" << std::endl << std::endl;
}

void vm::quit() {
    for(instance *obj: objectInstance) {
        obj->deleteInstance();
    }
    objectInstance.clear();

    for(object *obj: objects) {
        obj->deleteObject();
    }
    objects.clear();
}