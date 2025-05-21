// TOKENIZER 

function tokenize(code) {
    const tokens = [];
    const rules = [ // Las reglas nos ayudan a identificar el regex y construir el objeto en type con su value respectivo por si se necesita
      // detectan el tipo con su respectivo regex
      { type: 'whitespace', regex: /^[\s]+/ }, // Reconoce uno o más espacios en blanco (\\s = espacio, tab, NL, CR…). Anclado con ^ (inicio del string residual).
      { type: 'comment1', regex: /^\/\/.*(?:\n|$)/ }, // Reconoce el primer tipo de comentario // 
      { type: 'comment2', regex: /^\/\*[\s\S]*?\*\// }, // Reconoce el comentario multilinea
  
      //{ type: 'arrow', regex: /^->/ }, // Operador ->. Debe ir antes de sym para no ser partido en '-' '>'.
      { type: 'symbol', regex: /^[{}();:,~*.=]/ }, //  Un solo símbolo de la lista: { } ( ) ; : , ~ * . =
  
      {
        type: 'keyword',
        regex:
          /^(?:class|struct|public|private|protected|virtual|override|new|delete|int|float|double|char|bool|void|long|short|unsigned|signed)\b/,
      },
  
      { type: 'num', regex: /^[0-9]+(?:\.[0-9]+)?/ }, // Entero o flotante
      { type: 'str', regex: /^"(?:\\.|[^"])*"/ }, // String
      { type: 'identifier', regex: /^[A-Za-z_][A-Za-z0-9_]*/ }, // Identificador valido en C++ 
    ];
  
    let text = code;
    while (text.length) {
      let matched = false;
      for (const r of rules) {
        const m = text.match(r.regex); // Intenta matchear desde el inicio del array de rules 
        if (m) {
          matched = true;
          if (!r.type.startsWith('whitespace') && !r.type.startsWith('comment')) { // Nos deshacemos del whistepace o commentarios 
            tokens.push({ type: r.type, value: m[0] }); // Si detecta un token valido agregamos el objeto al arreglo de tokens
          }
          text = text.slice(m[0].length); // Quitamos la palabra del texto y lo actualizamos para ver cuando ya no quede texto parar
          break;
        }
      }
      if (!matched) {
        /*
        Si ninguna regla coincide (matched == false):
        Empuja un token 'unknown' con el primer carácter.
        Recorta solo un carácter (slice(1)) para evitar bucle infinito.
        */
        tokens.push({ type: 'unknown', value: text[0] });
        text = text.slice(1);
      }
    }
    tokens.push({ type: 'EOF', value: '<eof>' }); // Al final metemos al array de objetos un token custom EOF para que el parser sepa cuando terminar
    return tokens;
  }
  
// PARSER 

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

    // helper functions para procesar los tokens --------------------------------------------------

    peek(k = 0) { 
      return this.tokens[this.pos + k] || null; // Regresa el token en la posición actual + k( que por default es 0) o null si se sale del rango
    }
    at(t, v = undefined) {
      const tok = this.peek(); // Sacamos el token actual y vemos si es el mismo que le pasamos como parametros en esta función; Regresa T o F
                               // Pero no lo consume
      return tok && tok.type === t && (v === undefined || tok.value === v);
    }
    match(t, v = undefined) {
      if (this.at(t, v)) { // Aqui revisamos con la función at. Si es True consumimos (avanzamos el token con pos++) y regresamos True
        this.pos++; return true; 
      } 
      return false; // O regresamos False y no consumimos 
    }
    expect(t, v = undefined) { // Es algo que si o si tiene que pasar. Si no hace match, lanzamos el error
      if (!this.match(t, v)){
        throw new Error(`Se esperaba ${v ?? t} y se encontró ${this.peek()?.value}`);
      } 
    }
  
    // Producciones

    // Inicio de la gramática
    parseProgram() {
      while (!this.at('EOF')) {
        if (!this.parseInstruction()) this.pos++; // salto token desconocido
      }
    }
  
    parseInstruction() {
      return this.parseClassDef() || this.parseObjectInstantiation();
    }
  
    // Clase 
    parseClassDef() {
      const start = this.pos;
      if (!this.match('keyword', 'class')){
        return false;
      }
  
      this.hasClass = true; // Hizo match
      const classNameTok = this.peek();
      this.expect('identifier'); // Si o Si esperamos token identifier despues
      const className = classNameTok.value; // Obtenemos el nombre(value) del Token
  
      this.parseHerencia(); // Herencia opcional
      this.expect('symbol', '{'); // Si o si esperamos una llave abierta
      this.parseClassBody(className); // Parseamos el cuerpo de la clase
      this.expect('symbol', '}'); // Si o Si esperamos una llave cerrada
      this.match('symbol', ';'); // Consumir si hay un ;
      return true;
    }
  
    parseHerencia() {
      if (!this.match('symbol', ':')){ // Intenta consumir un :, si hay es que si hay delcaración de Herencia y la consume (avanza)
         return false;  // Si no hay ":" entonces no hay Herencia en esa parte y regresa false
      }
      if (this.at('keyword')){
        /*
        Después de ":" puede venir un especificador de acceso (public, private, protected) del regex de keyword.
        at('keyword') mira si el token actual es cualquier palabra clave; si lo es, match('keyword') la consume. Si no hay keyword se omite sin error.  
        */
        this.match('keyword');
      } 
      this.expect('identifier'); // Aqui si o si debe venir el nombre de la clase base que su token es identifier
      while (this.match('symbol', ',')){
        /*
        Maneja la lista de herencia múltiple.
        •match('sybol', ',') consume una coma si existe.
          • Si consumió la coma, inmediatamente exige otro identificador (expect('identifier')).
        • El while repite hasta que ya no haya más comas.
        */
       this.expect('identifier');
      } 
      return true; // Cuando termina es por que todo se parseo bien y regresa True 
    }
  
    parseClassBody(className) {


      /*
      No se construye un arbol sintáctico completo, no buscamos cada pedazo de código.
      Solo nos importa matchear las llaves { } y detectar si hay keywords para settear las flags como True

      Esto va a seguir hasta que las llaves se cierren y de profundidad sea 0 o se acaben los tokens con End Of File 
      */

      let depth = 1; // Entramos en la primera { de profundidad

      while (depth > 0 && !this.at('EOF')) {
        const tok = this.peek(); // Obtenemos el siguiente token 

        if (tok.type === 'symbol' && tok.value === '{') {
          depth++; // Si el token es otra llave aumentamos la profundidad

        } else if (tok.type === 'symbol' && tok.value === '}') {
          depth--; if (depth === 0) break;
          // Si el token es una llave cerrada } restamos al depth

        } else if (tok.type === 'keyword' && (tok.value === 'virtual' || tok.value === 'override')) {
          this.hasPolymorphism = true;
          // Si detectamos algun keyword de virtual o override ponemos la flag de Polymorphism como true

        } else if (tok.type === 'symbol' && tok.value === '(') {
          // El ( nos dice que puede ser un constructor o destructor

          const prev = this.tokens[this.pos - 1]; // Checamos el token anterior 
          
          if (prev && prev.type === 'identifier') { // y si es un identificador lo marcamos como true
            this.hasMethods = true;

            if (prev.value === className) this.hasCtorDtor = true; // De igual manera si se llama igual que la clase
            // marcamos como Constructor/Destructor True

          } else if (prev && prev.type === 'symbol' && prev.value === '~') {
            // Si el símbolo '~' seguido de identifier (clase) ya consumido antes fue un ~ entonces marcamos el 
            // destructor como true: 
            this.hasMethods = true;
            this.hasCtorDtor = true;
          }
        }
        this.pos++; // Seguimos avanzando entre el cuerpo de la clase
      }
    }
  
    // Declaración + new

    /*
    MiClase* ptr = new MiClase(42);
    Animal miMascota = new Perro("Princesa");
    */
    parseObjectInstantiation() {
      const start = this.pos; // this.pos es el índice del token actual. Guardamos una copia o checkpoint en start antes de empezar a consumir.
      if (!(this.match('keyword') || this.match('identifier'))){
        return false;
      } 
      /*
      Tipo puede ser:
      Palabra clave (int, float, char, …) ⇒ match('keyword')
      Identificador (MiClase, Animal) ⇒ match('identifier')
      Si no encuentra ni lo uno ni lo otro, no es una declaración: aborta.

      */
      while (this.match('symbol', '*')) {}

      if (!this.match('identifier')) { 
        this.pos = start; 
        /*
        Si alguna condición obligatoria no se cumple:
         Restauramos toda la posición (this.pos = start)
         Devuelve false → quien llama a esta función sabe que esta regla NO aplica
        */
        return false; 
      } // nombre de la variable

      while (this.match('symbol', '*')) { // Con match consume todos los * punteros que sean necesarios Animal**
      }
      if (!this.match('symbol', '=')) { 
        this.pos = start; // Matcheamos el signo de = 
        return false; 
      }
      if (!this.match('keyword', 'new')) { 
        this.pos = start; // Matcheamos la keyword New
        return false; 
      }
      if (!(this.match('keyword') || this.match('identifier'))) { 
        this.pos = start; // Matcheamos el tipo despues de New
        return false; 
      } 
      while (this.match('symbol', '*')) {} // Hacemos match de n cantidad de punteros * del tipo (opcional)

      // Aqui procesamos los argumentos del Objeto creado en caso de que se necesite
      // y balanceamos los paréntesis 
      if (this.match('symbol', '(')) {
        let d = 1;
        while (d > 0 && !this.at('EOF')) {
          if (this.match('symbol', '(')){
            d++; 
          } 
          else if (this.match('symbol', ')')){
            d--; 
          }
          else this.pos++; // Avanzamos hasta cerrar los parentesis 
        }
      }
      this.expect('symbol', ';'); // Esperamos si o si ; para cerrar la creación del objeto
      this.hasNew = true; // Setteamos las flags
      return true;
    }
  }
  
  // Funciones principales del programa


  function analyzeOOP(code) { // Se crea el parser y se envuelve todo en un try/catch para evitar el corte abrupto del compilador
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
  
  function isOOP(code) { // Función para llamar a todo el programa de manera externa. Regresa True si 
    // se encontró al menos una flag 
    const r = analyzeOOP(code);
    return (
    !r.error && (
      r.hasClass        ||
      r.hasPolymorphism ||
      r.hasNew          ||
      r.hasMethods      ||
      r.hasCtorDtor
    )
  );
  }
  
  module.exports = { analyzeOOP, isOOP }; // Exportar para usarse en otro archivo. Libreria
  
// MAIN de este archivo


  if (require.main === module) {
    const fs = require('fs');
    const file = process.argv[2];
    if (!file) {
      console.error('Uso: node oop_detector.js <archivo.cpp>');
      process.exit(1);
    }
    const src = fs.readFileSync(file, 'utf8');
    console.log(JSON.stringify(analyzeOOP(src), null, 2)); // Se llama a analyzeOOP para ver que tanto OOP hay en el archivo
    console.log(isOOP(src));    // Devuelve un simple True o False para ver la presencia de OOP en el archivo 
  }
  