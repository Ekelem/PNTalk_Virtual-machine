/**
 * @file    outputgen.cpp
 * @author  Erik Kelemen <xkelem01@stud.fit.vutbr.cz>
 */

#include "../lib/outputgen.h"

outputgen::outputgen() : stepCount(0)
{
	
}

void outputgen::startStep()
{
	static uint counter = 0;
	stepCount = counter;
	steps.push_back(archiveStep());
	counter++;
}

void outputgen::startTrans(std::pair<std::string, std::string> trans)
{
    transStack.push(trans);
}

void outputgen::stopTrans(std::pair<std::string, std::string> trans)
{
    if (!transStack.empty() && transStack.top() == trans) {
        transStack.pop();
    }
}

std::pair<std::string, std::string> outputgen::getTrans()
{
    return transStack.empty() ? std::pair<std::string, std::string>("","") : transStack.top();
}

void outputgen::record(struct archiveInstruction message)
{
    message.trans = getTrans().first;
    message.transClass = getTrans().second;
	steps[stepCount].instructions.push_back(message);
}

void outputgen::record(struct archiveTransition trans)
{
	steps[stepCount].transitions.push_back(trans);
}

void outputgen::generate()
{
	cereal::JSONOutputArchive archive(std::cout);
	archive(cereal::make_nvp("steps", steps));
}
