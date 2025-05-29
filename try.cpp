#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include <string>
#include <vector>
#include <iostream>


using namespace std;
vector<string> tokens = {"ABRE_LLAVE","CLASS_KW","IDENTIFIER","ABRE_LLAVE","ESPECIFICADOR_ACCESO","DOS_PUNTOS","TYPE","IDENTIFIER","END",
	"IDENTIFIER", "ABRE_PAR", "TYPE","IDENTIFIER", "CIERRA_PAR","ABRE_LLAVE","IDENTIFIER","IGUAL","IDENTIFIER","END", "CIERRA_LLAVE","CIERRA_LLAVE",
	"END","CIERRA_LLAVE", "EOF"};

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

Programa --> { Declaracion } -  ABRE_PAR Declaracion CIERRA_PAR
Declaracion --> Definicion declaracion | Epsylon
Definicion --> ClaseDefinicion | FuncionDefinicion | DeclaracionVariable | DeclaracionObjeto 

DeclaracionObjeto --> Tipo IDENTIFIER [IGUAL NEW Tipo [ABRE_PAR [Argumentos] CIERRA_PAR ]] END

MiembroDeclaracion --> Tipo IDENTIFIER [ABRE_PAR [Parametros] CIERRA_PAR] [OVERRIDE] [CONST] [IGUAL ZERO] (END | CuerpoFuncion)

FuncionDefinicion --> Tipo IDENTIFIER ABRE_PAR [Parametros] CIERRA_PAR CuerpoFuncion

ClaseDefinicion --> CLASS_KW IDENTIFIER [Herencia] ABRE_LLAVE CuerpoClase CIERRA_LLAVE END

	Herencia --> DOS_PUNTOS [EspecificadorAcceso] IDENTIFIER { COMA [EspecificadorAcceso] IDENTIFIER}

	Constructor --> IDENTIFIER ABRE_PAR [Parametros] CIERRA_PAR [DOS_PUNTOS InitList] CuerpoFuncion

	Destructor --> SQUIRLY IDENTIFIER ABRE_PAR CIERRA_PAR CuerpoFuncion



CuerpoClase --> EspAcc CuerpoClase | Epslyon

EspAcc --> ESPECIFICADOR_ACCESO DOS_PUNTOS MetVar | MetVar

MetVar --> Miembro MetVar | Epsylon

Miembro --> Constructor | Destructor | ClassFunc  | ClassVar

CuerpoFuncion --> ABRE_LLAVE Code CIERRA_LLAVE

Code --> EXTRA Code | Epsylon

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
bool code();
bool code1();
bool code2();
bool code3();
bool code4();
bool code5();
bool args();
bool isStartOfExpr();
bool expr();

bool funcionDefinicion();



bool declaracion();
bool declaracion1();
bool declaracion2();

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


inline bool isStartOfExpr() { return l == "EXTRA"; }

bool expr() {   
	if(l == "EXTRA"){
		if(match("EXTRA")){
			return true;
		}
	}else{
		return false;
	}
}

bool args() {
    if (!expr())                      // primer Expr obligatorio
        return false;

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
	if(l == "IDENTIFIER"){
		if(match("IDENTIFIER") && match("IGUAL") && match("IDENTIFIER") && match("END")){
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
		if(match("IGUAL") && match("NEW") && match("TYPE ") && match("ABRE_PAR")  && ( !isStartOfExpr() || args() ) && match("CIERRA_PAR") && match("END")){
			return true;
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
	if (code1() || code3() || code4() || code5() ||code2()){
		return true;
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

bool code5(){
	if(declaracionVarSinTipo() && code()){
		return true;
	}
	else{
		return false;
	}
}
bool code2(){
	if (l == "CIERRA_LLAVE"){
		return true;
	}
	else{
		return false;
	}
}
	

bool cuerpoFuncion(){
	if(l == "ABRE_LLAVE"){
		if (match("ABRE_LLAVE") && code() && match("CIERRA_LLAVE")){
			return true;
		}
	}
	else{
		return false;
	}
}

bool constructor(){
	if (l == "IDENTIFIER"){
		if(match("IDENTIFIER") && match("ABRE_PAR") && ( !isStartOfParametro() || parametros() ) && match("CIERRA_PAR") && cuerpoFuncion()){
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
	if (l == "TIPO"){
		if(match("TIPO") && match("IDENTIFIER") && match("ABRE_PAR") && ( !isStartOfParametro() || parametros() ) && match("CIERRA_PAR") && cuerpoFuncion()){
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
			return true;
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

bool definicion(){
	if(classDefinicion() || declaracionTipo()){
		return true;
	}
	else{
		return false;
	}
}
/*
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
	*/

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

int main() {
	
    do {
        l = tokens.front();
		tokens.erase(tokens.begin());
		cout << l << endl;
		// E is a start symbol.
	    programa();

    } while (l != "EOF");

    if (l == "EOF")
        printf("Parsing Successful\n");
}