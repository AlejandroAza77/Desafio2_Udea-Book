#ifndef CADENA_H
#define CADENA_H

#include <cstddef>
#include <iostream>

class Cadena {
private:
    char* _buffer;       // texto terminado en '\0'
    size_t _longitud;    // cantidad de caracteres, sin contar el '\0'

public:

    Cadena();                       // texto vacio
    Cadena(const char* str);        // a partir de un texto de C
    Cadena(const char* str, size_t longitud);   // longitud ya conocida: una sola pasada
    Cadena(const Cadena& otra);     // constructor de copia
    ~Cadena();

    size_t getLongitud() const;
    const char* c_str() const;

    void setTexto(const char* nuevoTexto);
    void setTexto(const char* nuevoTexto, size_t longitud);   // longitud ya conocida

    // Consulta
    bool estaVacia() const;

    // Operadores sobrecargados
    Cadena& operator=(const Cadena& otra);
    bool operator==(const Cadena& otra) const;
    char& operator[](size_t indice);               // el indice debe ser valido
    const char& operator[](size_t indice) const;

    long calcularMemoria() const;

    friend std::ostream& operator<<(std::ostream& os, const Cadena& cad);
};

#endif // CADENA_H