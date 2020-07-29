/**
 * @file    main.cpp
 * @author  Tomas Lapsansky <xlapsa00@stud.fit.vutbr.cz>
 */

#include "../lib/global.h"
#include "../lib/lexAnalyzator.h"
#include "../lib/parse.h"
#include "../lib/daemon.h"
#include <getopt.h>

using namespace std;

void help(){
    std::cout << "This compiler compile PNTalk for virtual mechine for OOPN." << std::endl;
    std::cout << "Author:\tTomas Lapsansky" << std::endl << std::endl;
    std::cout << "Bachelor thesis 2018/2019" << std::endl;
    std::cout << "FIT VUT Brno" << std::endl;

    std::cout << "Compiler has to be started as:" << std::endl;
    std::cout << "./program -f <file>" << std::endl;
    std::cout << "<file>\tname of the file" << std::endl << std::endl;

    std::cout << "Program executes output code to standard output." << std::endl;
}

int main(int argc, char * argv[]) {

    const char* const short_opts = "f:d:h";
    const option long_opts[] = {
            {"file", required_argument, nullptr, 'f'},
            {"daemon", required_argument, nullptr, 'd'},
            {"help", no_argument, nullptr, 'h'},
            {nullptr, no_argument, nullptr, 0}
    };

    auto opt = getopt_long(argc, argv, short_opts, long_opts, nullptr);
    int daemon_port = 0;     // undefined
    std::string file = "";
    while (opt != -1)
    {
        switch (opt)
        {
        case 'f':
            file = optarg;
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

    if (daemon_port != 0)
    {
        Daemon server;
        server.Run(daemon_port);
    }

    if (file == "")
    {
        help();
        exit(1);
    }

    lexAnalyzator lex(file);
    parse parser(file);

    parser.start();

    parser.secondAnalysis(file);

    /* Second start for semnatic analysis */
    parser.start();

    return 0;
}