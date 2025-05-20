#include <iostream>

// Función sencilla para elevar al cuadrado
int square(int x) {
    return x * x;
}

int main() {
    int numbers[5] = {1, 2, 3, 4, 5};
    int total = 0;

    // Bucle clásico estilo C
    for (int i = 0; i < 5; ++i) {
        total += square(numbers[i]);
    }

    std::cout << "La suma de los cuadrados es: " << total << std::endl;
    return 0;
}
