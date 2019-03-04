#include "../lib/global.h"
#include "../lib/lex_analyzator.h"

using namespace std;

int main(int argc, char * argv[]) {

    /* TODO parsovanie argumentov */

    if(argc != 3) {
        exit(1);
    }

    if(strcmp(argv[1], "-f") != 0) {
        exit(1);
    }

    string file = argv[2];

    lex_analyzator lex(file);

    tuple<int, string> token;

    cout << "-- Start --" << endl << endl;

    do {
        token = lex.getToken();

        cout << get<0>(token) << "\t" << get<1>(token) << endl;

    } while(get<0>(token) != 0);

    cout << endl << "-- End --" << endl;

    return 0;
}