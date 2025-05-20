// oop_detector.js (versión extendida)
// ------------------------------------------------------------
// Analizador léxico + parser de descenso recursivo **minimalista**
// para reconocer rasgos OOP en código C++:
//   • Definición de clases (con herencia opcional)
//   • Métodos marcados como `virtual` y/​o `override`  ➜ polimorfismo
//   • Instanciación dinámica con `new`
// 
// Devuelve un objeto con tres flags:
//   {
//     hasClass:         bool,   // apareció al menos una clase
//     hasPolymorphism:  bool,   // se detectó virtual|override
//     hasNew:           bool    // se vio "new Tipo(…)" o "new Tipo"
//   }
// 
// Si prefieres sólo true/false clás‑isOOP: el módulo también exporta
// `isOOP()` que evalúa "algún flag = true".
// ------------------------------------------------------------

// 1)  TOKENIZER ------------------------------------------------
function tokenize(code) {
    const tokens = [];
  
    const rules = [
      { type: "whitespace",      regex: /^[\s]+/ },
      { type: "comment_single",  regex: /^\/\/.*(?:\n|$)/ },
      { type: "comment_multi",   regex: /^\/\*[\s\S]*?\*\// },
  
      // Operadores & símbolos
      { type: "arrow",           regex: /^->/ },
      { type: "scope",           regex: /^::/ },
      { type: "symbol",          regex: /^[{}();:,~*.=]/ },
  
      // Palabras clave relevantes para OOP
      { type: "keyword",         regex: /^(?:class|struct|public|private|protected|virtual|override|new|delete)\b/ },
  
      { type: "number",          regex: /^[0-9]+(?:\.[0-9]+)?/ },
      { type: "string",          regex: /^"(?:\\.|[^"])*"/ },
      { type: "identifier",      regex: /^[A-Za-z_][A-Za-z0-9_]*/ },
    ];
  
    let text = code;
    while (text.length) {
      let matched = false;
  
      for (const rule of rules) {
        const m = text.match(rule.regex);
        if (m) {
          matched = true;
          if (!rule.type.startsWith("whitespace") && !rule.type.startsWith("comment")) {
            tokens.push({ type: rule.type, value: m[0] });
          }
          text = text.slice(m[0].length);
          break;
        }
      }
  
      if (!matched) {
        tokens.push({ type: "unknown", value: text[0] });
        text = text.slice(1);
      }
    }
    return tokens;
  }
  
  // 2)  PARSER ---------------------------------------------------
  function Parser(tokens) {
    this.tokens = tokens;
    this.pos = 0;
  
    // Flags de hallazgo
    this.hasClass = false;
    this.hasPolymorphism = false;
    this.hasNew = false;
  
    // helpers --------------------------------------------------
    this.peek = (offset = 0) => this.tokens[this.pos + offset] || null;
  
    this.consume = (type, value = undefined) => {
      const tok = this.peek();
      if (tok && tok.type === type && (value === undefined || tok.value === value)) {
        this.pos++;
        return tok;
      }
      return null;
    };
  
    // parser ordinario ----------------------------------------
    this.parseProgram = () => {
      while (this.peek()) {
        const tok = this.peek();
  
        // Definición de clase → analiza y marca flag
        if (tok.type === "keyword" && tok.value === "class") {
          this.parseClassDef();
          this.hasClass = true;
          continue;
        }
  
        // Patrón instanciación con "new"  [ Identificador '=' ] new Identificador '('  ?
        if (tok.type === "keyword" && tok.value === "new") {
          this.hasNew = true;
          // consume hasta semicolon para avanzar
          while (this.peek() && !(this.peek().type === "symbol" && this.peek().value === ";")) this.pos++;
          this.consume("symbol", ";");
          continue;
        }
  
        // Otro token → lo ignoramos
        this.pos++;
      }
    };
  
    // class X [: inheritance] { … } ;
    this.parseClassDef = () => {
      this.consume("keyword", "class");
      this.consume("identifier"); // nombre
  
      // Herencia opcional sencilla  : access? Base [, Base]*
      if (this.consume("symbol", ":")) {
        if (this.peek().type === "keyword") this.consume("keyword"); // public/private/protected opc.
        this.consume("identifier");
        while (this.consume("symbol", ",")) this.consume("identifier");
      }
  
      if (!this.consume("symbol", "{")) return; // si faltara { abortamos parsing de clase
  
      let braceDepth = 1;
      while (braceDepth > 0 && this.peek()) {
        const t = this.peek();
  
        if (t.type === "symbol" && t.value === "{") braceDepth++;
        else if (t.type === "symbol" && t.value === "}") braceDepth--;
  
        // Dentro del cuerpo detectamos keywords virtual/override
        if (t.type === "keyword" && (t.value === "virtual" || t.value === "override")) {
          this.hasPolymorphism = true;
        }
  
        this.pos++;
      }
  
      this.consume("symbol", ";"); // ; opcional después de la clase, pero muy común
    };
  }
  
  // 3)  API externa -------------------------------------------
  function analyzeOOP(code) {
    try {
      const parser = new Parser(tokenize(code));
      parser.parseProgram();
      return {
        hasClass: parser.hasClass,
        hasPolymorphism: parser.hasPolymorphism,
        hasNew: parser.hasNew,
      };
    } catch (_) {
      return { hasClass: false, hasPolymorphism: false, hasNew: false };
    }
  }
  
  function isOOP(code) {
    const r = analyzeOOP(code);
    return r.hasClass || r.hasPolymorphism || r.hasNew;
  }
  
  module.exports = { isOOP, analyzeOOP };
  
  // 4)  CLI ----------------------------------------------------
  if (require.main === module) {
    const fs = require("fs");
    const path = process.argv[2];
    if (!path) {
      console.error("Uso: node oop_detector.js <archivo.cpp>");
      process.exit(1);
    }
    const src = fs.readFileSync(path, "utf8");
    const info = analyzeOOP(src);
    console.log(JSON.stringify(info, null, 2));
  }
  