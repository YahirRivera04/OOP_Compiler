// oop_detector.js — Paso 3: métricas de “grado” OOP
// -----------------------------------------------------------------------------
//  ➤  Nuevos indicadores dentro de la clase:
//      • hasMethods    → se detectó al menos un método (firma con “(”)
//      • hasCtorDtor   → se halló constructor o destructor
//  Se devuelven junto a los otros flags.
//  La heurística es ligera: dentro del cuerpo de cada clase buscamos el token
//  “(” precedido por un identificador.  Si el identificador coincide con el
//  nombre de la clase → constructor (o con “~” antes → destructor).
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
        regex:
          /^(?:class|struct|public|private|protected|virtual|override|new|delete|int|float|double|char|bool|void|long|short|unsigned|signed)\b/,
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
      this.hasClass = false;
      this.hasPolymorphism = false;
      this.hasNew = false;
      this.hasMethods = false;
      this.hasCtorDtor = false;
    }
  
    // utilidades --------------------------------------------------
    peek(k = 0) { return this.tokens[this.pos + k] || null; }
    at(t, v = undefined) {
      const tok = this.peek();
      return tok && tok.type === t && (v === undefined || tok.value === v);
    }
    match(t, v = undefined) { if (this.at(t, v)) { this.pos++; return true; } return false; }
    expect(t, v = undefined) {
      if (!this.match(t, v)) throw new Error(`Se esperaba ${v ?? t} y se encontró ${this.peek()?.value}`);
    }
  
    // ------------------ Producciones ----------------------------------------
    parseProgram() {
      while (!this.at('EOF')) {
        if (!this.parseInstruction()) this.pos++; // salto token desconocido
      }
    }
  
    parseInstruction() {
      return this.parseClassDef() || this.parseObjectInstantiation();
    }
  
    // ----------- Clase -------------------------------------------------------
    parseClassDef() {
      const start = this.pos;
      if (!this.match('kw', 'class')) return false;
  
      this.hasClass = true;
      const classNameTok = this.peek();
      this.expect('id');
      const className = classNameTok.value;
  
      this.parseHerencia();
      this.expect('sym', '{');
      this.parseClassBody(className);
      this.expect('sym', '}');
      this.match('sym', ';');
      return true;
    }
  
    parseHerencia() {
      if (!this.match('sym', ':')) return false;
      if (this.at('kw')) this.match('kw');
      this.expect('id');
      while (this.match('sym', ',')) this.expect('id');
      return true;
    }
  
    parseClassBody(className) {
      let depth = 1;
      while (depth > 0 && !this.at('EOF')) {
        const tok = this.peek();
        if (tok.type === 'sym' && tok.value === '{') {
          depth++;
        } else if (tok.type === 'sym' && tok.value === '}') {
          depth--; if (depth === 0) break;
        } else if (tok.type === 'kw' && (tok.value === 'virtual' || tok.value === 'override')) {
          this.hasPolymorphism = true;
        } else if (tok.type === 'sym' && tok.value === '(') {
          // posible firma de método: miramos token anterior relevante
          const prev = this.tokens[this.pos - 1];
          if (prev && prev.type === 'id') {
            this.hasMethods = true;
            if (prev.value === className) this.hasCtorDtor = true; // constructor
          } else if (prev && prev.type === 'sym' && prev.value === '~') {
            // destructor: símbolo '~' seguido de id (clase) ya consumido antes
            this.hasMethods = true;
            this.hasCtorDtor = true;
          }
        }
        this.pos++;
      }
    }
  
    // ----------- Declaración + new ------------------------------------------
    parseObjectInstantiation() {
      const start = this.pos;
      if (!(this.match('kw') || this.match('id'))) return false; // tipo
      while (this.match('sym', '*')) {}
      if (!this.match('id')) { this.pos = start; return false; } // nombre var
      while (this.match('sym', '*')) {}
      if (!this.match('sym', '=')) { this.pos = start; return false; }
      if (!this.match('kw', 'new')) { this.pos = start; return false; }
      if (!(this.match('kw') || this.match('id'))) { this.pos = start; return false; } // tipo tras new
      while (this.match('sym', '*')) {}
      if (this.match('sym', '(')) {
        let d = 1;
        while (d > 0 && !this.at('EOF')) {
          if (this.match('sym', '(')) d++; else if (this.match('sym', ')')) d--; else this.pos++;
        }
      }
      this.expect('sym', ';');
      this.hasNew = true;
      return true;
    }
  }
  
  // ------------------------------ 3. API --------------------------------------
  function analyzeOOP(code) {
    try {
      const p = new Parser(tokenize(code));
      p.parseProgram();
      return {
        hasClass: p.hasClass,
        hasPolymorphism: p.hasPolymorphism,
        hasNew: p.hasNew,
        hasMethods: p.hasMethods,
        hasCtorDtor: p.hasCtorDtor,
      };
    } catch (e) {
      return {
        error: e.message,
        hasClass: false,
        hasPolymorphism: false,
        hasNew: false,
        hasMethods: false,
        hasCtorDtor: false,
      };
    }
  }
  
  function isOOP(code) {
    const r = analyzeOOP(code);
    return !r.error && r.hasClass;
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
  