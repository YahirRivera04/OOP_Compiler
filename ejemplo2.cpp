#include <iostream>
using namespace std;

/*-------------------------------------------------
 *  Clase base
 *------------------------------------------------*/
class Animal {
public:
    static int totalAnimales;          // atributo estático

    string nombre;

    /* Constructor */
    Animal(string n) : nombre(n) {
        ++totalAnimales;
    }

    /* Método virtual  ── Polimorfismo */
    virtual void hablar() {
        cout << "Soy un animal" << endl;
    }

    /* Destructor virtual */
    virtual ~Animal() {
        --totalAnimales;
    }

    /*  Método estático (no-virtual)  */
    static int getTotal() { return totalAnimales; }
};

int Animal::totalAnimales = 0;

/*-------------------------------------------------
 *  Clase derivada 1
 *------------------------------------------------*/
class Perro : public Animal {
public:
    Perro(string n) : Animal(n) {}

    /* override  ── Polimorfismo */
    void hablar() override {
        cout << "Guau! Soy " << nombre << endl;
    }

    /* Destructor override ── cuenta como dtor */
    ~Perro() override {
        cout << nombre << " dice adiós (🐶)\n";
    }
};

/*-------------------------------------------------
 *  Clase derivada 2
 *------------------------------------------------*/
class Gato : public Animal {
public:
    Gato(string n) : Animal(n) {}

    void hablar() override {
        cout << "Miau! Soy " << nombre << endl;
    }
};

/*-------------------------------------------------
 *  Función principal
 *------------------------------------------------*/
int main() {
    /* Instanciación dinámica con NEW  ── se detecta */
    Animal* miAnimal = new Perro("Firulais");
    miAnimal->hablar();
    delete miAnimal;                        // libera memoria

    /* Instancia automática (no usa new)   ── sigue contando métodos/ctor  */
    Animal otro("Bobby");
    otro.hablar();

    /* Otra instanciación con new para probar punteros */
    Animal* gato = new Gato("Misu");
    gato->hablar();
    delete gato;

    cout << "Total animales vivos: "
         << Animal::getTotal() << endl;     // llama método estático

    return 0;
}
