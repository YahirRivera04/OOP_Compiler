#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include "tokenizer/token.h"

using namespace std;
vector<string> tokens;
string l;

static string la;  

[[noreturn]] void syntaxError(const string& expected,
                              const string& rule = "")
{
    cerr << "\n┌─[ Syntax ERROR ]─────────────────────────────\n";
    if (!rule.empty())
        cerr << "│  On the rule: " << rule << "\n";
    	cerr << "│  Expected: "   << expected
         	<< "\n│  But got:    " << la << '\n'
         	<< "└────────────────────────────────────────────────────\n";
    exit(EXIT_FAILURE);
}

struct OopMetrics {
    bool   hasClass        = false;  
    bool   hasInheritance  = false;  
    bool   hasVirt         = false;  
    bool   hasEncapsulation= false;  
    bool   hasDynamicAlloc = false;  
    bool   hasArrowCalls   = false;  
    int    numClasses      = 0;
    int    numNew          = 0;
    int    numDelete       = 0;
} oop;



void handler(char* argv[]) {

    std::ifstream file(argv[1]);
    //string fileName = "procedural.cpp";
    string line;
    vector<string> lista;

    Token objectT;
    //ifstream file(fileName);

    if (!file.good()) {
        file.close();
        throw invalid_argument("File not found");
    }
    else {
		tokens.push_back("ABRE_LLAVE"); // Checar 
        while(getline(file,line)) {
            vector<string> tokenTemp = objectT.tokenizer(line);
        	for (const string& token : tokenTemp) { 
                tokens.push_back(token);
            }
        }
		tokens.push_back("CIERRA_LLAVE"); // Checar 
		tokens.push_back("EOF");
    }
    file.close();
}


bool programa();
bool definicion();
bool classDefinicion();
bool declaracionTipo();
bool declaracionVarSinTipo();
bool herencia();
bool listaHerencia();
bool heredaUno();
bool declaracionTipoAlpha();
bool declaracionTipoAlpha1();
bool declaracionTipoAlpha2();
bool callStmt();
bool declaracionPostPar();
bool declaracionPostPar1();
bool declaracionPostPar2();  
bool declaracionVarInit();
bool declaracionVarInit1();
bool declaracionVarInit2();
bool miembro();
bool miembroTipo();
bool constructor();
bool destructor();
bool cuerpoFuncion();
bool parametro();
bool parametros();
bool isStartOfParametro();
bool isStartOfHerencia();
bool cuerpoClase();
bool cuerpoClase1();
bool cuerpoClase2();
bool EspAcc();
bool EspAcc1();
bool EspAcc2();
bool metvar();
bool metvar1();
bool metvar2();
bool deleteStmt();
bool code();
bool code1();
bool code2();
bool code3();
bool code4();
bool code5();
bool code6();
bool code7();
bool code8();
bool endOfFunc();

bool simpleCallStmt();

bool initList();
bool initListAlpha();
bool initItem();

bool returnStmt();

bool funcCode();
bool funcCode1();
bool funcCode2();
bool funcCode3();
bool funcCode4();

bool args();
bool isStartOfExpr();
bool expr();
bool funcionDefinicion();
bool codeNonEmpty();



bool declaracion();
bool declaracion1();
bool declaracion2();

const std::string& la2();



// Match function
bool match(const string& token, const string& rule = "") {
    if (l == token) {
		cout << "l is :" << l <<endl;
        l = tokens.front();
		tokens.erase(tokens.begin());
		cout << "It matched - l now is: "<<l <<endl;
		return true;
    }
    else{
		syntaxError(token, rule);
	}
}



bool initList(){
	if(initItem() && initListAlpha()){
		return true;
	}
	else{
		return false;
	}
}


bool initListAlpha() {                 // mismo InitList'
    if (l != "COMA") return true;      // ε

    match("COMA");
	if(initItem() && initListAlpha()){
		return true;
	}
	else{
		return false;
	}
}



bool initItem(){
	if(l == "IDENTIFIER"){
		if(match("IDENTIFIER") && match("ABRE_PAR") && ( !isStartOfExpr() || args() ) && match("CIERRA_PAR")){
			return true;
		}
	}
	else{
		return false;
	}
}

inline bool isStartOfExpr() { return l == "EXTRA"|| l == "IDENTIFIER"; }

bool expr() {   
	if(l == "EXTRA" || l == "IDENTIFIER" || l == "NUMBER"){
		if(match(l)){
			return true;
		}
	}else{
		return false;
	}
}

bool returnStmt()                   
{
    if (l != "RETURN_KW") return false;

    match("RETURN_KW");

    /* consumir todo lo que venga hasta el ‘;’             */
    while (l != "END" && l != "EOF") {
        l = tokens.front();
        tokens.erase(tokens.begin());
    }
    return match("END");
}

bool skipStmt() {                 
    if (l != "EXTRA") return false;

    match("EXTRA");               // if / while / for / etc.

    // come todo hasta el ‘END’ que cierra la sentencia
    while (l != "END" && l != "EOF") {
        l = tokens.front();
        tokens.erase(tokens.begin());
    }
    return match("END");
}

bool args() {
    if (!expr()){
		return false;
	}                      
        

    while (l == "COMA") {             // { COMA Expr }
        match("COMA");
        if (!expr()) return false;
    }
    return true;
}


bool declaracionTipo(){
	if (l == "TYPE"){ 
		if (match("TYPE") && match("IDENTIFIER") && declaracionTipoAlpha()){
			return true;
		}
	}
	else{
		return false;
	}
}

bool declaracionVarSinTipo(){
	if (l == "IDENTIFIER"){
		if (match("IDENTIFIER") && declaracionTipoAlpha()){
			return true;
		}
	}
	else{
		return false;
	}
}

bool isStartOfParametro() { 
	if (l == "TYPE"){
		return true;
	}          
	else{
		return false;
	}
}


bool parametro() {
	if ( l == "TYPE"){
		if(match("TYPE") && match("IDENTIFIER")){
			return true;
		}
	}
    else{
		return false;
	}
}


bool parametros() {
    if (!isStartOfParametro())        // ε
        return true;

    parametro();                      // al menos uno
    while (l == "COMA") {
        match("COMA");
        parametro();
    }
    return true;
}



bool declaracionTipoAlpha(){
	if (declaracionTipoAlpha1() || declaracionTipoAlpha2()){
		return true;
	}
	else{
		return false;
	}
}
bool declaracionTipoAlpha1(){
	if (l == "ABRE_PAR"){
		match("ABRE_PAR");
		if(isStartOfParametro()){
			parametros();
		}
		match("CIERRA_PAR");
		return declaracionPostPar();
	}
	else{
		return false;
	}
}

bool declaracionTipoAlpha2(){
	if(declaracionVarInit()){
		return true;
	}
	else{
		return false;
	}
}


bool declaracionPostPar(){
	if(declaracionPostPar1() || declaracionPostPar2()){
		return true;
	}
	else{
		return false;
	}
}

bool declaracionPostPar1(){
	if(l == "OVERRIDE_KW"){
		match("OVERRIDE_KW"); // OPCIONAL
		oop.hasVirt = true;
	}
	if(l == "CONST_KW"){
		match("CONST_KW"); // OPCIONAL
	}

	if(cuerpoFuncion()){
		return true;
	}
	else{
		return false;
	}
}
bool declaracionPostPar2(){
	if(l == "END"){
		if(match("END")){
			return true;
		}
	}
	else{
		return false;
	}
}

bool deleteStmt() {      
	if(l == "DELETE"){
		if(match("DELETE") && match("IDENTIFIER") && match("END")){
			oop.hasDynamicAlloc = true;
			++oop.numDelete;
			return true;
		}
	}          // DELETE IDENTIFIER ;
	else{
		return false;
	}
}

bool declaracionVarInit(){
	if(declaracionVarInit1() || declaracionVarInit2()){
		return true;
	}
	else{
		return false;
	}
}


	bool declaracionVarInit1()
{
    if (l != "IGUAL") return false;
    match("IGUAL");

    /* ---------------------------  caso especial:  new …  */
    if (l == "NEW") {
        match("NEW");                         // new
        if (l == "TYPE" || l == "IDENTIFIER") // tipo o clase
            match(l);
        else
            return false;

        if (!match("ABRE_PAR"))  return false;
        if (isStartOfExpr())     args();      // [Args]
        if (!match("CIERRA_PAR"))return false;
        if (!match("END"))        return false;

        oop.hasDynamicAlloc = true;
        ++oop.numNew;
        return true;
    }

    /* -------------  cualquier otra expresión hasta ‘;’  */
    while (l != "END" && l != "EOF") {        
        l = tokens.front();
        tokens.erase(tokens.begin());
    }
    return match("END");                      
}


bool declaracionVarInit2(){
	if (l == "END"){
		if(match("END")){
			return true;
		}
	}
	else{
		return false;
	}
}

bool cuerpoClase(){
	if (cuerpoClase2() || cuerpoClase1() ){
		return true;
	}
	else{
		return false;
	}
}


bool simpleCallStmt()
{
    if (l != "IDENTIFIER")                 
        return false;
    if (tokens.empty() || tokens[0] != "ABRE_PAR")   // look-ahead
        return false;

    match("IDENTIFIER");
    match("ABRE_PAR");                     // entramos en (...)

    int depth = 1;                         // para anidar paréntesis
    while (depth > 0 && l != "EOF") {
        l = tokens.front();
        tokens.erase(tokens.begin());
        if (l == "ABRE_PAR")   ++depth;
        if (l == "CIERRA_PAR") --depth;
    }
    /*  ahora l es CIERRA_PAR  */
    match("CIERRA_PAR");
    return match("END");                   // ‘;’
}

bool cuerpoClase1(){
	if(EspAcc() && cuerpoClase()){
			return true;
	}
	else{
		return false;
	}
}

bool cuerpoClase2(){
	//lookahead de 1 en este caso... cuerpoClase }
	if (l == "CIERRA_LLAVE"){
		
		return true;
	}
	else{
		return false;
	}

}

bool EspAcc(){
	if (EspAcc1() || EspAcc2()){
		return true;
	}
	else{
		return false;
	}
}
bool EspAcc1(){
	if(l == "ESPECIFICADOR_ACCESO"){
		if(match("ESPECIFICADOR_ACCESO") && match("DOS_PUNTOS") && metvar()){
			oop.hasEncapsulation = true;
			return true;
		}
	}
	else{
		return false;
	}
}

bool EspAcc2(){
	if(metvar()){
		return true;
	}
	else{
		return false;
	}
}

bool metvar(){
	if (metvar1() || metvar2()){
		return true;
	}
	else{
		return false;
	}
}

bool metvar1(){
	if (miembro() && metvar()){
		return true;
	}
	else{
		return false;
	}
}

bool metvar2(){
	if (l == "CIERRA_LLAVE"|| l == "ESPECIFICADOR_ACCESO"){
		return true;
	}
	else{
		return false;
	}
}



bool code(){
	if (code1() || code3() || code4() || simpleCallStmt()||code7() ||code6() ||code2()){
		return true;
	}
	else{
		return false;
	}
}
bool code6(){
	if (l == "END"){
		if(match("END") && code())
		{
			return true;
		}
	}
	else{
		return false;
	}
}

bool code7(){
	if (l == "IDENTIFIER"){
		if(match("IDENTIFIER") && code())
		{
			return true;
		}
	}
	else{
		return false;
	}
}

bool code1(){
	if (l == "EXTRA"){
		if(match("EXTRA") && code())
		{
			return true;
		}
	}
	else{
		return false;
	}
}
bool code3(){
	if(declaracionTipo() && code()){
		return true;
	}
	else{
		return false;
	}
}


bool code4(){
	if(funcionDefinicion() && code()){
		return true;
	}
	else{
		return false;
	}
}

bool code2(){
	if (l == "CIERRA_LLAVE"){
		match("CIERRA_LLAVE");   
		return true;
	}
	if (l == "CLASS_KW") {
        return true;          // ← sin  match()
    }
	else{
		return false;
	}
}

inline bool isCallStmt() {
    return l == "IDENTIFIER" && tokens.size() > 0 && tokens[0] == "ARROW";
}

bool callStmt() {    
	if(isCallStmt()){
		if(match("IDENTIFIER")
         && match("ARROW")
         && match("IDENTIFIER")
         && match("ABRE_PAR")
         && ( !isStartOfExpr() || args() )   // [Args] opcional
         && match("CIERRA_PAR")
         && match("END")){
			oop.hasArrowCalls = true;
			return true;
		 }
	}                  // IDENTIFIER -> IDENTIFIER ( ) ;
	else{
		return false;
	}
}



bool funcCode() {                        // Secuencia de sentencias
    if (endOfFunc()) return true;        // ε  ← sale sin recursión

    // Una sentencia obligatoria
    if (   skipStmt() 
		|| returnStmt()
		|| callStmt()
		|| simpleCallStmt()
        || deleteStmt()
        || funcCode4()      
        || funcCode1()      
        || funcCode2() )    // EXTRA
    {
        return funcCode();              
    }
    return false;                       // error de sintaxis
}

bool funcCode2(){
	if (l == "EXTRA"){
		if(match("EXTRA") && funcCode())
		{
			return true;
		}
	}
	else{
		return false;
	}
}
bool funcCode1(){
	if(declaracionTipo() && funcCode()){
		return true;
	}
	else{
		return false;
	}
}

bool endOfFunc() {                       // FIRST(ε)
    return l == "CIERRA_LLAVE"
        || l == "EOF"
        || l == "CLASS_KW";
}


bool funcCode4(){
	if (declaracionVarSinTipo() && funcCode()){
		return true;
	}
	else{
		return false;
	}
}




bool cuerpoFuncion(){
	if(l == "ABRE_LLAVE"){
		if (match("ABRE_LLAVE") && funcCode() && match("CIERRA_LLAVE")){
			return true;
		}
	}
	else{
		return false;
	}
}
inline bool isStartOfInitList() { return l == "DOS_PUNTOS"; }
bool constructor(){
	if (l == "IDENTIFIER"){
		if(match("IDENTIFIER") && match("ABRE_PAR") && ( !isStartOfParametro() || parametros() ) && match("CIERRA_PAR") && ( !isStartOfInitList() || ( match("DOS_PUNTOS") && initList() ) ) && cuerpoFuncion()){   // ← aquí el opcional
			return true;
		}
	}
	else{
		return false;
	}
}

bool destructor(){
	if(l == "SQUIGLY"){
		if(match("SQUIGLY") && match("IDENTIFIER") && match("ABRE_PAR") && match("CIERRA_PAR") && cuerpoFuncion()){
			return true;
		}
	}
	else{
		return false;
	}
}

bool funcionDefinicion(){
	if (l == "TYPE"){
		if(match("TYPE") && match("IDENTIFIER") && match("ABRE_PAR") && ( !isStartOfParametro() || parametros() ) && match("CIERRA_PAR") && cuerpoFuncion()){
			return true;
		}
		else{
			return false;
		}
	}
}

bool miembro(){
	if (l == "VIRTUAL_KW"){
		match("VIRTUAL_KW");
		oop.hasVirt = true;
	}
	if (constructor() || destructor() || miembroTipo()){
		return true;
	}
	else{
		return false;
	}
}

bool miembroTipo(){
	if(declaracionTipo()){
		return true;
	}
	else{
		return false;
	}
}
inline bool isStartOfHerencia() {          // FIRST(Herencia)
    return l == "DOS_PUNTOS";
}

bool herencia() {
	if ( l == "DOS_PUNTOS"){
		if(match("DOS_PUNTOS") && listaHerencia()){
			oop.hasInheritance = true;
			return true;
		}
	}
	else{
		return false;
	}
}

bool listaHerencia() {
    if (!heredaUno()) return false;
    while (l == "COMA") {
        match("COMA");
        if (!heredaUno()) return false;
    }
    return true;
}

bool heredaUno(){
	if (l == "ESPECIFICADOR_ACCESO"){
		if(match("ESPECIFICADOR_ACCESO") && match("IDENTIFIER")){
			return true;
		}
	}
	else{
		return false;
	}
}

bool classDefinicion() {
	
    if (l == "CLASS_KW") {
		if(match("CLASS_KW") && match("IDENTIFIER") && (!isStartOfHerencia() || herencia()) && match("ABRE_LLAVE") && cuerpoClase() && match("CIERRA_LLAVE") && match("END"))
		{
			oop.hasClass = true;
        	++oop.numClasses;
			return true;
		}
    }else{
		return false;
	}
}

bool codeNonEmpty() {
    std::size_t before = tokens.size();   // cuántos tokens había
    if (!code()) return false;            // code falló de verdad

    return tokens.size() < before;        // ← ¿consumió alguno?
}

bool definicion(){
	if(classDefinicion() || declaracionTipo() || codeNonEmpty()){
		return true;
	}
	else{
		return false;
	}
}

bool declaracion(){
	if (declaracion1() || declaracion2()){
		return true;
	}else{
		return false;
	}
}

bool declaracion1(){
	if(definicion() && declaracion()){
		return true;
	}
	else{
		return false;
	}
}

bool declaracion2(){
	if (l == "CIERRA_LLAVE"){
		return true;
	}
	else{
		return false;
	}
}

bool programa() {
    if (l == "ABRE_LLAVE") {
		
        if (match("ABRE_LLAVE") && declaracion() && match("CIERRA_LLAVE")){
			return true;
		}
    }else{
		false;
	}
}

int main(int argc, char* argv[]) {
	
	handler(argv);
	for (const std::string& s : tokens) {
        std::cout << s << '\n';
    }
	do {
        l = tokens.front();
		tokens.erase(tokens.begin());
		// Programa is a start symbol.
	    programa();

    } while (l != "EOF");

    if (l == "EOF"){
		cout << "Parsing Successful" << endl;
		cout<<endl;
		cout<<endl;
		cout << "\n--- OOP Features ---\n";
		cout << "Classes:           " << oop.numClasses      << '\n';
		cout << "Inheritance:         " << (oop.hasInheritance ? "yes" : "no") << '\n';
		cout << "Encapsulation:    " << (oop.hasEncapsulation ? "yes" : "no") << '\n';
		cout << "Polimorfism:     " << (oop.hasVirt ? "yes" : "no") << '\n';
		cout << "new/delete usados: " << (oop.hasDynamicAlloc ? "yes" : "no") 
                              << "  (new="<<oop.numNew<<" delete="<<oop.numDelete<<")\n";
		cout << "obj->met() calls: " << (oop.hasArrowCalls ? "yes" : "no") << '\n';

		cout<<endl;
		cout<<endl;

		bool esOOP = oop.hasClass &&(oop.hasInheritance || oop.hasVirt || oop.hasEncapsulation);
		cout << "\n¿Is the file OOP?  " << (esOOP ? "✓ YES" : "✗ NO") << '\n';
		
	}
        
	return 0;
}