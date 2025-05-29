



GRAMATICA

Programa { Declaracion  } EOF

Declaracion —> Definicion Declaracion | Epsylon 
Definicion —> ClaseDefinicion | DeclaracionTipo | code

ClaseDefinicion —> CLASS_KW IDENTIFIER [ Herencia ] ABRE_LLAVE CuerpoClase CIERRA_LLAVE END

Herencia —> DOS_PUNTOS ListaHerencia 
ListaHerencia —> HeredaUno { COMA HeredaUno } 
HeredaUno —> ESPECIFICADOR_ACCESO IDENTIFIER

FuncionDefinicion —> TYPE IDENTIFIER ABRE_PAR [ Parametros ] CIERRA_PAR Cuerpofuncion

DeclaracionTipo —> TYPE IDENTIFIER DeclaracionTipoAlpha


DeclaracionTipoAlpha —> ABRE_PAR [ Parametros ] CIERRA_PAR DeclaracionPostPar | DeclaracionVarInit 


DeclaracionPostPar —> CuerpoFuncion | END

DeclaracionVarInit —> IGUAL NEW TYPE ABRE_PAR [Args] CIERRA_PAR END | END | IGUAL (EXTRA | IDENTIFIER) END


	
FuncCode --> ReturnStmt
            | CallStmt
            | DeleteStmt
            | DeclaracionVarSinTipo
            | DeclaracionTipo
            | EXTRA 

Cuerpoclase —> EspAcc CuerpoClase | Epsylon

EspAcc —> ESPECIFICADOR_ACCESO DOS_PUNTOS MetVar | MetVar

MetVar —>  Miembro MetVar | Epsylon 

Miembro —> Constructor | Destructor | MiembroTipo


Constructor —> IDENTIFIER ABRE_PAR [Parametros] CIERRA_PAR [ DOS_PUNTOS InitList ] CuerpoFuncion

Destructor       → SQUIGLY IDENTIFIER ABRE_PAR CIERRA_PAR CuerpoFuncion

MiembroTipo —> DeclaracionTipo

CuerpoFuncion —> ABRE_LLAVE FuncCode CIERRA_LLAVE 
declaracionVarSinTipo —>  IDENTIFIER DelcaracionTipoAlpha



code —> EXTRA code
		| FuncionDefinicion code
		| declaracionTipo code
		| declaracionVarSinTipo code
		| Epsylon

Parametros —> Param { COMA Param } 
Param —> TIPO IDENTIFIER 

Args —> Expr { COMA Expr } 

Expr —> EXTRA | IDENTIFIER | NUMBER


CallStmt --> IDENTIFIER ARROW IDENTIFIER ABRE_PAR  [ Args ]  CIERRA_PAR  END
InitList —> IDENTIFIER ( ABRE_PAR Args CIERRA_PAR)
DeleteStmt —> DELETE IDENTIFIER END
returnStmt → RETURN_KW ( NUMBER | IDENTIFIER ) END
