/**
 * @file    outputgen.h
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

#include "place.h"

#include <list>
#include <map>

namespace cereal
{
    // Serializing for place pairs
    template <class Archive> inline
    void CEREAL_SERIALIZE_FUNCTION_NAME( Archive & ar, std::pair<int, std::string> & pair )
    {
        ar( CEREAL_NVP_("type",  pair.first),
            CEREAL_NVP_("value", pair.second) );
    }
} // namespace cereal

namespace cereal
{
    // Serializing for place pointers
    template <class Archive> inline
    void CEREAL_SERIALIZE_FUNCTION_NAME( Archive & ar, std::shared_ptr<place> & p )
    {
        ar( cereal::make_nvp(p->name, p->values));
    }
}

namespace cereal
{
    // Serializing for places
    template <class Archive> inline
    void CEREAL_SERIALIZE_FUNCTION_NAME( Archive & ar, place & place )
    {
        ar( cereal::make_nvp(place.name, place.values));
    }
}

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

    archiveInstruction(uint id, std::vector<std::string> respoonses ) {
        respondTo = id;
        response = respoonses;
    }
	
    /**
     * Constructor, instruction
     *
     * @param instruction instruction keyword
     * @param caller caller object instance identifier
     * @param callerClass caller object class identifier
     * @param receiver receiver object instance identifier
     * @param receiverClass receiver object class identifier
     */
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

    // serialize
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

    /**
     * Constructor, reesponse message
     *
     * @param messageId message identifier
     * @param response response
     */
    archiveResponse(uint messageId, std::string response) : refId(messageId), value(response) {

    }

    // serialize
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
};

struct stackTransition
{
    uint32_t id;
    std::string	name;
    std::string caller;
    std::string cls;
    std::list<place> places;

    friend bool operator==(const stackTransition& l, const stackTransition& r)
    {
        return l.caller == r.caller && l.cls == r.cls;
    }

    /**
     * Constructor, copy current place values
     *
     * @param trans transition name
     * @param inst instance identificator
     * @param reference class identificator
     * @param listPlaces memory footprint of places, later used for computation of changelog
     */
    stackTransition(std::string trans, std::string inst, std::string reference, std::list<std::shared_ptr<place>> listPlaces) : id(0), name(trans), caller(inst), cls(reference) {
        for (std::shared_ptr<place> p: listPlaces) {
            places.push_back(*p);
        }
    }
    stackTransition(std::string trans, std::string inst, std::string reference) : id(0), name(trans), caller(inst), cls(reference), places(std::list<place>()) {}
};

struct archiveTransition
{
    uint id;
	std::string name;
	std::string	instance;
	std::string reference;

    /**
     * Constructor, id assignment
     *
     * @param transition transition name
     * @param instance instance identificator
     * @param instanceClass class identificator
     */
	archiveTransition(std::string transition, std::string instance, std::string instanceClass)
	: name(transition), instance(instance), reference(instanceClass) {
        static uint id_provider = 0;
        id_provider++;
        id = id_provider;
	}

    //serialization
	template<class Archive> void serialize(Archive & ar) 
   	{ 
    	ar( cereal::make_nvp("id", id), cereal::make_nvp("transition_name", name),
    		cereal::make_nvp("instance", instance),
    		cereal::make_nvp("instance_class", reference));
   	}
};

struct archiveTransStart
{
    std::string name;
    std::string inst;
    std::string reference;
    uint32_t id;

    /**
     * Constructor, allocating id
     *
     * @param trans transition name
     * @param inst instance identificator
     * @param cls class identificator
     */
    archiveTransStart(std::string trans, std::string inst, std::string cls) : name(trans), inst(inst), reference(cls) {
        static uint32_t cnt = 0;
        id = cnt++;
    }

    // serialization
    template<class Archive> void serialize(Archive & ar)
    {
        ar( cereal::make_nvp("transition_name", name),
            cereal::make_nvp("instance", inst),
            cereal::make_nvp("instance_class", reference),
            cereal::make_nvp("id", id)
        );
    }
};


struct archiveTransEnd
{
    uint32_t original;
    std::list<place> changelog;

    /**
     * Constructor, referencing start and changelog captures
     *
     * @param id transition identificator
     * @param change captured change
     */
    archiveTransEnd(uint32_t id, std::list<place> change) : original(id), changelog(change) {}

    // serialization
    template<class Archive> void serialize(Archive & ar)
    {
        ar( cereal::make_nvp("id", original),
            cereal::make_nvp("changelog", changelog)
        );
    }
};

struct archiveStep
{
	std::vector<archiveInstruction> instructions;
	std::vector<archiveTransStart> transStarts;
    std::vector<archiveTransEnd> transEnds;

    /**
     * Default constructor
     */
	archiveStep() {};

	template<class Archive> void serialize(Archive & ar) 
   	{
    	ar( cereal::make_nvp("messages", instructions),
    		cereal::make_nvp("transition_starts", transStarts),
            cereal::make_nvp("transition_ends", transEnds));
	} 
};


struct archiveInitial
{
    std::string cls;
    std::string inst;
    uint32_t creation;
    std::list<place> places;

    /**
     * Constructor, init state of instances
     *
     * @param instanceName instance identificator
     * @param instanceClass class identificator
     * @param instancePlaces current places
     * @param transId create transition identifier
     */
    archiveInitial(std::string instanceName, std::string instanceClass, std::list<std::shared_ptr<place>> instancePlaces, uint32_t transId = 0) : inst(instanceName), cls(instanceClass), creation(transId), places(std::list<place>()) {
        for (auto & place: instancePlaces) {
            places.push_back(*place);
        }
    }

    // serialize
    template<class Archive> void serialize(Archive & ar)
    {
        ar( cereal::make_nvp("instance", inst),
            cereal::make_nvp("cls", cls),
            cereal::make_nvp("creation", creation),
            cereal::make_nvp("places", places)
            );
    }
};

class outputgen {

public:
    outputgen();

    /**
     * Generate output on stream from captured data
     *
     * @param output output stream
     */
    void generate(std::ostream & output);

    /**
     * Record starting new step and finishing previous
     */
    void startStep();

    /**
     * Internaly record starting of transition
     *
     * @param trans transition data
     */
    void startTrans(stackTransition trans);

    /**
     * Internaly record finish of transition
     *
     * @param trans transition data
     */
    void stopTrans(stackTransition trans);

    /**
     * Get active transition
     * @return transition data
     */
    stackTransition getTrans();

    /**
     * Record message from simulation
     *
     * @param message message data
     */
    void record(struct archiveInstruction message);

    /**
     * Record transition start from simulation
     *
     * @param trans transition start data
     */
    void record(struct archiveTransStart trans);

    /**
     * Record transition end from simulation
     *
     * @param trans transition end data
     */
    void record(struct archiveTransEnd trans);

    /**
     * Record initialization of new instance from simulation
     *
     * @param init initialization data
     */
    void record(struct archiveInitial init);

private:
	uint stepCount;
	uint messageCount;
	std::stack<stackTransition> transStack;
	uint transCount;
	std::vector<archiveStep> steps;
    std::vector<archiveInitial> initial;
};

#endif