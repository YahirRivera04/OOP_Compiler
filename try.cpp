#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include <string>
#include <vector>
#include <iostream>


using namespace std;


/*
vector<string> tokens = {"ABRE_LLAVE","EXTRA","EXTRA","CLASS_KW","IDENTIFIER","ABRE_LLAVE","ESPECIFICADOR_ACCESO","DOS_PUNTOS",
	"TYPE", "IDENTIFIER","IGUAL","EXTRA","END","TYPE","IDENTIFIER","END",
	"IDENTIFIER", "ABRE_PAR", "TYPE","IDENTIFIER", "CIERRA_PAR","ABRE_LLAVE","IDENTIFIER","IGUAL","IDENTIFIER","END", "CIERRA_LLAVE","CIERRA_LLAVE",
	"END","CIERRA_LLAVE", "EOF"};

*/


#include <fstream>
#include "token.h"

using std::ifstream;
using std::invalid_argument;
using std::getline;
using std::cout;
using std::endl;


struct OopMetrics {
    bool   hasClass        = false;  // class ó struct
    bool   hasInheritance  = false;  // ':' Base
    bool   hasVirt         = false;  // virtual/override
    bool   hasEncapsulation= false;  // public/private/protected
    bool   hasDynamicAlloc = false;  // new/delete
    bool   hasArrowCalls   = false;  //  obj->met()
    int    numClasses      = 0;
    int    numNew          = 0;
    int    numDelete       = 0;
} oop;


vector<string> tokens;
void handler() {

    string line;
    vector<string> lista;

    string fileName = "ejemplo.cpp";
    Token objectT;
    ifstream file(fileName);
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



//FIXME

/***************************
Example:
Grammar:	

TOKENS


{ ABRE_LLAVE    
} CIERRA_LLAVE
; END 
( ABRE_PAR
) CIERRE_PAR
SQUIGLY 
: DOS_PUNTOS
CLASS CLASS_KW

NEW 
DELETE 

a-zA-Z_ if not keyword = IDENTIFIER 

EXTRA

'class', 'struct', 'public', 'private', 'protected', 
'virtual', 'override', 'new', 'delete', 'int', 'float', 
'double', 'char', 'bool', 'void', 'long', 'short', 				KEYWORD
'unsigned', 'signed', 'this', 'extends', 'implements'

LEXER 



PARSER : 

GRAMATICA

Programa { Declaracion  } EOF

Declaracion —> Definicion Declaracion | Epsylon 
Definicion —> ClaseDefinicion | DeclaracionTipo | code

ClaseDefinicion —> CLASS_KW IDENTIFIER [ Herencia ] ABRE_LLAVE CuerpoClase CIERRA_LLAVE END

Herencia —> DOS_PUNTOS ListaHerencia 
ListaHerencia —> HeredaUno { COMA HeredaUno } 
HeredaUno —> ESPECIFICADOR_ACCESO IDENTIFIER

FuncionDefinicion —> TIPO IDENTIFIER ABRE_PAR [ Parametros ] CIERRA_PAR Cuerpofuncion

DeclaracionTipo —> TIPO IDENTIFIER DeclaracionTipoAlpha


DeclaracionTipoAlpha —> ABRE_PAR [ Parametros ] CIERRA_PAR DeclaracionPostPar | DeclaracionVarInit 


DeclaracionPostPar —> CuerpoFuncion | END

DeclaracionVarInit —> IGUAL NEW TIPO ABRE_PAR [Args] CIERRA_PAR END | END | IGUAL (EXTRA | IDENTIFIER) END


	


Cuerpoclase —> EspAcc CuerpoClase | Epsylon

EspAcc —> ESPECIFICADOR_ACCESO DOS_PUNTOS MetVar | MetVar

MetVar —>  Miembro MetVar | Epsylon 

Miembro —> Constructor | Destructor | MiembroTipo


Constructor —> IDENTIFIER ABRE_PAR [Parametros] CIERRA_PAR [ DOS_PUNTOS InitList ] CuerpoFuncion

Destructor       → SQUIGLY IDENTIFIER ABRE_PAR CIERRA_PAR CuerpoFuncion

MiembroTipo —> DeclaracionTipo

CuerpoFuncion —> ABRE_LLAVE code CIERRA_LLAVE 
declaracionVarSinTipo —>  IDENTIFIER IGUAL IDENTIFIER END

code —> EXTRA code
		| FuncionDefinicion code
		| declaracionTipo code
		| declaracionVarSinTipo code
		| Epsylon

Parametros —> Param { COMA Param } 
Param —> TIPO IDENTIFIER 

Args —> Expr { COMA Expr } 

Expr —> EXTRA 

InitList —> IDENTIFIER ( ABRE_PAR Args? CIERRA_PAR)
returnStmt → KEYWORD ( NUMBER | IDENTIFIER ) END


***************************/
string l;

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

void error(){
	printf("Error\n");
	exit(-1);
}


// Match function
bool match(string token) {
    if (l == token) {
		cout << "l hizo match con el token: "<< l << " ";
        l = tokens.front();
		tokens.erase(tokens.begin());
		cout << "ahora l es : "<< l << " "<<endl;
		cout << "Size vec: " <<tokens.size()<<endl;
		return true;
    }
    else
		cout << "match hizo error con l: "<< l << " "<<endl;
		error();
}

/*
// Definition of E' as per the given production
bool E_alpha(){
	if(E_alpha_1() || E_alpha_2()){
		return true;
	}else{
		error();
	}
}
*/
// Definition of E_2' as per the given production

//  {"ABRE_LLAVE", "CLASS_KW", "IDENTIFIER", "ABRE_PAR","CUERPO_CLASE","CIERRA_PAR",";","CIERRA_LLAVE","EOF"};


// CLASEEEEEEEEEEEEE

/*

miembroClase --> Constructor | Destructor | Virtual | Private | Public | Protected | 

*/

bool initList(){
	cout << "entro en INITLIST" <<endl;
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
	cout << "Entro en initITEM"<<endl;
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
	cout << "Entro en expr"<<endl;
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
    if (l == "RETURN_KW"){
		match("RETURN_KW");                   // return
		if (l == "NUMBER" || l == "IDENTIFIER"){
			if(match(l) && match("END")){
				return true;
			}
		}
	} 
	else{
		return false;
	}
}

bool args() {
    if (!expr()){
		cout<<"ENTRO EN NO EXPR"<<endl;
		return false;
	}                      // primer Expr obligatorio
        

    while (l == "COMA") {             // { COMA Expr }
		cout<<"ENTRO EN COMA"<<endl;
        match("COMA");
        if (!expr()) return false;
    }
    return true;
}


bool declaracionTipo(){
	cout << "En declaracionTipo"<<endl;
	if (l == "TYPE"){ // HERE
		if (match("TYPE") && match("IDENTIFIER") && declaracionTipoAlpha()){
			return true;
		}
	}
	else{
		return false;
	}
}

bool declaracionVarSinTipo(){
	cout << "En declaracionTipo"<<endl;
	if (l == "IDENTIFIER"){
		if (match("IDENTIFIER") && declaracionTipoAlpha()){
			return true;
		}
	}
	else{
		return false;
	}
}

/*

bool declaracionVarSinTipo(){
	if(l == "IDENTIFIER"){
		if(match("IDENTIFIER") && match("IGUAL") && match("IDENTIFIER") && match("END")){
			return true;
		}
	}
	else{
		return false;
	}
}
*/
bool isStartOfParametro() { 
	if (l == "TYPE"){
		return true;
	}          // FIRST(Parametro) == {TIPO}
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
	cout << "En declaracionTipoAlpha"<<endl;
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
	cout<<"EN DELETE STMT" <<endl;   
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
bool declaracionVarInit1(){
	if (l == "IGUAL"){
		match("IGUAL"); // Para ver el siguiente Lookahead l
		if(l == "NEW"){
			match("NEW");
			if(l == "TYPE" || l == "IDENTIFIER"){
				if(match(l) && match("ABRE_PAR")  && ( !isStartOfExpr() || args() ) && match("CIERRA_PAR") && match("END")){
					oop.hasDynamicAlloc = true;
					++oop.numNew;
					return true;
				}
			}
		}
		else if(l == "EXTRA" || l == "IDENTIFIER"){
			cout << "Entro VAR INIT DE EXTRA IDENTIFIER" << endl;
			if(match(l) && match("END")){
				return true;
			}
		}
		
	}
	else{
		return false;
	}
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

/*
bool declaracionObjeto(){
	if (declaracionObjeto1() || declaracionObjeto2()){
		return true;
	}
	else{
		return false;
	}
}
// Check options Checar ambiguedad 
bool declaracionObjeto1(){
	if (l == "TYPE"){
		if(match("TYPE") && match("IDENTIFIER") && match("IGUAL") && match("NEW") && match("TYPE") && match("ABRIR_PAR") && args() && match("CERRAR_PAR") && match("END")){
			return true;
		}
	}
	else{
		return false;
	}
}

bool declaracionObjeto2(){
	if ( l == "TYPE"){
		if(match("TYPE") && match("IDENTIFIER") && match("END")){
			return true;
		}
	}
	else{
		return false;
	}
}
	*/
bool cuerpoClase(){
	if (cuerpoClase2() || cuerpoClase1() ){
		return true;
	}
	else{
		return false;
	}
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
	cout <<"Aqui "<<endl;
	//lookahead de 1 en este caso... cuerpoClase }
	// EPSYLON HELP 
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
	if (l == "CIERRA_LLAVE"){
		return true;
	}
	else{
		return false;
	}
}



bool code(){
	if (code1() || code3() || code4() ||code7() ||code6() ||code2()){
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
	cout<< "ENTRO CODE3"<< endl;
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
	if (l == "CIERRA_LLAVE"|| l == "EOF" || l == "CLASS_KW"){
		return true;
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

/*
bool funcCode(){
	if (callStmt() ||deleteStmt() || funcCode4() || funcCode1() || funcCode2() || funcCode3()){
		return true;
	}
	else{
		return false;
	}
}
	*/

bool funcCode() {                        // Secuencia de sentencias
    if (endOfFunc()) return true;        // ε  ← sale sin recursión

    // Una sentencia obligatoria
    if (   returnStmt()
		|| callStmt()
        || deleteStmt()
        || funcCode4()      // var-sin-tipo
        || funcCode1()      // declaracionTipo
        || funcCode2() )    // EXTRA
    {
        return funcCode();              // ◄── recursión UNA sola vez
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
	cout << "Entro en CuerpoFuncion" << endl;
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
		cout << "Entro en contructor" << endl;
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

/*

bool miembro(){
	if (constructor() || destructor() || classFunc() || classVar()){
		return true;
	}
	else{
		return false;
	}
}
*/

bool miembro(){
	if (l == "VIRTUAL_KW"){
		match("VIRTUAL_KW");
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
}/*
bool definicion(){
	if (classDefinicion() || funcionDefinicion() || declaracionVarialbe() || declaracionObjeto()){
		return true;
	}
	else{
		return false;
	}
}

bool definicion(){
	if (classDefinicion() || funcionDefinicion()){
		return true;
	}
	else{
		return false;
	}
}
	*/


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

// Definition of E, as per the given production

/*
bool programa() {
    if (l == "ABRE_LLAVE") {
		
        if (match("ABRE_LLAVE") && definicion() && match("CIERRA_LLAVE")){
			return true;
		}
    }else{
		cout << " ERROR AQUI" << endl;
		error();
	}
}
	*/
bool programa() {
    if (l == "ABRE_LLAVE") {
		
        if (match("ABRE_LLAVE") && declaracion() && match("CIERRA_LLAVE")){
			return true;
		}
    }else{
		cout << " ERROR AQUI" << endl;
		error();
	}
}

int main() {
	handler();
	
	for (size_t i = 0; i < tokens.size(); ++i) {
        std::cout << tokens[i] << '\n';
    }
		
    do {
        l = tokens.front();
		tokens.erase(tokens.begin());
		cout << l << endl;
		// Programa is a start symbol.
	    programa();

    } while (l != "EOF");

    if (l == "EOF"){
		printf("Parsing Successful\n");
		cout<<endl;
		cout<<endl;
		cout << "\n--- Rasgos POO detectados ---\n";
		cout << "Clases:           " << oop.numClasses      << '\n';
		cout << "Herencia:         " << (oop.hasInheritance ? "sí" : "no") << '\n';
		cout << "Encapsulación:    " << (oop.hasEncapsulation ? "sí" : "no") << '\n';
		cout << "Polimorfismo:     " << (oop.hasVirt ? "sí" : "no") << '\n';
		cout << "new/delete usados: " << (oop.hasDynamicAlloc ? "sí" : "no") 
                              << "  (new="<<oop.numNew<<" delete="<<oop.numDelete<<")\n";
		cout << "obj->met() calls: " << (oop.hasArrowCalls ? "sí" : "no") << '\n';

		cout<<endl;
		cout<<endl;

		bool esOOP = oop.hasClass &&(oop.hasInheritance || oop.hasVirt || oop.hasEncapsulation);
		cout << "\n¿El fichero es POO?  " << (esOOP ? "✓ Sí" : "✗ No") << '\n';
		
	}
        
	return 0;
}