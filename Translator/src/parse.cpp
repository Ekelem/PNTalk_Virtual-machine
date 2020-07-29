/**
 * @file    parse.cpp
 * @author  Tomas Lapsansky <xlapsa00@stud.fit.vutbr.cz>
 */

#include "../lib/parse.h"
#include "../lib/lexAnalyzator.h"

parse::parse(std::string file) {

    lex = new lexAnalyzator(file);
    sem = new semantic();
    secondCheck = false;
}

void parse::secondAnalysis(std::string file) {

    lex = new lexAnalyzator(file);
    secondCheck = true;
}

void parse::getToken() {
    if(lex != nullptr) {
        token = lex->getToken();
    }
}

void parse::start() {

    mainLine();

    getToken();

    /* main syntax */
    while(token != std::tuple<int, std::string>(0, "EOF")) {
        /* for every class */

        classCheck();
    }

}

void parse::translate(std::string inputCodePath)
{
    start();
    secondAnalysis(inputCodePath);
    start();
}

void parse::mainLine() {

    getToken();

    if(token != std::tuple<int, std::string>(1, "main")) {
        syntax_error(lex->line);
    }

    getToken();

    if(std::get<0>(token) != 20) {
        syntax_error(lex->line);
    }

    outputCode("MAIN: " + std::get<1>(token), true);

    std::string checkClassName = std::get<1>(token);

    if(secondCheck) {
        if(!sem->checkClass(&checkClassName))
            semanticError(lex->line);
    }

    getToken();

    if(std::get<0>(token) != 2) {
        syntax_error(lex->line);
    }

}

void parse::classCheck() {

    std::string classVar;

    if(token != std::tuple<int, std::string>(1, "class")) {
        syntax_error(lex->line);
    }

    getToken();

    if(std::get<0>(token) != 20) {
        syntax_error(lex->line);
    }

    classVar = std::get<1>(token);

    getToken();

    if(token != std::tuple<int, std::string>(1, "is_a")) {
        syntax_error(lex->line);
    }

    getToken();

    if(std::get<0>(token) != 20) {
        syntax_error(lex->line);
    }

    getToken();

    /* ENDLINE */
    if(std::get<0>(token) != 2) {
        syntax_error(lex->line);
    }

    getToken();

    outputCode("CLASS " + classVar + ":", true);

    if(!(sem->addClass(&classVar, secondCheck))) {
        semanticError(lex->line);
    }

    bool loop = true;

    while(loop) {

        if(token == std::tuple<int, std::string>(1, "object")) {
            /* Objects */
            outputCode("OBJ:", true);
            if(objectsCheck() != 0) {
                syntax_error(lex->line);
            }

        } else if(token == std::tuple<int, std::string>(1, "method")) {
            /* Methods */
            outputCode("FUNC ", false);
            if(methodsCheck() != 0) {
                syntax_error(lex->line);
            }

        } else if(token == std::tuple<int, std::string>(1, "sync")) {

            /* Sync */
            outputCode("SYNC ", false);
            if(syncCheck() != 0) {
                syntax_error(lex->line);
            }

        } else {
            loop = false;
        }

    }
}

int parse::objectsCheck() {

    getToken();

    /* ENDLINE */
    if(std::get<0>(token) != 2) {
        syntax_error(lex->line);
    }

    std::string funcName = "object";
    std::list<std::string> params;

    if(!(sem->addFunction(&funcName, params, secondCheck))) {
        semanticError(lex->line);
    }

    /* Possible objects */
    /* trans, place */
    bool loop = true;

    getToken();

    while(loop) {

        if(token == std::tuple<int, std::string>(1, "place")) {
            /* place */
            outputCode("PLACE ", false);
            getToken();

            checkConditions(true, false);
            outputCode("", true);

        } else if(token == std::tuple<int, std::string>(1, "trans")) {
            /* trans */
            outputCode("TRANS ", false);
            getToken();

            /* name */
            if(std::get<0>(token) != 18 && std::get<0>(token) != 19) {
                syntax_error(lex->line);
            }
            outputCode(std::get<1>(token), true);

            getToken();

            /* ENDLINE */
            if(std::get<0>(token) != 2) {
                syntax_error(lex->line);
            }

            parseTrans(true);

        } else {
            loop = false;
        }
    }

    return 0;
}

int parse::methodsCheck() {

    getToken();

    if(std::get<0>(token) == 18) {          /* Function name: with parameters */

        std::string funcName = std::get<1>(token);
        std::list<std::string> funcVars;

        outputCode(funcName + " ", false);

        do {
            getToken();

            if(std::get<0>(token) != 19) {
                syntax_error(lex->line);
            }

            outputCode(std::get<1>(token), false);
            funcVars.push_front(std::get<1>(token));

            getToken();

        } while(std::get<0>(token) == 3);   /* Delimeter */

        outputCode("", true);
        if(!(sem->addFunction(&funcName, funcVars, secondCheck))) {
            semanticError(lex->line);
        }

    } else if(std::get<0>(token) == 19) {   /* Function name without parameters */
        std::string funcName = std::get<1>(token);
        std::list<std::string> params;
        if(!(sem->addFunction(&funcName, params, secondCheck))) {
            semanticError(lex->line);
        }
        outputCode(funcName + ":", true);
        getToken();
    } else {
        syntax_error(lex->line);
    }

    /* ENDLINE */
    if(std::get<0>(token) != 2) {
        syntax_error(lex->line);
    }

    /* Possible objects */
    /* trans, place */
    bool loop = true;

    getToken();

    while(loop) {

        if(token == std::tuple<int, std::string>(1, "place")) {
            /* place */
            outputCode("PLACE ", false);
            getToken();

            checkConditions(true, false);
            outputCode("", true);

        } else if(token == std::tuple<int, std::string>(1, "trans")) {
            /* trans */
            outputCode("TRANS ", false);
            getToken();

            /* name */
            if(std::get<0>(token) != 18 && std::get<0>(token) != 19) {
                syntax_error(lex->line);
            }
            outputCode(std::get<1>(token), true);

            getToken();

            /* ENDLINE */
            if(std::get<0>(token) != 2) {
                syntax_error(lex->line);
            }

            parseTrans(true);

        } else {
            loop = false;
        }
    }

    return 0;
}

int parse::syncCheck() {

    getToken();

    /* Have to have arguments */
    if(std::get<0>(token) == 18) {

        outputCode(std::get<1>(token) + " ", false);

        do {
            getToken();

            if(std::get<0>(token) != 19) {
                syntax_error(lex->line);
            }

            outputCode(std::get<1>(token), false);

            getToken();

        } while(std::get<0>(token) == 3);   /* Delimeter */

        outputCode("", true);
    } else {
        syntax_error(lex->line);
    }

    parseTrans(false);

    return 0;

}

void parse::parseTerm() {

    getToken();

    if(std::get<0>(token) == 32) {          /* List */

        do {
            getToken();

            /* name */
            if(!isTerm()) {
                syntax_error(lex->line);
            }

            std::string output = std::get<1>(token);

            outputCode(output + " ", false);

            //parseTerm();

            getToken();

        } while(std::get<0>(token) == 3);

    } else if(std::get<0>(token) == 30) {   /* Term */

        termCalc(false);

        getToken();

    } else if(isTerm()){                    /* Others */


        std::string output = std::get<1>(token);
        if(output[0] == '(' && output[output.size() - 1] == ')') {
            output = output.substr(1, output.size() - 2);
        }

        outputCode(output, false);

        if(lex->isVariable(output)) {
            sem->addVariable(&output, secondCheck);
        }

        getToken();

    } else if(std::get<0>(token) == 33) {   /* Multiset */

        std::string output = std::get<1>(token);
        if(output[0] == '(' && output[output.size() - 1] == ')') {
            output = output.substr(1, output.size() - 2);
        }

        outputCode(output, false);

        getToken();

    } else if(std::get<0>(token) == 31) {   /* Empty */
        getToken();
    } else {
        syntax_error(lex->line);
    }
}

void parse::parseTrans(bool isTrans) {

    getToken();

    if(token == std::tuple<int, std::string>(1, "cond")) {
        outputCode("COND", true);

        getToken();

        checkConditions(false, false);
        outputCode("", true);
    }

    if(token == std::tuple<int, std::string>(1, "precond")) {
        outputCode("PRECOND", true);

        getToken();

        checkConditions(false, false);
        outputCode("", true);
    }

    if(token == std::tuple<int, std::string>(1, "guard")) {
        outputCode("GUARD:", true);

        getToken();

        checkConditions(false, true);
    }

    if(isTrans) {   /* Available only for TRANS, not for SYNC */
        if(token == std::tuple<int, std::string>(1, "action")) {
            outputCode("ACTION", true);

            getToken();

            checkConditions(false, false);
        }
    }

    if(token == std::tuple<int, std::string>(1, "postcond")) {
        outputCode("POSTCOND", true);

        getToken();

        checkConditions(false, false);
        outputCode("", true);
    }

}

void parse::checkConditions(bool isPlace, bool guard) {

    do {
        if(std::get<0>(token) == 30) {

            termCalc(guard);

            getToken();

        } else {

            /* name */
            if(std::get<0>(token) != 18 && std::get<0>(token) != 19) {
                syntax_error(lex->line);
            }

            std::string placeName = std::get<1>(token);

            if(!(lex->isVariable(placeName))) {
                syntax_error(lex->line);
            } else {
                if(isPlace) {
                    sem->addPlace(&placeName);
                } else {
                    if(!(sem->checkPlace(&placeName, secondCheck))) {
                        semanticError(lex->line);
                    }
                }
            }

            outputCode(std::get<1>(token) + ": ", false);

            parseTerm();

        }

        /* End condition */
        if((std::get<0>(token) != 3) || isPlace)
            break;

        outputCode("", true);

        getToken();

    } while(true);

    /* ENDLINE */
    if(std::get<0>(token) != 2) {
        syntax_error(lex->line);
    }

    getToken();

}

bool parse::isTerm() {
    int ret = std::get<0>(token);
    return ((11 <= ret) && (ret <= 19));
}

void parse::termCalc(bool guard) {

    std::string str = std::get<1>(token);

    str = str.substr(1, str.size() - 2);

    /* Convert to prefix */

    if(guard) {
        conditionCheck(str);
    } else if(std::regex_match(str, std::regex(".*:=.*"))) {
    /* := */

        int pos = str.find(":=");

        std::string var = str.substr(0, pos);
        var = lex->trim(var);

        if(!(lex->isVariable(var))) {
            syntax_error(lex->line);
        }

        sem->addVariable(&var, secondCheck);

        std::string term = str.substr(pos+2, std::string::npos);
        term = lex->trim(term);

        //std::cerr << term << std::endl;

        if(std::regex_match(term, std::regex("[a-z]([a-z]|[A-Z]|[0-9])* [a-z]([a-z]|[A-Z]|[0-9])*.*"))) {
            /* Possible function call */

            int i = 0;
            std::string outputVar;

            while(term[i] != ' ') {
                outputVar += term[i];
                i++;
            }

            if(lex->isVariable(outputVar)) {
                if(!(sem->checkVariable(&outputVar, secondCheck))) {
                    semanticError(lex->line);
                }
            } else {
                syntax_error(lex->line);
            }

            /* Instance variable */

            std::string callFunc;

            i++;
            int j = 0;
            while(term[i] != ' ') {
                callFunc += term[i];
                i++;
                j++;
            }

            std::list<std::string> params;

            if(callFunc[j-1] == ':') {  /* Parameters of function */
                callFunc.erase(callFunc.size() - 1, 1);

                std::string variable;

                do {
                    i++;

                    if(term[i] != ' ' && term[i] != 0) {
                        variable += term[i];
                    } else {
                        if(variable[variable.size() - 1] == ',') {
                            variable.erase(variable.size() - 1, 1);
                        }
                        outputCode("PUSHTEMP " + variable, true);
                        if(!(lex->isVariable(variable))) {
                            syntax_error(lex->line);
                        }
                        params.push_front(variable);
                        variable = "";
                    }
                } while(term[i] != 0);
            }

            if(secondCheck) {
                if(!(sem->checkFunction(&callFunc, params))) {
                    semanticError(lex->line);
                }
            }

            outputCode("CALL " + outputVar + "." + callFunc, true);
            outputCode("POPTEMP " + var, true);


        } else if(std::regex_match(term, std::regex("[A-Z]([a-z]|[A-Z]|[0-9])* new\\..*"))) {
            /* New instance */

            std::string object;
            int i = 0;
            while(term[i] != ' ') {
                object += term[i];
                i++;
            }

            if(secondCheck) {
                if(!(sem->checkClass(&object))) {
                    semanticError(lex->line);
                }
            }

            outputCode("NEW " + object, true);
            outputCode("POPTEMP " + var, true);   /* VM will push address to TEMPSTACK */

        } else {
            /* Term */
            std::queue<std::string> queue;

            queue = parse::infixToPostfix(term);
            while(!queue.empty()) {
                if(isOperator(queue.front())) {
                    if(queue.front() == "+") {
                        outputCode("CALLTEMP ADD", true);
                    } else if(queue.front() == "-") {
                        outputCode("CALLTEMP DEC", true);
                    } else if(queue.front() == "*") {
                        outputCode("CALLTEMP MUL", true);
                    } else if(queue.front() == "/") {
                        outputCode("CALLTEMP DIV", true);
                    } else {
                        syntax_error(lex->line);
                    }
                } else {
                    if(lex->isVariable(queue.front())) {
                        if(!(sem->checkVariable(&queue.front(), secondCheck))) {
                            semanticError(lex->line);
                        }
                    }
                    outputCode("PUSHTEMP " + queue.front(), true);
                }
                queue.pop();
            }

            outputCode("POPTEMP " + var, true);
        }
    } else {
    /* Function call */

        int i = 0;
        std::string outputVar;

        while(str[i] != ' ') {
            outputVar += str[i];
            i++;
        }

        /* Instance variable */

        std::string callFunc;

        i++;
        int j = 0;
        while(str[i] != ' ' && str[i] != 0) {
            callFunc += str[i];
            i++;
            j++;
        }

        if(callFunc[j-1] == ':') {          /* Parameters of function */
            callFunc.erase(callFunc.size() - 1, 1);

            std::string variable;

            do {
                i++;

                if(str[i] != ' ' && str[i] != 0) {
                    variable += str[i];
                } else {
                    outputCode("PUSHTEMP " + variable, true);
                    variable = "";
                }
            } while(str[i] != 0);
        } else if(callFunc[j-1] == '.') {   /* Without parameters */
            callFunc.erase(callFunc.size() - 1, 1);
        } else {
            syntax_error(lex->line);
        }

        outputCode("CALL " + outputVar + "." + callFunc, true);
    }
}

void parse::outputCode(const std::string str, bool newLine) {

    if(!parse::secondCheck) {
        static int line = 0;
        static int lastLine = true;
        if(lastLine) {
            line++;
            *_output << line << ":\t" << str;
        } else {
            *_output << str;
        }

        if(newLine) {
            *_output << std::endl;
            lastLine = true;
        } else {
            lastLine = false;
        }
    }

}

void parse::setOutput(std::ostream & output) {
    _output = &output;
}

bool parse::isOperator(std::string str)
{
    if(str.size() != 1) {
        return false;
    } else {
        return (!isalpha(str[0]) && !isdigit(str[0]));
    }
}

int parse::getPriority(std::string str)
{
    if (str[0] == '-' || str[0] == '+')
        return 1;
    else if (str[0] == '*' || str[0] == '/')
        return 2;
    else if (str[0] == '^')
        return 3;
    return 0;
}

std::queue<std::string> parse::infixToPostfix(std::string infix)
{
    std::string str = '(' + infix + ')';
    std::stack<std::string> parts;
    std::queue<std::string> output;

    int pos = 0;

    while ((pos != str.size())) {
        if((pos = matchPart(str, R"((\+|\-)?([0-9]+(\.[0-9]*)?(e(\+|\-)?[0-9]+)?))")) != -1) {
            /* number */
            output.push(str.substr(0, pos+1));

        } else if((pos = matchPart(str, "[a-z]([a-z]|[A-Z]|[0-9])*")) != -1) {
            /* variable */
            output.push(str.substr(0, pos+1));

        } else {
            /* something else */
            pos = 0;

            if(str[0] == ' ') {

            } else if(str[0] == '('){
                parts.push(std::string(1, str[0]));

            } else if(str[0] == ')') {
                while(parts.top() != "(") {
                    output.push(parts.top());
                    parts.pop();
                }

                parts.pop();
            } else {
                if (isOperator(parts.top())) {
                    while (getPriority(std::string(1, str[0])) <= getPriority(parts.top())) {
                        output.push(parts.top());
                        parts.pop();
                    }

                    // Push current Operator on stack
                    parts.push(std::string(1, str[0]));
                }
            }
        }

        str = str.erase(0, pos + 1);
    }

    while(!parts.empty()) {
        if(parts.top() != std::string("(", 1))
            output.push(parts.top());
        parts.pop();
    }

    return output;
}

int parse::matchPart(const std::string str, const std::string reg) {

    std::string toMatch = reg + ".*";
    if(std::regex_match(str, std::regex(toMatch))) {

        int pos = 0;
        std::string sub;
        for(int i = 0; i < str.size(); i++) {
            sub = str.substr(0, i+1);
            if(std::regex_match(sub, std::regex(reg))) {
                pos = i;
            } else {
                return pos;
            }
        }

        return pos;

    } else {
        return -1;
    }
}

int parse::conditionCheck(std::string str) {

    if(std::regex_match(str, std::regex("[a-z]([a-z]|[A-Z]|[0-9])* [a-z]([a-z]|[A-Z]|[0-9])*.*"))) {
        /* Sync */

        int i = 0;
        std::string outputVar;

        while(str[i] != ' ') {
            outputVar += str[i];
            i++;
        }

        /* Instance variable */

        std::string callFunc;

        i++;
        int j = 0;
        while(str[i] != ' ') {
            callFunc += str[i];
            i++;
            j++;
        }

        if(callFunc[j-1] == ':') {  /* Parameters of function */
            callFunc.erase(callFunc.size() - 1, 1);

            std::string variable;

            do {
                i++;

                if(str[i] != ' ' && str[i] != 0 && str[i] != '.') {
                    variable += str[i];
                } else {
                    outputCode("PUSHTEMP " + variable, true);
                    variable = "";
                }
            } while(str[i] != 0 && str[i] != '.');
        }

        outputCode("CALL " + outputVar + "." + callFunc, true);

        str.erase(0, i+1);
    }

    /* Conditions */

    int pos = 0;
    int comparator = 0;

    /* Choose the comparator */
    if((pos = str.find('<')) != std::string::npos) {
        comparator = 1;
    } else if((pos = str.find('>')) != std::string::npos) {
        comparator = 2;
    } else if((pos = str.find("<=")) != std::string::npos) {
        comparator = 3;
    } else if((pos = str.find(">=")) != std::string::npos) {
        comparator = 4;
    } else {
        syntax_error(lex->line);
    }

    /* Parse to two terms */

    std::string term1 = str.substr(0, pos);
    std::string term2;

    if(comparator == 1 || comparator == 2) {
        /* Short comparators */
        term2 = str.substr(pos + 2, str.size());
    } else {
        /* Long comparators */
        term2 = str.substr(pos + 3, str.size());
    }

    term1 = lex->trim(term1);
    term2 = lex->trim(term2);

    std::queue<std::string> queue;

    queue = parse::infixToPostfix(term1);
    while(!queue.empty()) {
        if(isOperator(queue.front())) {
            if(queue.front() == "+") {
                outputCode("CALLTEMP ADD", true);
            } else if(queue.front() == "-") {
                outputCode("CALLTEMP DEC", true);
            } else if(queue.front() == "*") {
                outputCode("CALLTEMP MUL", true);
            } else if(queue.front() == "/") {
                outputCode("CALLTEMP DIV", true);
            } else {
                syntax_error(lex->line);
            }
        } else {
            outputCode("PUSHTEMP " + queue.front(), true);
        }
        queue.pop();
    }

    outputCode("POPTEMP TEMP1", true);

    queue = parse::infixToPostfix(term2);
    while(!queue.empty()) {
        if(isOperator(queue.front())) {
            if(queue.front() == "+") {
                outputCode("CALLTEMP ADD", true);
            } else if(queue.front() == "-") {
                outputCode("CALLTEMP DEC", true);
            } else if(queue.front() == "*") {
                outputCode("CALLTEMP MUL", true);
            } else if(queue.front() == "/") {
                outputCode("CALLTEMP DIV", true);
            } else {
                syntax_error(lex->line);
            }
        } else {
            outputCode("PUSHTEMP " + queue.front(), true);
        }
        queue.pop();
    }

    outputCode("POPTEMP TEMP2", true);

    outputCode("PUSHTEMP TEMP1", true);
    outputCode("PUSHTEMP TEMP2", true);
    outputCode("CALLTEMP ", false);

    outputCode(std::to_string(comparator), true);

    return 0;
}
