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

int main(int argc, char* argv[]) {

    string line;
    vector<string> lista;

    //string fileName = "ejemplo2.cpp";
    Token objectT;

    if (argc < 2) {
        std::cerr << "Usage: lexer <filename>\n";
        return 1;
    }

    std::ifstream file(argv[1]);
    if (!file) {
        std::cerr << "Could not open file: " << argv[1] << "\n";
        return 1;
    }
    
    std::ifstream file(argv[1]);

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



