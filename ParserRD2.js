// ===================================================================
// ANALIZADOR LÉXICO (TOKENIZER)
// ===================================================================

class Token {
  constructor(type, value, line, column) {
    this.type = type;
    this.value = value;
    this.line = line;
    this.column = column;
  }
}

class Lexer {
  constructor(input) {
    this.input = input;
    this.position = 0;
    this.line = 1;
    this.column = 1;
    this.tokens = [];
  }

  // Advance position and update line/column
  advance() {
    if (this.position < this.input.length && this.input[this.position] === '\n') {
      this.line++;
      this.column = 1;
    } else {
      this.column++;
    }
    this.position++;
  }

  peek(offset = 0) {
    const pos = this.position + offset;
    return pos < this.input.length ? this.input[pos] : null;
  }

  // Skip whitespace
  skipWhitespace() {
    while (this.position < this.input.length && /\s/.test(this.input[this.position])) {
      this.advance();
    }
  }

  // Skip comments
  skipComments() {
    if (this.peek() === '/' && this.peek(1) === '/') {
      // Single line comment
      while (this.position < this.input.length && this.input[this.position] !== '\n') {
        this.advance();
      }
      return true;
    } else if (this.peek() === '/' && this.peek(1) === '*') {
      // Multi-line comment
      this.advance(); // Skip /
      this.advance(); // Skip *
      while (this.position < this.input.length - 1) {
        if (this.input[this.position] === '*' && this.input[this.position + 1] === '/') {
          this.advance(); // Skip *
          this.advance(); // Skip /
          return true;
        }
        this.advance();
      }
      throw new Error(`Unclosed comment at line ${this.line}, column ${this.column}`);
    }
    return false;
  }

  // Read identifier or keyword
  readIdentifierOrKeyword() {
    const start = this.position;
    const startCol = this.column;
    
    while (this.position < this.input.length && /[a-zA-Z0-9_]/.test(this.input[this.position])) {
      this.advance();
    }
    
    const value = this.input.substring(start, this.position);
    
    // Check if it's a keyword
    const keywords = [
      'class', 'struct', 'public', 'private', 'protected', 
      'virtual', 'override', 'new', 'delete', 'int', 'float', 
      'double', 'char', 'bool', 'void', 'long', 'short', 
      'unsigned', 'signed', 'this', 'extends', 'implements'
    ];
    
    const type = keywords.includes(value) ? 'KEYWORD' : 'IDENTIFIER';
    return new Token(type, value, this.line, startCol);
  }

  // Read number
  readNumber() {
    const start = this.position;
    const startCol = this.column;
    
    while (this.position < this.input.length && /[0-9]/.test(this.input[this.position])) {
      this.advance();
    }
    
    // Check for decimal
    if (this.peek() === '.' && /[0-9]/.test(this.peek(1))) {
      this.advance(); // Skip .
      while (this.position < this.input.length && /[0-9]/.test(this.input[this.position])) {
        this.advance();
      }
    }
    
    const value = this.input.substring(start, this.position);
    return new Token('NUMBER', value, this.line, startCol);
  }

  // Read string
  readString() {
    const start = this.position;
    const startCol = this.column;
    const quote = this.input[this.position];
    this.advance(); // Skip opening quote
    
    while (this.position < this.input.length && this.input[this.position] !== quote) {
      if (this.input[this.position] === '\\') {
        this.advance(); // Skip escape character
      }
      this.advance();
    }
    
    if (this.position >= this.input.length) {
      throw new Error(`Unterminated string at line ${this.line}, column ${startCol}`);
    }
    
    this.advance(); // Skip closing quote
    const value = this.input.substring(start, this.position);
    return new Token('STRING', value, this.line, startCol);
  }

  // Tokenize input
  tokenize() {
    while (this.position < this.input.length) {
      this.skipWhitespace();
      if (this.position >= this.input.length) break;
      
      // Skip comments
      if (this.skipComments()) continue;
      
      const char = this.input[this.position];
      const col = this.column;
      
      // Identifiers and keywords
      if (/[a-zA-Z_]/.test(char)) {
        this.tokens.push(this.readIdentifierOrKeyword());
      }
      // Numbers
      else if (/[0-9]/.test(char)) {
        this.tokens.push(this.readNumber());
      }
      // Strings
      else if (char === '"' || char === "'") {
        this.tokens.push(this.readString());
      }
      // Two-character operators
      else if (char === '-' && this.peek(1) === '>') {
        this.tokens.push(new Token('ARROW', '->', this.line, col));
        this.advance();
        this.advance();
      }
      // Single character symbols
      else if ('{}();:,~*.=<>!+-/[]'.includes(char)) {
        this.tokens.push(new Token('SYMBOL', char, this.line, col));
        this.advance();
      }
      // Unknown character
      else {
        throw new Error(`Unexpected character '${char}' at line ${this.line}, column ${this.column}`);
      }
    }
    
    // Add EOF token
    this.tokens.push(new Token('EOF', 'EOF', this.line, this.column));
    return this.tokens;
  }
}

// ===================================================================
// ANALIZADOR SINTÁCTICO (PARSER) - Con Gramática Formal
// ===================================================================

class Parser {
  constructor(tokens) {
    this.tokens = tokens;
    this.currentToken = 0;
    this.symbolTable = new Map();
    
    // Flags para detección de OOP
    this.hasClass = false;
    this.hasInheritance = false;
    this.hasPolymorphism = false;
    this.hasEncapsulation = false;
    this.hasNew = false;
    this.hasMethods = false;
    this.hasConstructor = false;
    this.hasDestructor = false;
    
    // Stack para análisis de contexto
    this.contextStack = [];
    this.currentClass = null;
  }

  // Obtener token actual
  peek(offset = 0) {
    const pos = this.currentToken + offset;
    return pos < this.tokens.length ? this.tokens[pos] : null;
  }

  // Verificar tipo de token
  check(type, value = null) {
    const token = this.peek();
    if (!token) return false;
    return token.type === type && (value === null || token.value === value);
  }

  // Consumir token esperado
  consume(type, value = null) {
    if (this.check(type, value)) {
      const token = this.tokens[this.currentToken];
      this.currentToken++;
      return token;
    }
    return null;
  }

  // Esperar token específico (con error si no se encuentra)
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

  // Programa ::= { Declaracion }
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

  // Declaracion ::= ClaseDefinicion | FuncionDefinicion | DeclaracionVariable | DeclaracionObjeto
  parseDeclaration() {
    // Intentar parsear clase
    if (this.check('KEYWORD', 'class') || this.check('KEYWORD', 'struct')) {
      this.parseClassDefinition();
    }
    // Intentar parsear función (procedural)
    else if (this.isTypeSpecifier()) {
      const savedPos = this.currentToken;
      this.parseType();
      
      if (this.check('IDENTIFIER')) {
        const name = this.consume('IDENTIFIER');
        
        // Es una función
        if (this.check('SYMBOL', '(')) {
          this.parseFunctionDefinition(name);
        }
        // Es una declaración de variable/objeto
        else {
          this.currentToken = savedPos;
          this.parseVariableOrObjectDeclaration();
        }
      } else {
        this.currentToken = savedPos;
        this.parseVariableOrObjectDeclaration();
      }
    }
    // Error: no se reconoce la declaración
    else {
      throw new Error(`Unexpected token '${this.peek().value}' at line ${this.peek().line}`);
    }
  }

  // ClaseDefinicion ::= ("class" | "struct") IDENTIFIER [Herencia] "{" CuerpoClase "}" [";"]
  parseClassDefinition() {
    const classKeyword = this.consume('KEYWORD');
    const className = this.expect('IDENTIFIER');
    
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
    
    // Parsear herencia opcional
    if (this.check('SYMBOL', ':')) {
      this.parseInheritance();
    }
    
    this.expect('SYMBOL', '{');
    this.parseClassBody();
    this.expect('SYMBOL', '}');
    this.consume('SYMBOL', ';'); // Opcional
    
    this.contextStack.pop();
    this.currentClass = null;
  }

  // Herencia ::= ":" [EspecificadorAcceso] IDENTIFIER { "," [EspecificadorAcceso] IDENTIFIER }
  parseInheritance() {
    this.expect('SYMBOL', ':');
    this.hasInheritance = true;
    
    do {
      // Especificador de acceso opcional
      if (this.check('KEYWORD', 'public') || 
          this.check('KEYWORD', 'private') || 
          this.check('KEYWORD', 'protected')) {
        this.consume('KEYWORD');
      }
      
      const baseClass = this.expect('IDENTIFIER');
      // Registrar herencia
    } while (this.consume('SYMBOL', ','));
  }

  // CuerpoClase ::= { MiembroClase }
  parseClassBody() {
    while (!this.check('SYMBOL', '}')) {
      // Especificadores de acceso
      if (this.check('KEYWORD', 'public') || 
          this.check('KEYWORD', 'private') || 
          this.check('KEYWORD', 'protected')) {
        const access = this.consume('KEYWORD');
        this.hasEncapsulation = true;
        this.expect('SYMBOL', ':');
        continue;
      }
      
      // Virtual keyword
      if (this.check('KEYWORD', 'virtual')) {
        this.consume('KEYWORD');
        this.hasPolymorphism = true;
      }
      
      // Constructor o Destructor
      if (this.check('SYMBOL', '~')) {
        this.parseDestructor();
      } else if (this.check('IDENTIFIER') && this.peek().value === this.currentClass) {
        this.parseConstructor();
      } else if (this.isTypeSpecifier() || this.check('IDENTIFIER')) {
        this.parseMemberDeclaration();
      } else {
        throw new Error(`Unexpected token in class body: '${this.peek().value}'`);
      }
    }
  }

  // Constructor ::= IDENTIFIER "(" [Parametros] ")" [":"InitList] CuerpoFuncion
  parseConstructor() {
    const name = this.expect('IDENTIFIER');
    this.hasConstructor = true;
    this.hasMethods = true;
    
    this.expect('SYMBOL', '(');
    this.parseParameterList();
    this.expect('SYMBOL', ')');
    
    // Initialization list opcional
    if (this.check('SYMBOL', ':')) {
      this.parseInitializationList();
    }
    
    this.parseFunctionBody();
  }

  // Destructor ::= "~" IDENTIFIER "(" ")" CuerpoFuncion
  parseDestructor() {
    this.expect('SYMBOL', '~');
    const name = this.expect('IDENTIFIER');
    this.hasDestructor = true;
    this.hasMethods = true;
    
    this.expect('SYMBOL', '(');
    this.expect('SYMBOL', ')');
    this.parseFunctionBody();
  }

  // MiembroDeclaracion ::= Tipo IDENTIFIER ["(" [Parametros] ")"] ["override"] ["const"] ["=" "0"] (";" | CuerpoFuncion)
  parseMemberDeclaration() {
    const type = this.parseType();
    const name = this.expect('IDENTIFIER');
    
    // Es un método
    if (this.check('SYMBOL', '(')) {
      this.hasMethods = true;
      this.expect('SYMBOL', '(');
      this.parseParameterList();
      this.expect('SYMBOL', ')');
      
      // Override keyword (después de los paréntesis)
      if (this.check('KEYWORD', 'override')) {
        this.consume('KEYWORD');
        this.hasPolymorphism = true;
      }
      
      // Const method
      if (this.check('KEYWORD', 'const')) {
        this.consume('KEYWORD');
      }
      
      // Pure virtual
      if (this.check('SYMBOL', '=') && this.peek(1) && this.peek(1).value === '0') {
        this.consume('SYMBOL', '=');
        this.consume('NUMBER');
        this.hasPolymorphism = true;
      }
      
      // Cuerpo del método o punto y coma
      if (this.check('SYMBOL', '{')) {
        this.parseFunctionBody();
      } else {
        this.expect('SYMBOL', ';');
      }
    }
    // Es un atributo
    else {
      this.expect('SYMBOL', ';');
    }
  }

  // DeclaracionObjeto ::= Tipo IDENTIFIER ["=" "new" Tipo ["(" [Argumentos] ")"]] ";"
  parseVariableOrObjectDeclaration() {
    const type = this.parseType();
    const name = this.expect('IDENTIFIER');
    
    // Verificar si es una instanciación con new
    if (this.check('SYMBOL', '=')) {
      this.consume('SYMBOL', '=');
      
      if (this.check('KEYWORD', 'new')) {
        this.consume('KEYWORD');
        this.hasNew = true;
        
        const objectType = this.parseType();
        
        // Argumentos del constructor
        if (this.check('SYMBOL', '(')) {
          this.expect('SYMBOL', '(');
          this.parseArgumentList();
          this.expect('SYMBOL', ')');
        }
      } else {
        // Expresión de inicialización
        this.parseExpression();
      }
    }
    
    this.expect('SYMBOL', ';');
  }

  // FuncionDefinicion ::= Tipo IDENTIFIER "(" [Parametros] ")" CuerpoFuncion
  parseFunctionDefinition(name) {
    this.expect('SYMBOL', '(');
    this.parseParameterList();
    this.expect('SYMBOL', ')');
    this.parseFunctionBody();
  }

  // Tipo ::= TipoBasico {"*"}
  parseType() {
    let type = '';
    
    if (this.isTypeSpecifier()) {
      type = this.consume('KEYWORD').value;
    } else if (this.check('IDENTIFIER')) {
      type = this.consume('IDENTIFIER').value;
    } else {
      throw new Error(`Expected type specifier at line ${this.peek().line}`);
    }
    
    // Punteros
    while (this.check('SYMBOL', '*')) {
      this.consume('SYMBOL', '*');
      type += '*';
    }
    
    return type;
  }

  // Auxiliares
  isTypeSpecifier() {
    const typeKeywords = [
      'int', 'float', 'double', 'char', 'bool', 'void',
      'long', 'short', 'unsigned', 'signed'
    ];
    return this.check('KEYWORD') && typeKeywords.includes(this.peek().value);
  }

  parseParameterList() {
    if (!this.check('SYMBOL', ')')) {
      do {
        this.parseType();
        this.consume('IDENTIFIER'); // Nombre opcional
      } while (this.consume('SYMBOL', ','));
    }
  }

  parseArgumentList() {
    if (!this.check('SYMBOL', ')')) {
      do {
        this.parseExpression();
      } while (this.consume('SYMBOL', ','));
    }
  }

  parseInitializationList() {
    this.expect('SYMBOL', ':');
    do {
      this.expect('IDENTIFIER');
      this.expect('SYMBOL', '(');
      this.parseArgumentList();
      this.expect('SYMBOL', ')');
    } while (this.consume('SYMBOL', ','));
  }

  parseFunctionBody() {
    this.expect('SYMBOL', '{');
    let depth = 1;
    
    while (depth > 0 && !this.check('EOF')) {
      if (this.check('SYMBOL', '{')) {
        this.consume('SYMBOL', '{');
        depth++;
      } else if (this.check('SYMBOL', '}')) {
        this.consume('SYMBOL', '}');
        depth--;
      } else {
        this.currentToken++;
      }
    }
  }

  parseExpression() {
    // Parseo simplificado de expresiones
    let parenDepth = 0;
    
    while (!this.check('EOF')) {
      if (this.check('SYMBOL', '(')) {
        parenDepth++;
      } else if (this.check('SYMBOL', ')')) {
        if (parenDepth === 0) break;
        parenDepth--;
      } else if (this.check('SYMBOL', ';') || this.check('SYMBOL', ',')) {
        if (parenDepth === 0) break;
      }
      this.currentToken++;
    }
  }

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

function analyzeOOP(code) {
  try {
    const lexer = new Lexer(code);
    const tokens = lexer.tokenize();
    const parser = new Parser(tokens);
    return parser.parse();
  } catch (error) {
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

function isOOP(code) {
  const results = analyzeOOP(code);
  return !results.error && (
    results.hasClass ||
    results.hasInheritance ||
    results.hasPolymorphism ||
    results.hasEncapsulation ||
    results.hasNew ||
    results.hasMethods
  );
}

function classifyParadigm(code) {
  const results = analyzeOOP(code);
  
  if (results.error) {
    return { paradigm: 'ERROR', details: results.error };
  }
  
  const oopFeatures = [
    results.hasClass,
    results.hasInheritance,
    results.hasPolymorphism,
    results.hasEncapsulation,
    results.hasNew,
    results.hasMethods
  ].filter(Boolean).length;
  
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

module.exports = { analyzeOOP, isOOP, classifyParadigm, Lexer, Parser };

// Punto de entrada principal
if (require.main === module) {
  const fs = require('fs');
  const filename = process.argv[2];
  
  if (!filename) {
    console.error('Usage: node oop_detector.js <filename.cpp>');
    process.exit(1);
  }
  
  try {
    const code = fs.readFileSync(filename, 'utf8');
    const classification = classifyParadigm(code);
    
    console.log('=== ANÁLISIS DE PARADIGMA ===');
    console.log(`Paradigma detectado: ${classification.paradigm}`);
    console.log('\n=== CARACTERÍSTICAS OOP DETECTADAS ===');
    console.log(JSON.stringify(classification.details, null, 2));
    
    // Mostrar tokens (para depuración)
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