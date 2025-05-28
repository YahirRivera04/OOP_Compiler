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
Declaracion --> ClaseDefinicion | FuncionDefinicion | DeclaracionVariable | DeclaracionObjeto 

DeclaracionObjeto --> Tipo IDENTIFIER [IGUAL NEW Tipo [ABRE_PAR [Argumentos] CIERRA_PAR ]] END

MiembroDeclaracion --> Tipo IDENTIFIER [ABRE_PAR [Parametros] CIERRA_PAR] [OVERRIDE] [CONST] [IGUAL ZERO] (END | CuerpoFuncion)

FuncionDefinicion --> Tipo IDENTIFIER ABRE_PAR [Parametros] CIERRA_PAR CuerpoFuncion

ClaseDefinicion --> CLASS_KW IDENTIFIER [Herencia] ABRE_LLAVE CuerpoClase CIERRA_LLAVE END

	Herencia --> DOS_PUNTOS [EspecificadorAcceso] IDENTIFIER { COMA [EspecificadorAcceso] IDENTIFIER}

	CuerpoClase --> ABRE_LLAVE MiembroClase CIERRA_LLAVE

	Constructor --> IDENTIFIER ABRE_PAR [Parametros] CIERRA_PAR [DOS_PUNTOS InitList] CuerpoFuncion

	Destructor --> SQUIRLY IDENTIFIER ABRE_PAR CIERRA_PAR CuerpoFuncion


E --> i E'
E' --> + i E' | e



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
		if(match("ABRE_LLAVE") && miembroClase && match("CIERRA_LLAVE")){
			return true;
		}
	}else{
		return false;
	}
}


bool declaracion() {
	
    if (l == "CLASS_KW") {
		if(match("CLASS_KW") && match("IDENTIFIER") && match("ABRE_PAR") && match("CUERPO_CLASE") && match("CIERRA_PAR") && match(";"))
			return true;
    }else{
		return false;
	}
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