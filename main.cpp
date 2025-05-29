/*
* return string vector with:
*
* Compilacion para debug:  
*    g++ -std=c++17 -g -o main *.cpp 
*
* Ejecucion con valgrind:
*    nix-env -iA nixpkgs.valgrind
*    valgrind --leak-check=full ./main
*
* Compilacion para ejecucion:  
*    g++ -std=c++17 -g -o main *.cpp 
* Ejecucion:
*    ./main > out.txt
*/

#include <iostream>
#include <fstream>
#include "token.h"

using std::ifstream;
using std::invalid_argument;
using std::getline;
using std::cout;
using std::endl;

int main() {

    string line;
    vector<string> lista;

    string fileName = "ejemplo2.cpp";
    Token objectT;
    
    ifstream file(fileName);
    if (!file.good()) {
        file.close();
        throw invalid_argument("File not found");
    }
    else {
        while(getline(file,line)) {
            auto tokens = objectT.tokenizer(line);
            for (const auto& token : tokens) { 
                cout << token << endl;
            }
        }
        cout << "EOF" << endl;
    }

    file.close();
    return 0;
}



