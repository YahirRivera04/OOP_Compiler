// ===================================================================
// ANALIZADOR LÉXICO (TOKENIZER)
// ===================================================================
// El analizador léxico convierte el código fuente (string) en una secuencia
// de tokens que el parser puede procesar. Cada token tiene un tipo y valor.

/**
 * Clase Token
 * Representa una unidad léxica del código fuente
 * @param {string} type - Tipo del token (KEYWORD, IDENTIFIER, SYMBOL, etc.)
 * @param {string} value - Valor literal del token
 * @param {number} line - Línea donde aparece el token
 * @param {number} column - Columna donde comienza el token
 */
class Token {
  constructor(type, value, line, column) {
    this.type = type;
    this.value = value;
    this.line = line;
    this.column = column;
  }
}

/**
 * Clase Lexer (Analizador Léxico)
 * Convierte el código fuente en tokens siguiendo las reglas léxicas de C++
 */
class Lexer {
  constructor(input) {
    this.input = input;        // Código fuente completo
    this.position = 0;         // Posición actual en el string
    this.line = 1;            // Línea actual (para reportar errores)
    this.column = 1;          // Columna actual
    this.tokens = [];         // Array donde se almacenarán los tokens
  }

  /**
   * Avanza el cursor una posición y actualiza línea/columna
   * Si encuentra un salto de línea, incrementa la línea y resetea la columna
   */
  advance() {
    if (this.position < this.input.length && this.input[this.position] === '\n') {
      this.line++;
      this.column = 1;
    } else {
      this.column++;
    }
    this.position++;
  }

  /**
   * Mira el carácter en la posición actual + offset sin consumirlo
   * @param {number} offset - Cuántos caracteres adelante mirar (default: 0)
   * @returns {string|null} El carácter o null si está fuera de rango
   */
  peek(offset = 0) {
    const pos = this.position + offset;
    return pos < this.input.length ? this.input[pos] : null;
  }

  /**
   * Salta todos los espacios en blanco (espacios, tabs, saltos de línea)
   * Los espacios no son significativos en C++ excepto para separar tokens
   */
  skipWhitespace() {
    while (this.position < this.input.length && /\s/.test(this.input[this.position])) {
      this.advance();
    }
  }

  /**
   * Salta comentarios de C++
   * - Comentarios de una línea: // hasta el final de línea
   * - Comentarios multilínea: /* hasta */
   * @returns {boolean} true si se saltó un comentario, false si no había comentario
   */
  skipComments() {
    // Comentario de una línea //
    if (this.peek() === '/' && this.peek(1) === '/') {
      // Avanzar hasta el final de la línea
      while (this.position < this.input.length && this.input[this.position] !== '\n') {
        this.advance();
      }
      return true;
    } 
    // Comentario multilínea /* */
    else if (this.peek() === '/' && this.peek(1) === '*') {
      this.advance(); // Saltar /
      this.advance(); // Saltar *
      
      // Buscar */ para cerrar el comentario
      while (this.position < this.input.length - 1) {
        if (this.input[this.position] === '*' && this.input[this.position + 1] === '/') {
          this.advance(); // Saltar *
          this.advance(); // Saltar /
          return true;
        }
        this.advance();
      }
      // Si llegamos aquí, el comentario no se cerró - error
      throw new Error(`Unclosed comment at line ${this.line}, column ${this.column}`);
    }
    return false;
  }

  /**
   * Lee un identificador o palabra clave
   * Los identificadores en C++ empiezan con letra o _ y pueden contener letras, números y _
   * Si el identificador coincide con una palabra reservada, se clasifica como KEYWORD
   * @returns {Token} Token de tipo IDENTIFIER o KEYWORD
   */
  readIdentifierOrKeyword() {
    const start = this.position;
    const startCol = this.column;
    
    // Leer mientras sea válido para un identificador
    while (this.position < this.input.length && /[a-zA-Z0-9_]/.test(this.input[this.position])) {
      this.advance();
    }
    
    const value = this.input.substring(start, this.position);
    
    // Lista de palabras reservadas de C++ relevantes para OOP
    const keywords = [
      'class', 'struct', 'public', 'private', 'protected', 
      'virtual', 'override', 'new', 'delete', 'int', 'float', 
      'double', 'char', 'bool', 'void', 'long', 'short', 
      'unsigned', 'signed', 'this', 'extends', 'implements'
    ];
    
    // Determinar si es keyword o identificador
    const type = keywords.includes(value) ? 'KEYWORD' : 'IDENTIFIER';
    return new Token(type, value, this.line, startCol);
  }

  /**
   * Lee un número (entero o flotante)
   * Formato: [0-9]+ o [0-9]+.[0-9]+
   * @returns {Token} Token de tipo NUMBER
   */
  readNumber() {
    const start = this.position;
    const startCol = this.column;
    
    // Leer parte entera
    while (this.position < this.input.length && /[0-9]/.test(this.input[this.position])) {
      this.advance();
    }
    
    // Verificar si hay parte decimal
    if (this.peek() === '.' && /[0-9]/.test(this.peek(1))) {
      this.advance(); // Saltar el punto
      // Leer parte decimal
      while (this.position < this.input.length && /[0-9]/.test(this.input[this.position])) {
        this.advance();
      }
    }
    
    const value = this.input.substring(start, this.position);
    return new Token('NUMBER', value, this.line, startCol);
  }

  /**
   * Lee una cadena de texto entre comillas simples o dobles
   * Maneja secuencias de escape como \"
   * @returns {Token} Token de tipo STRING
   */
  readString() {
    const start = this.position;
    const startCol = this.column;
    const quote = this.input[this.position]; // Guardar qué tipo de comilla (' o ")
    this.advance(); // Saltar comilla inicial
    
    // Leer hasta encontrar la comilla de cierre
    while (this.position < this.input.length && this.input[this.position] !== quote) {
      if (this.input[this.position] === '\\') {
        this.advance(); // Saltar carácter de escape
      }
      this.advance();
    }
    
    // Verificar que se cerró la cadena
    if (this.position >= this.input.length) {
      throw new Error(`Unterminated string at line ${this.line}, column ${startCol}`);
    }
    
    this.advance(); // Saltar comilla final
    const value = this.input.substring(start, this.position);
    return new Token('STRING', value, this.line, startCol);
  }

  /**
   * Método principal que convierte todo el código fuente en tokens
   * @returns {Token[]} Array de tokens terminado con un token EOF
   */
  tokenize() {
    // Procesar todo el input
    while (this.position < this.input.length) {
      // Saltar espacios en blanco
      this.skipWhitespace();
      if (this.position >= this.input.length) break;
      
      // Saltar comentarios y continuar si se encontró uno
      if (this.skipComments()) continue;
      
      const char = this.input[this.position];
      const col = this.column;
      
      // Identificadores y palabras clave (empiezan con letra o _)
      if (/[a-zA-Z_]/.test(char)) {
        this.tokens.push(this.readIdentifierOrKeyword());
      }
      // Números
      else if (/[0-9]/.test(char)) {
        this.tokens.push(this.readNumber());
      }
      // Cadenas de texto
      else if (char === '"' || char === "'") {
        this.tokens.push(this.readString());
      }
      // Operador flecha -> (dos caracteres)
      else if (char === '-' && this.peek(1) === '>') {
        this.tokens.push(new Token('ARROW', '->', this.line, col));
        this.advance();
        this.advance();
      }
      // Símbolos de un carácter
      else if ('{}();:,~*.=<>!+-/[]'.includes(char)) {
        this.tokens.push(new Token('SYMBOL', char, this.line, col));
        this.advance();
      }
      // Carácter no reconocido - error
      else {
        throw new Error(`Unexpected character '${char}' at line ${this.line}, column ${this.column}`);
      }
    }
    
    // Agregar token EOF para marcar el final
    this.tokens.push(new Token('EOF', 'EOF', this.line, this.column));
    return this.tokens;
  }
}

// ===================================================================
// ANALIZADOR SINTÁCTICO (PARSER) - Con Gramática Formal
// ===================================================================
// El parser toma los tokens del lexer y verifica que sigan la gramática
// de C++, identificando características de programación orientada a objetos

/**
 * Clase Parser (Analizador Sintáctico)
 * Implementa un parser descendente predictivo para analizar código C++
 * y detectar características de programación orientada a objetos
 */
class Parser {
  constructor(tokens) {
    this.tokens = tokens;           // Array de tokens del lexer
    this.currentToken = 0;          // Índice del token actual
    this.symbolTable = new Map();   // Tabla de símbolos para guardar clases, métodos, etc.
    
    // Flags para detección de características OOP
    this.hasClass = false;          // ¿Se definieron clases?
    this.hasInheritance = false;    // ¿Hay herencia entre clases?
    this.hasPolymorphism = false;   // ¿Hay métodos virtuales/override?
    this.hasEncapsulation = false;  // ¿Se usan modificadores de acceso?
    this.hasNew = false;            // ¿Se usa allocación dinámica con new?
    this.hasMethods = false;        // ¿Las clases tienen métodos?
    this.hasConstructor = false;    // ¿Hay constructores definidos?
    this.hasDestructor = false;     // ¿Hay destructores definidos?
    
    // Estado del parser
    this.contextStack = [];         // Pila de contextos (para saber si estamos en una clase)
    this.currentClass = null;       // Nombre de la clase actual siendo parseada
  }

  /**
   * Obtiene el token en la posición actual + offset sin consumirlo
   * @param {number} offset - Cuántos tokens adelante mirar
   * @returns {Token|null} El token o null si no existe
   */
  peek(offset = 0) {
    const pos = this.currentToken + offset;
    return pos < this.tokens.length ? this.tokens[pos] : null;
  }

  /**
   * Verifica si el token actual es del tipo y valor esperados
   * @param {string} type - Tipo de token esperado
   * @param {string} value - Valor específico (opcional)
   * @returns {boolean} true si coincide
   */
  check(type, value = null) {
    const token = this.peek();
    if (!token) return false;
    return token.type === type && (value === null || token.value === value);
  }

  /**
   * Consume el token actual si coincide con lo esperado
   * @param {string} type - Tipo de token esperado
   * @param {string} value - Valor específico (opcional)
   * @returns {Token|null} El token consumido o null
   */
  consume(type, value = null) {
    if (this.check(type, value)) {
      const token = this.tokens[this.currentToken];
      this.currentToken++;
      return token;
    }
    return null;
  }

  /**
   * Espera un token específico y lanza error si no se encuentra
   * @param {string} type - Tipo de token esperado
   * @param {string} value - Valor específico (opcional)
   * @returns {Token} El token consumido
   * @throws {Error} Si el token no coincide
   */
  expect(type, value = null) {
    const token = this.consume(type, value);
    if (!token) {
      const current = this.peek();
      throw new Error(
        `Expected ${type}${value ? ` '${value}'` : ''} but found ${
          current ? `${current.type} '${current.value}'` : 'EOF'
        } at line ${current ? current.line : 'EOF'}`
      );
    }
    return token;
  }

  // ===== PRODUCCIONES DE LA GRAMÁTICA =====
  // Cada método parse* corresponde a una regla de la gramática

  /**
   * Punto de entrada del parser
   * Programa ::= { Declaracion }
   * Procesa todas las declaraciones hasta encontrar EOF
   * @returns {Object} Resultados del análisis con flags OOP y posibles errores
   */
  parse() {
    try {
      while (!this.check('EOF')) {
        this.parseDeclaration();
      }
      return this.getAnalysisResults();
    } catch (error) {
      return {
        error: error.message,
        ...this.getAnalysisResults()
      };
    }
  }

  /**
   * Parsea una declaración de nivel superior
   * Declaracion ::= ClaseDefinicion | FuncionDefinicion | DeclaracionVariable | DeclaracionObjeto
   * Determina qué tipo de declaración es y llama al método apropiado
   */
  parseDeclaration() {
    // Verificar si es una definición de clase
    if (this.check('KEYWORD', 'class') || this.check('KEYWORD', 'struct')) {
      this.parseClassDefinition();
    }
    // Verificar si es una función o variable (empiezan con tipo)
    else if (this.isTypeSpecifier()) {
      const savedPos = this.currentToken; // Guardar posición por si necesitamos retroceder
      this.parseType();
      
      if (this.check('IDENTIFIER')) {
        const name = this.consume('IDENTIFIER');
        
        // Si hay paréntesis después del identificador, es una función
        if (this.check('SYMBOL', '(')) {
          this.parseFunctionDefinition(name);
        }
        // Si no, es una declaración de variable/objeto
        else {
          this.currentToken = savedPos; // Retroceder para re-parsear
          this.parseVariableOrObjectDeclaration();
        }
      } else {
        this.currentToken = savedPos;
        this.parseVariableOrObjectDeclaration();
      }
    }
    // Token no reconocido
    else {
      throw new Error(`Unexpected token '${this.peek().value}' at line ${this.peek().line}`);
    }
  }

  /**
   * Parsea una definición de clase
   * ClaseDefinicion ::= ("class" | "struct") IDENTIFIER [Herencia] "{" CuerpoClase "}" [";"]
   * Detecta: clases, herencia, métodos, constructores, destructores
   */
  parseClassDefinition() {
    const classKeyword = this.consume('KEYWORD'); // 'class' o 'struct'
    const className = this.expect('IDENTIFIER');   // Nombre de la clase
    
    // Marcar que encontramos una clase
    this.hasClass = true;
    this.currentClass = className.value;
    this.contextStack.push('class');
    
    // Agregar clase a la tabla de símbolos
    this.symbolTable.set(className.value, {
      type: 'class',
      name: className.value,
      line: className.line,
      members: new Map()
    });
    
    // Verificar si hay herencia (empieza con :)
    if (this.check('SYMBOL', ':')) {
      this.parseInheritance();
    }
    
    this.expect('SYMBOL', '{');    // Inicio del cuerpo de la clase
    this.parseClassBody();          // Parsear miembros de la clase
    this.expect('SYMBOL', '}');    // Fin del cuerpo
    this.consume('SYMBOL', ';');   // ; opcional después de la clase
    
    // Restaurar contexto
    this.contextStack.pop();
    this.currentClass = null;
  }

  /**
   * Parsea la herencia de una clase
   * Herencia ::= ":" [EspecificadorAcceso] IDENTIFIER { "," [EspecificadorAcceso] IDENTIFIER }
   * Soporta herencia múltiple de C++
   */
  parseInheritance() {
    this.expect('SYMBOL', ':');
    this.hasInheritance = true;  // Marcar que hay herencia
    
    // Parsear lista de clases base
    do {
      // Especificador de acceso opcional (public, private, protected)
      if (this.check('KEYWORD', 'public') || 
          this.check('KEYWORD', 'private') || 
          this.check('KEYWORD', 'protected')) {
        this.consume('KEYWORD');
        this.hasEncapsulation = true;  // Los especificadores indican encapsulación
      }
      
      const baseClass = this.expect('IDENTIFIER'); // Nombre de la clase base
      // Aquí podríamos registrar la relación de herencia en la tabla de símbolos
    } while (this.consume('SYMBOL', ',')); // Continuar si hay más clases base
  }

  /**
   * Parsea el cuerpo de una clase
   * CuerpoClase ::= { MiembroClase }
   * Procesa todos los miembros hasta encontrar }
   */
  parseClassBody() {
    while (!this.check('SYMBOL', '}')) {
      // Especificadores de acceso (cambian el nivel de acceso para los siguientes miembros)
      if (this.check('KEYWORD', 'public') || 
          this.check('KEYWORD', 'private') || 
          this.check('KEYWORD', 'protected')) {
        const access = this.consume('KEYWORD');
        this.hasEncapsulation = true;  // Uso de encapsulación
        this.expect('SYMBOL', ':');    // Los especificadores van seguidos de :
        continue;
      }
      
      // Palabra clave 'virtual' antes de un método
      if (this.check('KEYWORD', 'virtual')) {
        this.consume('KEYWORD');
        this.hasPolymorphism = true;   // Virtual indica polimorfismo
      }
      
      // Verificar si es un destructor (~ClassName)
      if (this.check('SYMBOL', '~')) {
        this.parseDestructor();
      } 
      // Verificar si es un constructor (mismo nombre que la clase)
      else if (this.check('IDENTIFIER') && this.peek().value === this.currentClass) {
        this.parseConstructor();
      } 
      // Es un miembro normal (método o atributo)
      else if (this.isTypeSpecifier() || this.check('IDENTIFIER')) {
        this.parseMemberDeclaration();
      } 
      // Token no esperado en el cuerpo de la clase
      else {
        throw new Error(`Unexpected token in class body: '${this.peek().value}'`);
      }
    }
  }

  /**
   * Parsea un constructor
   * Constructor ::= IDENTIFIER "(" [Parametros] ")" [":"InitList] CuerpoFuncion
   * Los constructores tienen el mismo nombre que la clase
   */
  parseConstructor() {
    const name = this.expect('IDENTIFIER'); // Nombre (debe ser igual a la clase)
    this.hasConstructor = true;
    this.hasMethods = true;
    
    this.expect('SYMBOL', '(');
    this.parseParameterList();      // Parámetros del constructor
    this.expect('SYMBOL', ')');
    
    // Lista de inicialización opcional (: member(value), ...)
    if (this.check('SYMBOL', ':')) {
      this.parseInitializationList();
    }
    
    this.parseFunctionBody();       // Cuerpo del constructor
  }

  /**
   * Parsea un destructor
   * Destructor ::= "~" IDENTIFIER "(" ")" CuerpoFuncion
   * Los destructores empiezan con ~ y no tienen parámetros
   */
  parseDestructor() {
    this.expect('SYMBOL', '~');
    const name = this.expect('IDENTIFIER'); // Nombre (debe ser igual a la clase)
    this.hasDestructor = true;
    this.hasMethods = true;
    
    this.expect('SYMBOL', '(');
    this.expect('SYMBOL', ')');    // Los destructores no tienen parámetros
    this.parseFunctionBody();       // Cuerpo del destructor
  }

  /**
   * Parsea una declaración de miembro (método o atributo)
   * MiembroDeclaracion ::= Tipo IDENTIFIER ["(" [Parametros] ")"] ["override"] ["const"] ["=" "0"] (";" | CuerpoFuncion)
   * Distingue entre métodos y atributos basándose en la presencia de paréntesis
   */
  parseMemberDeclaration() {
    const type = this.parseType();              // Tipo de retorno o tipo del atributo
    const name = this.expect('IDENTIFIER');     // Nombre del método o atributo
    
    // Si hay paréntesis, es un método
    if (this.check('SYMBOL', '(')) {
      this.hasMethods = true;
      this.expect('SYMBOL', '(');
      this.parseParameterList();    // Parámetros del método
      this.expect('SYMBOL', ')');
      
      // Palabra clave 'override' (indica que sobrescribe un método virtual)
      if (this.check('KEYWORD', 'override')) {
        this.consume('KEYWORD');
        this.hasPolymorphism = true;
      }
      
      // Métodos const (no modifican el estado del objeto)
      if (this.check('KEYWORD', 'const')) {
        this.consume('KEYWORD');
      }
      
      // Método virtual puro (= 0)
      if (this.check('SYMBOL', '=') && this.peek(1) && this.peek(1).value === '0') {
        this.consume('SYMBOL', '=');
        this.consume('NUMBER');
        this.hasPolymorphism = true;  // Los métodos virtuales puros indican polimorfismo
      }
      
      // El método puede tener cuerpo o terminar con ;
      if (this.check('SYMBOL', '{')) {
        this.parseFunctionBody();
      } else {
        this.expect('SYMBOL', ';');
      }
    }
    // Si no hay paréntesis, es un atributo
    else {
      this.expect('SYMBOL', ';');
    }
  }

  /**
   * Parsea una declaración de variable o creación de objeto
   * DeclaracionObjeto ::= Tipo IDENTIFIER ["=" "new" Tipo ["(" [Argumentos] ")"]] ";"
   * Detecta el uso del operador new para creación dinámica
   */
  parseVariableOrObjectDeclaration() {
    const type = this.parseType();              // Tipo de la variable
    const name = this.expect('IDENTIFIER');     // Nombre de la variable
    
    // Verificar si hay inicialización
    if (this.check('SYMBOL', '=')) {
      this.consume('SYMBOL', '=');
      
      // Verificar si usa 'new' (creación dinámica de objetos)
      if (this.check('KEYWORD', 'new')) {
        this.consume('KEYWORD');
        this.hasNew = true;         // Marcar uso de new
        
        const objectType = this.parseType();    // Tipo del objeto a crear
        
        // Argumentos del constructor (opcional)
        if (this.check('SYMBOL', '(')) {
          this.expect('SYMBOL', '(');
          this.parseArgumentList();
          this.expect('SYMBOL', ')');
        }
      } else {
        // Expresión de inicialización normal
        this.parseExpression();
      }
    }
    
    this.expect('SYMBOL', ';');
  }

  /**
   * Parsea una definición de función (no miembro de clase)
   * FuncionDefinicion ::= Tipo IDENTIFIER "(" [Parametros] ")" CuerpoFuncion
   * Las funciones libres indican programación procedural
   */
  parseFunctionDefinition(name) {
    this.expect('SYMBOL', '(');
    this.parseParameterList();
    this.expect('SYMBOL', ')');
    this.parseFunctionBody();
  }

  /**
   * Parsea un tipo de dato
   * Tipo ::= TipoBasico {"*"} | IDENTIFIER {"*"}
   * Soporta tipos básicos de C++ y punteros (múltiples *)
   * @returns {string} Representación del tipo como string
   */
  parseType() {
    let type = '';
    
    // Verificar si es un tipo básico o un identificador (clase definida por usuario)
    if (this.isTypeSpecifier()) {
      type = this.consume('KEYWORD').value;
    } else if (this.check('IDENTIFIER')) {
      type = this.consume('IDENTIFIER').value;
    } else {
      throw new Error(`Expected type specifier at line ${this.peek().line}`);
    }
    
    // Agregar punteros (*) si existen
    while (this.check('SYMBOL', '*')) {
      this.consume('SYMBOL', '*');
      type += '*';
    }
    
    return type;
  }

  /**
   * Verifica si el token actual es un especificador de tipo básico
   * @returns {boolean} true si es un tipo básico de C++
   */
  isTypeSpecifier() {
    const typeKeywords = [
      'int', 'float', 'double', 'char', 'bool', 'void',
      'long', 'short', 'unsigned', 'signed'
    ];
    return this.check('KEYWORD') && typeKeywords.includes(this.peek().value);
  }

  /**
   * Parsea una lista de parámetros de función
   * ParameterList ::= Tipo [IDENTIFIER] { "," Tipo [IDENTIFIER] }
   * Los nombres de parámetros son opcionales en declaraciones
   */
  parseParameterList() {
    if (!this.check('SYMBOL', ')')) {
      do {
        this.parseType();               // Tipo del parámetro
        this.consume('IDENTIFIER');     // Nombre opcional
      } while (this.consume('SYMBOL', ','));
    }
  }

  /**
   * Parsea una lista de argumentos en una llamada
   * ArgumentList ::= Expression { "," Expression }
   */
  parseArgumentList() {
    if (!this.check('SYMBOL', ')')) {
      do {
        this.parseExpression();
      } while (this.consume('SYMBOL', ','));
    }
  }

  /**
   * Parsea una lista de inicialización de constructor
   * InitList ::= IDENTIFIER "(" ArgumentList ")" { "," IDENTIFIER "(" ArgumentList ")" }
   * Usado para inicializar miembros en el constructor
   */
  parseInitializationList() {
    this.expect('SYMBOL', ':');
    do {
      this.expect('IDENTIFIER');        // Nombre del miembro
      this.expect('SYMBOL', '(');
      this.parseArgumentList();         // Valores de inicialización
      this.expect('SYMBOL', ')');
    } while (this.consume('SYMBOL', ','));
  }

  /**
   * Parsea el cuerpo de una función
   * CuerpoFuncion ::= "{" { Statement } "}"
   * Usa balance de llaves para saltar el contenido sin analizarlo en detalle
   */
  parseFunctionBody() {
    this.expect('SYMBOL', '{');
    let depth = 1;  // Contador de profundidad de llaves
    
    // Continuar hasta que se cierren todas las llaves
    while (depth > 0 && !this.check('EOF')) {
      if (this.check('SYMBOL', '{')) {
        this.consume('SYMBOL', '{');
        depth++;
      } else if (this.check('SYMBOL', '}')) {
        this.consume('SYMBOL', '}');
        depth--;
      } else {
        this.currentToken++;  // Avanzar token por token
      }
    }
  }

  /**
   * Parsea una expresión de forma simplificada
   * Expression ::= cualquier secuencia válida hasta ; , o )
   * No construimos un árbol de expresiones, solo saltamos tokens
   */
  parseExpression() {
    let parenDepth = 0;  // Balance de paréntesis
    
    while (!this.check('EOF')) {
      // Incrementar profundidad en paréntesis abierto
      if (this.check('SYMBOL', '(')) {
        parenDepth++;
      } 
      // Decrementar en paréntesis cerrado
      else if (this.check('SYMBOL', ')')) {
        if (parenDepth === 0) break;  // Fin de la expresión
        parenDepth--;
      } 
      // Fin de expresión si encontramos ; o , al nivel superior
      else if (this.check('SYMBOL', ';') || this.check('SYMBOL', ',')) {
        if (parenDepth === 0) break;
      }
      this.currentToken++;
    }
  }

  /**
   * Recopila los resultados del análisis
   * @returns {Object} Objeto con todas las flags OOP y la tabla de símbolos
   */
  getAnalysisResults() {
    return {
      hasClass: this.hasClass,
      hasInheritance: this.hasInheritance,
      hasPolymorphism: this.hasPolymorphism,
      hasEncapsulation: this.hasEncapsulation,
      hasNew: this.hasNew,
      hasMethods: this.hasMethods,
      hasConstructor: this.hasConstructor,
      hasDestructor: this.hasDestructor,
      symbolTable: Array.from(this.symbolTable.entries())
    };
  }
}

// ===================================================================
// FUNCIONES PRINCIPALES
// ===================================================================

/**
 * Función principal que analiza código C++ y detecta características OOP
 * @param {string} code - Código fuente C++ a analizar
 * @returns {Object} Objeto con las características OOP detectadas o error
 */
function analyzeOOP(code) {
  try {
    // Paso 1: Análisis léxico - convertir código en tokens
    const lexer = new Lexer(code);
    const tokens = lexer.tokenize();
    
    // Paso 2: Análisis sintáctico - parsear tokens según gramática
    const parser = new Parser(tokens);
    return parser.parse();
  } catch (error) {
    // Si hay error en cualquier fase, retornar objeto con error
    return {
      error: error.message,
      hasClass: false,
      hasInheritance: false,
      hasPolymorphism: false,
      hasEncapsulation: false,
      hasNew: false,
      hasMethods: false,
      hasConstructor: false,
      hasDestructor: false
    };
  }
}

/**
 * Función simplificada que determina si el código es OOP
 * @param {string} code - Código fuente C++ a analizar
 * @returns {boolean} true si tiene características OOP, false si no
 */
function isOOP(code) {
  const results = analyzeOOP(code);
  // Es OOP si no hay error y tiene al menos una característica OOP
  return !results.error && (
    results.hasClass ||
    results.hasInheritance ||
    results.hasPolymorphism ||
    results.hasEncapsulation ||
    results.hasNew ||
    results.hasMethods
  );
}

/**
 * Función que clasifica el paradigma de programación del código
 * @param {string} code - Código fuente C++ a analizar
 * @returns {Object} Objeto con el paradigma detectado y detalles
 * 
 * Criterios de clasificación:
 * - OBJECT_ORIENTED: 3 o más características OOP
 * - HYBRID: 1-2 características OOP
 * - PROCEDURAL: 0 características OOP
 */
function classifyParadigm(code) {
  const results = analyzeOOP(code);
  
  // Si hay error, retornar paradigma ERROR
  if (results.error) {
    return { paradigm: 'ERROR', details: results.error };
  }
  
  // Contar cuántas características OOP tiene
  const oopFeatures = [
    results.hasClass,
    results.hasInheritance,
    results.hasPolymorphism,
    results.hasEncapsulation,
    results.hasNew,
    results.hasMethods
  ].filter(Boolean).length;  // filter(Boolean) cuenta solo los true
  
  // Clasificar según el número de características
  if (oopFeatures >= 3) {
    return { paradigm: 'OBJECT_ORIENTED', details: results };
  } else if (oopFeatures >= 1) {
    return { paradigm: 'HYBRID', details: results };
  } else {
    return { paradigm: 'PROCEDURAL', details: results };
  }
}

// ===================================================================
// EXPORTACIÓN Y PRUEBAS
// ===================================================================

// Exportar funciones para usar como módulo
module.exports = { analyzeOOP, isOOP, classifyParadigm, Lexer, Parser };

// Punto de entrada cuando se ejecuta directamente
if (require.main === module) {
  const fs = require('fs');
  const filename = process.argv[2];
  
  // Verificar que se proporcionó un archivo
  if (!filename) {
    console.error('Usage: node oop_detector.js <filename.cpp>');
    process.exit(1);
  }
  
  try {
    // Leer el archivo
    const code = fs.readFileSync(filename, 'utf8');
    
    // Clasificar el paradigma
    const classification = classifyParadigm(code);
    
    // Mostrar resultados
    console.log('=== ANÁLISIS DE PARADIGMA ===');
    console.log(`Paradigma detectado: ${classification.paradigm}`);
    console.log('\n=== CARACTERÍSTICAS OOP DETECTADAS ===');
    console.log(JSON.stringify(classification.details, null, 2));
    
    // Mostrar tokens si se pide con --tokens
    if (process.argv.includes('--tokens')) {
      const lexer = new Lexer(code);
      const tokens = lexer.tokenize();
      console.log('\n=== TOKENS ===');
      tokens.forEach(token => {
        console.log(`${token.type.padEnd(12)} ${token.value.padEnd(20)} [${token.line}:${token.column}]`);
      });
    }
  } catch (error) {
    console.error('Error:', error.message);
    process.exit(1);
  }
}