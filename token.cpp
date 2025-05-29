//token main

#include "token.h"

/*
Token::Token() 
    : token_list{
        {regex("^(class)\\b"), "CLASS_KW"},
        {regex("^(override)\\b"), "OVERRIDE_KW"},
        {regex("^(const)\\b"), "CONST_KW"},
        {regex("^(virtual)\\b"), "VIRTUAL_KW"},
        {regex(R"(\b(?:void|bool|string|char(?:8_t|16_t|32_t)?|wchar_t|short|int|long(?:\s+long)?|float|double|long\s+double|unsigned|signed|unsigned\s+(?:char|short|int|long(?:\s+long)?)|signed\s+(?:char|short|int|long(?:\s+long)?)|std::(?:array|vector|deque|list|forward_list|set|multiset|unordered_set|unordered_multiset|map|multimap|unordered_map|unordered_multimap|queue|priority_queue|stack|string|u8string|u16string|u32string|wstring|string_view|span|optional|variant|any|pair|tuple|unique_ptr|shared_ptr|weak_ptr|))\b)"), "TYPE"},
        {regex("^(if|else|elif|while|for|return|import|in|range|not)\\b"), "KEYWORD"},
        {regex("^(public|private|protected)\\b"), "ESPECIFICADOR_ACCESO"},
        {regex("^[a-zA-Z_][a-zA-Z0-9_]*"), "IDENTIFIER"},
        {regex("^\\{"), "ABRE_LLAVE"},
        {regex("^\\}"), "CIERRA_LLAVE"},
        {regex("^\\="), "IGUAL"},
        {regex("^\\;"), "END"},
        {regex("^\\("), "ABRE_PAR"},
        {regex("^\\)"), "CIERRA_PAR"},    
        {regex("^\\~"), "SQUIGLY"},
        {regex("^:"), "DOS_PUNTOS"},
        //{regex("^\\\""), "COMILLA"},
        {regex("^[0-9]+"), "NUMBER"}
        
    }
{}
*/
Token::Token()
  : token_list{
      {regex(R"(^(class)\b)"),               "CLASS_KW"},
      {regex(R"(^(override)\b)"),            "OVERRIDE_KW"},
      {regex(R"(^(const)\b)"),               "CONST_KW"},
      {regex(R"(^(virtual)\b)"),             "VIRTUAL_KW"},
      {regex(R"(^(new)\b)"),             "NEW"},
      {regex(R"(^(delete)\b)"),             "DELETE"},
      {regex(R"(^->)"),             "ARROW"},
      {regex(R"(^(return)\b)"),             "RETURN_KW"},

      /* tipos básicos + contenedores STL */
      { regex(R"(^(?:void|bool|string|char(?:8_t|16_t|32_t)?|wchar_t|short|int|long(?:\s+long)?|float|double|long\s+double|unsigned|signed|unsigned\s+(?:char|short|int|long(?:\s+long)?)|signed\s+(?:char|short|int|long(?:\s+long)?)|std::[a-zA-Z_][a-zA-Z0-9_]*)\b)"), "TYPE" },
      /* palabras reservadas que realmente te interesen */
      {regex(R"(^(if|else|while|for)\b)"), "EXTRA"},

      {regex(R"(^(public|private|protected)\b)"), "ESPECIFICADOR_ACCESO"},

      /* literales de cadena y carácter — cuéntalos como EXTRA */
      {regex(R"(^\"([^\\\"]|\\.)*\")"),           "EXTRA"},
      {regex(R"(^\'([^\\\']|\\.)*\')"),           "EXTRA"},

      /* operadores C++ que tu parser no distingue → EXTRA */
      {regex(R"(^(<<|>>|\+\+|--|\+=|-=|\*=|/=|==|!=|<=|>=|&&|\|\||::))"), "EXTRA"},

      /* comentarios (// …) o directivas # …  */
      {regex(R"(^(//.*))"),                      "EXTRA"},
      {regex(R"(^#.*)"),                         "EXTRA"},

      /* símbolos estructurales que el parser SÍ usa */
      {regex(R"(^\{)"),                         "ABRE_LLAVE"},
      {regex(R"(^\})"),                         "CIERRA_LLAVE"},
      {regex(R"(^\()"),                         "ABRE_PAR"},
      {regex(R"(^\))"),                         "CIERRA_PAR"},
      {regex(R"(^:)"),                          "DOS_PUNTOS"},
      {regex(R"(^~)"),                          "SQUIGLY"},
      {regex(R"(^=)"),                          "IGUAL"},
      {regex(R"(^;)"),                          "END"},

      /* identificadores (después de keywords para no robarles) */
      {regex(R"(^[a-zA-Z_][a-zA-Z0-9_]*)"),     "IDENTIFIER"},

      /* números → podría ser NUMBER o EXTRA */
      {regex(R"(^[0-9]+)"),                     "NUMBER"},
  }
{}



/*
vector<string> Token::tokenizer (const string& input_){
    vector<string> values;
    string input = input_;

    while(!input.empty()){
        
        bool matched = false; 

        // Skip whitespaces
        if (isspace(input[0])) {
           input.erase(0,1);
            continue;
        }
        for (const auto& [pattern, type] : token_list) {
            smatch match;
            if (regex_search(input, match, pattern)) {
                values.push_back(type);
                input = input.substr(match.length());
                matched = true;
                break;
            }
        }

        if (!matched) {
            values.push_back("EXTRA");
            input.erase(0, 1);
        }
    }
    return values;
}


*/


vector<string> Token::tokenizer(const string& line) {
    vector<string> out;
    string input = line;

    while (!input.empty()) {
        if (isspace(input[0])) { input.erase(0,1); continue; }

        bool matched = false;
        for (const auto& [pat, type] : token_list) {
            smatch m;
            if (regex_search(input, m, pat)) {
                out.push_back(type);
                input.erase(0, m.length());
                matched = true;
                break;
            }
        }

        if (!matched) {
            /* consume hasta el próximo espacio o símbolo separador */
            size_t n = input.find_first_of(" \t\r\n{}()~=:;");
            if (n == string::npos) n = input.size();
            out.push_back("EXTRA");
            input.erase(0, n);
        }
    }
    return out;
}
