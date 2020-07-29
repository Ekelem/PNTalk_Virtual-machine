/**
 * @file    main.cpp
 * @author  Tomas Lapsansky <xlapsa00@stud.fit.vutbr.cz>
 *          Erik Kelemen <xkelem01@stud.fit.vutbr.cz>
 */

#include "../lib/global.h"
#include "../lib/vm.h"
#include "../lib/daemon.h"

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

void help() {
    std::cout << "Virtual machine has to be started as:" << std::endl;
    std::cout << "./program [-f|--file <file>] [-s|--steps <steps>]|[-h|--help]" << std::endl;
    std::cout << "<file>\tname of the file" << std::endl;
    std::cout << "<steps>\tnumber of simulation steps" << std::endl;
}

int main(int argc, char * argv[]) {

    const char* const short_opts = "f:s:c:d:h";
    const option long_opts[] = {
            {"file", required_argument, nullptr, 'f'},
            {"scenario", required_argument, nullptr, 'c'},
            {"steps", required_argument, nullptr, 's'},
            {"daemon", required_argument, nullptr, 'd'},
            {"help", no_argument, nullptr, 'h'},
            {nullptr, no_argument, nullptr, 0}
    };

    auto opt = getopt_long(argc, argv, short_opts, long_opts, nullptr);
    int steps = 20;     // default value
    int daemon_port = 0;     // undefined
    std::string path = "";
    std::string scenario_path = "";
    while (opt != -1)
    {
        switch (opt)
        {
        case 'f':
            path = optarg;
            break;

        case 's':
            steps = std::stoi(optarg);
            break;

        case 'c':
            scenario_path = optarg;
            break;
        case 'd':
            daemon_port = std::stoi(optarg);
            break;
        case 'h': // -h or --help
        case '?': // Unrecognized option
        default:
            help();
            break;
        }

        opt = getopt_long(argc, argv, short_opts, long_opts, nullptr);
    }

    if(daemon_port != 0)
    {
      Daemon server;
      server.Run(daemon_port);
    }

    if(path == "") {
        help();
        exit(1);
    }

    vm virtualMachine = vm();

    if(scenario_path != "") {
        virtualMachine.registerScenario(&scenario_path);
    }

    virtualMachine.parseInput(&path);

    for (int index = 1; index < steps; index++) {
        virtualMachine.archiver.startStep();
        virtualMachine.step();
    }

    virtualMachine.archiver.generate(std::cout);
    virtualMachine.quit();

    return 0;
}