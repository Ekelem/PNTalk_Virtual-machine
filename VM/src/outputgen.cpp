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

void outputgen::startTrans(stackTransition trans)
{
    static uint32_t cnt = 0;
    trans.id = cnt++;
    transStack.push(trans);

    record(archiveTransStart(trans.name, trans.caller, trans.cls));
}

void outputgen::stopTrans(stackTransition trans)
{
    if (!transStack.empty() && transStack.top() == trans) {
        trans.id = transStack.top().id;

        std::list<place> changelog = std::list<place>();
        for (auto & origin : transStack.top().places) {
            for (auto & compare : trans.places) {
                if (origin.name == compare.name && origin.values != compare.values) {
                    place change = place(&compare.name, &compare.name);
                    change.values = compare.values;
                    changelog.push_back(change);
                }
            }
        }
        transStack.pop();
        record(archiveTransEnd(trans.id, changelog));
    }

}

stackTransition outputgen::getTrans()
{
    return transStack.empty() ? stackTransition("","","") : transStack.top();
}

void outputgen::record(struct archiveInstruction message)
{
    message.trans = getTrans().caller;
    message.transClass = getTrans().cls;
	steps[stepCount].instructions.push_back(message);
}

void outputgen::record(struct archiveTransStart trans)
{
	steps[stepCount].transStarts.push_back(trans);
}

void outputgen::record(struct archiveTransEnd trans)
{
    steps[stepCount].transEnds.push_back(trans);
}

void outputgen::record(struct archiveInitial init)
{
    initial.push_back(init);
}

void outputgen::generate()
{
	cereal::JSONOutputArchive archive(std::cout);
    archive(cereal::make_nvp("initial", initial));
	archive(cereal::make_nvp("steps", steps));
}
