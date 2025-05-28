class Animal {
private:
    string name;
public:
    Animal(string n) : name(n) {}
    virtual void speak() = 0;
};

class Dog : public Animal {
public:
    Dog(string n) : Animal(n) {}
    void speak() override {
        cout << "Woof!" << endl;
    }
};

int main() {
    Animal* pet = new Dog("Rex");
    pet->speak();
    delete pet;
    return 0;
}