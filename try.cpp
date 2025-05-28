#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include <string>
#include <vector>
#include <iostream>


using namespace std;
vector<string> tokens = {"ABRE_LLAVE", "CLASS_KW", "IDENTIFIER", "ABRE_PAR","CUERPO_CLASE","CIERRA_PAR",";","CIERRA_LLAVE","EOF"};

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
Declaracion -- Definicion declaracion  Epsylon
Definicion --> ClaseDefinicion | FuncionDefinicion | DeclaracionVariable | DeclaracionObjeto | ELSE ? 

DeclaracionObjeto --> Tipo IDENTIFIER [IGUAL NEW Tipo [ABRE_PAR [Argumentos] CIERRA_PAR ]] END

MiembroDeclaracion --> Tipo IDENTIFIER [ABRE_PAR [Parametros] CIERRA_PAR] [OVERRIDE] [CONST] [IGUAL ZERO] (END | CuerpoFuncion)

FuncionDefinicion --> Tipo IDENTIFIER ABRE_PAR [Parametros] CIERRA_PAR CuerpoFuncion

ClaseDefinicion --> CLASS_KW IDENTIFIER [Herencia] ABRE_LLAVE CuerpoClase CIERRA_LLAVE END

	Herencia --> DOS_PUNTOS [EspecificadorAcceso] IDENTIFIER { COMA [EspecificadorAcceso] IDENTIFIER}

	CuerpoClase --> ABRE_LLAVE MiembroClase CIERRA_LLAVE

	Constructor --> IDENTIFIER ABRE_PAR [Parametros] CIERRA_PAR [DOS_PUNTOS InitList] CuerpoFuncion

	Destructor --> SQUIRLY IDENTIFIER ABRE_PAR CIERRA_PAR CuerpoFuncion



MiembroClase -- EspAcc MiembroClase Epslyon

EspAcc -- ESPECIFICADOR_ACCESO DOS_PUNTOS MetVar MetVar

MetVar -- Miembro MetVar Epsylon


Miembro -- Constructor 	Destructor ClassFunc ClassVar

CuerpoFuncion -- ABRE_LLAVE Code CIERRA_LLAVE

Code -- EXTRA Code Epsylon

***************************/

string l;

bool programa();
bool declaracion();

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

bool cuerpoClase(){
	if (l == "ABRE_LLAVE"){
		if(match("ABRE_LLAVE") && miembroClase() && match("CIERRA_LLAVE")){
			return true;
		}
	}else{
		return false;
	}
}

bool miembroClase(){
	if (miembroClase1() || miembroClase2()){
		return true;
	}
	else{
		return false;
	}
}
bool miembroClase1(){
	if(EspAcc() && miembroClase()){
			return true;
	}
	else{
		return false;
	}
}

bool miembroClase2(){
	// EPSYLON HELP 

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
		if(match("ESPECIFICADOR_ACCESO") && match("DOS_PUNTOS") && metvar())
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
	// EPSYLON !!
}

bool constructor(){
	if (l == "IDENTIFIER"){
		if(match("IDENTIFIER") && match("ABRE_PAR") && match("CIERRA_PAR") && cuerpoFuncion()){
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
		if(match("TIPO") && match("IDENTIFIER") && match("ABRE_PAR") && match("CIERRA_PAR") && cuerpoFuncion()){
			return true;
		}
		else{
			return false;
		}
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
bool code(){
	if (code1() || code2()){
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
bool code2(){
	// EPSYLON HELP !
}

bool miembro(){
	if (constructor() || destructor() || classFunc() || classVar()){
		return true;
	}
	else{
		return false;
	}
}

bool classDefinicion() {
	
    if (l == "CLASS_KW") {
		if(match("CLASS_KW") && match("IDENTIFIER") && match("ABRE_PAR") && cuerpoClase() && match("CIERRA_PAR") && match("END"))
			return true;
    }else{
		return false;
	}
}
bool definicion(){
	if (classDefinicion() || funcionDefinicion() || declaracionVarialbe() || declaracionObjeto()){
		return true;
	}
	else{
		return false;
	}
}

bool declaracion(){
	if (declaracion1() || delcaracion2()){
		return true;
	}else{
		return false;
	}
}

bool delcaracion1(){
	if(definicion() && declaracion()){
		return true;
	}
	else{
		return false;
	}
}

bool declaracion2(){
	// EPSYLON
}

// Definition of E, as per the given production
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