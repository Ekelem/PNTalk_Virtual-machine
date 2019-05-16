/**
 * @file    main.cpp
 * @author  Tomas Lapsansky <xlapsa00@stud.fit.vutbr.cz>
 */

#include "../lib/global.h"
#include "../lib/lexAnalyzator.h"
#include "../lib/parse.h"

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

    if(argc != 3) {
        help();
        exit(1);
    }

    if(strcmp(argv[1], "-f") != 0) {
        help();
        exit(1);
    }

    string file = argv[2];

    lexAnalyzator lex(file);
    parse parser(file);

    parser.start();

    parser.secondAnalysis(file);

    /* Second start for semnatic analysis */
    parser.start();

    return 0;
}