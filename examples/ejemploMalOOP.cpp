// ejemplo.cpp

class Animal {
public:
    void speak() { std::cout << "Animal speaks" << std::endl; }
};

class Dog : Animal {               // ① herencia privada implícita (no es el mayor de los problemas aquí)
public:
    void speak() override {        // ② intento de override sobre función NO virtual
        std::cout << "Woof!" << std::endl   // ③ falta ';' al final de la instrucción
    }                              // cierre de método
}                                  // ④ falta ';' tras la definición de la clase

int main() {
    Dog d;
    Animal* a = &d;
    a->speak();
    return 0                       // ⑤ falta ';' aquí también
}
