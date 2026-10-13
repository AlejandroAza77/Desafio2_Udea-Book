#ifndef CADENA_H
#define CADENA_H

#include <cstddef>
#include <iostream>

class Cadena {
private:
    char* _buffer;
    size_t _longitud;

public:
    // Constructores y destructor
    Cadena();
    Cadena(const char* str);
    Cadena(const Cadena& otra);
    ~Cadena();

    // Getters
    size_t getLongitud() const;
    const char* c_str() const;

    // Sobrecarga de operadores
    Cadena& operator=(const Cadena& otra);
    bool operator==(const Cadena& otra) const;
    char& operator[](size_t indice);
    const char& operator[](size_t indice) const;

    // Métodos utilitarios
    long calcularMemoria() const;

    // Sobrecarga de operador de salida para std::cout
    friend std::ostream& operator<<(std::ostream& os, const Cadena& cad);
};

#endif // CADENA_H