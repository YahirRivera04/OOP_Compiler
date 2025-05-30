
class Vehiculo {
protected:
    string marca;

public:
     ~Vehiculo();

    void conducir() const {
        cout << "Conduciendo un vehículo de marca " << marca << '\n';
    }
};

class Coche : public Vehiculo {
    int puertas;

public:

    void conducir() const override {
        cout << "Coche " << marca << " con " << puertas
             << " puertas en marcha\n";
    }
};


class Moto : public Vehiculo {
public:       

    void conducir() const override {
        cout << "Moto " << marca << " rugiendo por la pista\n";
    }
};


int main() {
    Vehiculo* v1 = new Coche("Toyota", 4);   
    Vehiculo* v2 = new Moto("Yamaha");       

    v1->conducir();                          
    v2->conducir();                          

    delete v1;                               
    delete v2;                               
    return 0;
}
