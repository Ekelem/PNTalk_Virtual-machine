/**
 * @file    outgen.h
 * @author  Erik Kelemen <xkelem01@stud.fit.vutbr.cz>
 *
 * @section DESCRIPTION
 *
 * The outputgen class represent archiver for simulation data
 */

#ifndef BP_OUTPUTGEN_H
#define BP_OUTPUTGEN_H

#include "cereal/archives/json.hpp"
#include "cereal/types/vector.hpp"
#include "cereal/types/map.hpp"
#include "cereal/types/list.hpp"
#include "cereal/types/string.hpp"

#include "instance.h"

#include <list>
#include <map>


struct archiveInstruction
{
    uint id;
	std::string name;
	std::string caller;
	std::string callerClass;
	std::string reciever;
	std::string recieverClass;
    std::string trans;
    std::string transClass;
    std::vector<std::string> response;

    uint respondTo;

	//std::map<std::string, std::string> args;
    archiveInstruction(uint id, std::vector<std::string> respoonses ) {
        respondTo = id;
        response = respoonses;
    }
	//archiveInstruction(std::string instruction);
	//archiveInstruction(std::string instruction, std::string caller, std::string callerClass) : name(instruction), caller(caller), callerClass(callerClass), reciever(""), recieverClass("") {}
	archiveInstruction(std::string instruction, std::string caller, std::string callerClass, std::string reciever, std::string recieverClass) : name(instruction), caller(caller), callerClass(callerClass), reciever(reciever), recieverClass(recieverClass) {
	    static uint id_provider = 0;
	    id = id_provider;
	    id_provider++;
        respondTo = 0;
	    trans = "";
        transClass = "";
        response = std::vector<std::string>();
    }

	uint getId() {
	    return id;
	}

	template<class Archive> void serialize(Archive & ar) 
   	{
        if (!respondTo)
            ar( cereal::make_nvp("id", id),
                cereal::make_nvp("message_name", name),
                cereal::make_nvp("caller_instance", caller),
                cereal::make_nvp("caller_class", callerClass),
                cereal::make_nvp("receiver_instance", reciever),
                cereal::make_nvp("receiver_class", recieverClass),
                cereal::make_nvp("transition", trans));
        else
            ar( cereal::make_nvp("respond_to", respondTo),
                cereal::make_nvp("response", response));
	}
};

struct archiveResponse
{
    uint refId;
    std::string value;

    archiveResponse(uint messageId, std::string response) : refId(messageId), value(response) {

    }

    template<class Archive> void serialize(Archive & ar)
    {
        ar( cereal::make_nvp("message_id", refId),
            cereal::make_nvp("response", value));
    }
};

struct transitionCache
{
    std::string name;
    std::string cls;
    std::map<std::string, std::list<std::string>> state;

    /*transitionCache(instance & inst) : name(inst->name), cls(inst->reference->name), state(inst->)
    {

    }*/

};

struct stackTransition
{
    std::string	caller;
    std::string cls;
    std::list<place *> places;

    stackTransition(std::string name, std::string reference, std::list<place *> listPlaces) : caller(name), cls(reference), places(listPlaces)
    {

    }
};

struct archiveTransition
{
    uint id;
	std::string name;
	std::string	instance;
	std::string reference;
	//std::list<place *> places;

	archiveTransition(std::string transition, std::string instance, std::string instanceClass)
	: name(transition), instance(instance), reference(instanceClass) {
        static uint id_provider = 0;
        id_provider++;
        id = id_provider;
	}

	template<class Archive> void serialize(Archive & ar) 
   	{ 
    	ar( cereal::make_nvp("id", id), cereal::make_nvp("transition_name", name),
    		cereal::make_nvp("instance", instance),
    		cereal::make_nvp("instance_class", reference));
   	}
};

struct archiveStep
{
	std::vector<archiveInstruction> instructions;
	std::vector<archiveTransition> transitions;
	archiveStep() {};

	template<class Archive> void serialize(Archive & ar) 
   	{
    	ar( cereal::make_nvp("messages", instructions),
    		cereal::make_nvp("transitions", transitions)); 
	} 
};

struct archiveInitial
{
    std::string cls;
    std::string inst;
    std::list<place *> places;

    archiveInitial(std::string instanceName, std::string instanceClass, std::list<place *> instancePlaces) : inst(instanceName), cls(instanceClass), places(instancePlaces) {

    }

    template<class Archive> void serialize(Archive & ar)
    {
        ar( cereal::make_nvp("instance", inst),
            cereal::make_nvp("class", cls),
            cereal::make_nvp("places", places));
    }
};

class outputgen {

public:
    /**
     * Default constructor
     */
    outputgen();

    void generate();

    void startStep();
    void startTrans(stackTransition trans);
    void stopTrans(stackTransition trans);
    std::pair<std::string, std::string> getTrans();
    void record(struct archiveInstruction message);
    void record(struct archiveTransition trans);
    void record(struct archiveInitial init);

private:
	uint stepCount;
	uint messageCount;
	std::stack<std::pair<std::string, std::string>> transStack;
	uint transCount;
	std::vector<archiveStep> steps;
    std::vector<archiveInitial> initial;
};

#endif