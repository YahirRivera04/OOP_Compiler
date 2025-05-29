#include <iostream>
EXTRA     EXTRA
using namespace std;
EXTRA EXTRA    EXTRA END
// Clase base

CLASS_KW IDENTIFIER ABRE_LLAVE
class Animal {

ACCESO DOS_PUNTOS
public:

    TYPE    IDENTIFIER END
    string nombre;

    IDENTIFIER ABRE_PAR TYPE IDENTIFIER CIERRA_PAR ABRE_LLAVE
    Animal(string n) {
        IDENTIFIER IGUAL IDENTIFIER END
        nombre = n;
    }
    ABRE_LLAVE


    SQUIGLY IDENTIFIER ABRE_PAR CIERRA_PAR ABRE_LLAVE CIERRA_LLAVE
    ~Animal() {}

    CIERRA_LLAVE END
};

// Clase derivada

CLASS_KW IDENTIFIER DOS_PUNTOS ESPECIFICADOR_ACCESO IDENTIFIER ABRE_LLAVE
class Perro : public Animal {

ESPECIFICADOR_ACCESO DOS_PUNTOS
public:

    IDENTIFIER ABRE_PAR TYPE IDENTIFIER CIERRA_PAR DOS_PUNTOS IDENTIFIER ABRE_PAR IDENTIFIER CIERRA_PAR ABRE_LLAVE CIERRA_LLAVE
    Perro(string n) : Animal(n) {}

    TYPE IDENTIFIER ABRE_PAR CIERRA_PAR OVERRIDE_KW ABRE_LLAVE
    void hablar() override {

        EXTRA EXTRA EXTRA EXTRA EXTRA EXTRA IDENTIFIER EXTRA EXTRA EXTRA END
        cout << "Guau! Soy " << nombre << endl;
    }
};

int main() {
    Animal* miAnimal = new Perro("Firulais");
    miAnimal->hablar();
    delete miAnimal;
    return 0;
}
