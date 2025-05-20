#include <iostream>
using namespace std;

// Clase base
class Animal {
public:
    string nombre;

    Animal(string n) {
        nombre = n;
    }

    virtual void hablar() {
        cout << "Soy un animal" << endl;
    }

    virtual ~Animal() {}
};

// Clase derivada
class Perro : public Animal {
public:
    Perro(string n) : Animal(n) {}

    void hablar() override {
        cout << "Guau! Soy " << nombre << endl;
    }
};

int main() {
    Animal* miAnimal = new Perro("Firulais");
    miAnimal->hablar();
    delete miAnimal;
    return 0;
}
