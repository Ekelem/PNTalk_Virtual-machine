/**
 * @file    main.cpp
 * @author  Tomas Lapsansky <xlapsa00@stud.fit.vutbr.cz>
 */

#include "../lib/global.h"
#include "../lib/vm.h"

void badStarting() {
    std::cout << "Virtual machine has to be started as:" << std::endl;
    std::cout << "./program -f <file>" << std::endl;
    std::cout << "<file>\tname of the file" << std::endl;
}

void showStartingDialog(vm* virtualMachine) {
    std::cout << "This virtual machine simulates Object Oriented Petri Nets implemented by PNTalk." << std::endl;
    std::cout << "Author:\tTomas Lapsansky" << std::endl << std::endl;
    std::cout << "Bachelor thesis 2018/2019" << std::endl;
    std::cout << "FIT VUT Brno" << std::endl;

    virtualMachine->help();
}

int main(int argc, char * argv[]) {

    if(argc != 3) {
        badStarting();
        exit(1);
    }

    if(strcmp(argv[1], "-f") != 0) {
        badStarting();
        exit(1);
    }

    std::string file = argv[2];

    vm virtualMachine = vm();

    virtualMachine.parseInput(&file);

    std::string str;

    showStartingDialog(&virtualMachine);

    while(true) {
        std::cout << "Enter input: ";
        std::cin >> str;

        if(str == "c") {
            break;
        } else if(str == "s" || str == "step") {
            virtualMachine.step();
        } else if(str == "r" || str == "run") {
            virtualMachine.run();
        } else if(str == "d" || str == "detail") {
            virtualMachine.detail();
        } else {
            virtualMachine.help();
        }
    }

    virtualMachine.quit();

    return 0;
}