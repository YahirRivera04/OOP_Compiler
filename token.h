#ifndef __TOKEN_H_
#define __TOKEN_H_

#include <vector>
#include <regex>
#include <string>

#include <iostream> // borrar

using std::vector;
using std::string;
using std::pair;
using std::regex;
using std::isspace;
using std::smatch;
using std::regex_search;


class Token {
    private: 
    vector<pair<regex, string>> token_list;
    
    public:
    Token();
    vector<string> tokenizer (const string& input_);

};

#endif // __TOKEN_H_