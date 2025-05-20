// oop_detector.js — Paso 1: estructura RD canónica básica
// -----------------------------------------------------------------------------
// Repartimos el parser en funciones por producción clave, sin abarcar todavía
// TODOS los casos.  Cobertura inicial:
//   • Definición de clases (marca hasClass, busca virtual/override en el cuerpo)
//   • Detección de instancias dinámicas via palabra clave `new` (hasNew)
//   • Flags de polimorfismo cuando aparece `virtual` u `override`.
// Más adelante podremos añadir: atributos, métodos con cuerpo, constructores,
// herencia con especificadores de acceso, etc.
// -----------------------------------------------------------------------------

// ------------------------------ 1. TOKENIZER --------------------------------
function tokenize(code) {
    const tokens = [];
    const rules = [
      { type: 'ws', regex: /^[\s]+/ },
      { type: 'comment1', regex: /^\/\/.*(?:\n|$)/ },
      { type: 'comment2', regex: /^\/\*[\s\S]*?\*\// },
  
      { type: 'arrow', regex: /^->/ },
      { type: 'scope', regex: /^::/ },
      { type: 'sym', regex: /^[{}();:,~*.=]/ },
  
      {
        type: 'kw',
        regex: /^(?:class|struct|public|private|protected|virtual|override|new|delete)\b/,
      },
  
      { type: 'num', regex: /^[0-9]+(?:\.[0-9]+)?/ },
      { type: 'str', regex: /^"(?:\\.|[^"])*"/ },
      { type: 'id', regex: /^[A-Za-z_][A-Za-z0-9_]*/ },
    ];
  
    let text = code;
    while (text.length) {
      let matched = false;
      for (const r of rules) {
        const m = text.match(r.regex);
        if (m) {
          matched = true;
          if (!r.type.startsWith('ws') && !r.type.startsWith('comment')) {
            tokens.push({ type: r.type, value: m[0] });
          }
          text = text.slice(m[0].length);
          break;
        }
      }
  
      if (!matched) {
        tokens.push({ type: 'unknown', value: text[0] });
        text = text.slice(1);
      }
    }
    tokens.push({ type: 'EOF', value: '<eof>' });
    return tokens;
  }
  
  // ------------------------------ 2. PARSER -----------------------------------
  class Parser {
    constructor(tokens) {
      this.tokens = tokens;
      this.pos = 0;
  
      // indicadores
      this.hasClass = false;
      this.hasPolymorphism = false;
      this.hasNew = false;
    }
  
    // utilidades --------------------------------------------------------------
    peek(k = 0) {
      return this.tokens[this.pos + k] || null;
    }
    at(type, value = undefined) {
      const tok = this.peek();
      return tok && tok.type === type && (value === undefined || tok.value === value);
    }
    match(type, value = undefined) {
      if (this.at(type, value)) {
        this.pos++;
        return true;
      }
      return false;
    }
    expect(type, value = undefined) {
      if (!this.match(type, value)) {
        throw new Error(`Se esperaba ${value ?? type} y se encontró ${this.peek()?.value}`);
      }
    }
  
    // ------------------ Producciones ----------------------------------------
  
    parseProgram() {
      while (!this.at('EOF')) {
        if (!this.parseInstruction()) {
          // token no reconocido → avanzar para no quedar en bucle
          this.pos++;
        }
      }
    }
  
    parseInstruction() {
      return this.parseClassDef() || this.parseNewExpression();
    }
  
    // class Nombre [ : Herencia ] { … } ;?
    parseClassDef() {
      const start = this.pos;
      if (!this.match('kw', 'class')) return false;
  
      this.hasClass = true;
      this.expect('id'); // nombre de la clase
  
      this.parseHerencia(); // opcional
  
      this.expect('sym', '{');
      this.parseClassBody();
      this.expect('sym', '}');
      this.match('sym', ';'); // opcional
      return true;
    }
  
    parseHerencia() {
      if (!this.match('sym', ':')) return false;
      // podemos ignorar public/private/protected aquí
      if (this.at('kw')) this.match('kw');
      this.expect('id');
      while (this.match('sym', ',')) this.expect('id');
      return true;
    }
  
    parseClassBody() {
      let depth = 1;
      while (depth > 0 && !this.at('EOF')) {
        if (this.at('sym', '{')) {
          depth++;
        } else if (this.at('sym', '}')) {
          depth--;
          if (depth === 0) break; // la llave que cierra la clase la consume quien llama
        } else if (this.at('kw', 'virtual') || this.at('kw', 'override')) {
          this.hasPolymorphism = true;
        }
        this.pos++;
      }
    }
  
    // Detecta cualquier ocurrencia de la keyword `new` y avanza hasta `;`
    parseNewExpression() {
      if (!this.match('kw', 'new')) return false;
      this.hasNew = true;
      // saltar hasta punto y coma o EOF
      while (!this.at('sym', ';') && !this.at('EOF')) this.pos++;
      this.match('sym', ';');
      return true;
    }
  }
  
  // ------------------------------ 3. API --------------------------------------
  function analyzeOOP(code) {
    try {
      const parser = new Parser(tokenize(code));
      parser.parseProgram();
      return {
        hasClass: parser.hasClass,
        hasPolymorphism: parser.hasPolymorphism,
        hasNew: parser.hasNew,
      };
    } catch (e) {
      return {
        error: e.message,
        hasClass: false,
        hasPolymorphism: false,
        hasNew: false,
      };
    }
  }
  
  function isOOP(code) {
    const res = analyzeOOP(code);
    return !res.error && (res.hasClass || res.hasPolymorphism || res.hasNew);
  }
  
  module.exports = { analyzeOOP, isOOP };
  
  // ------------------------------ 4. CLI --------------------------------------
  if (require.main === module) {
    const fs = require('fs');
    const file = process.argv[2];
    if (!file) {
      console.error('Uso: node oop_detector.js <archivo.cpp>');
      process.exit(1);
    }
    const src = fs.readFileSync(file, 'utf8');
    console.log(JSON.stringify(analyzeOOP(src), null, 2));
  }
  