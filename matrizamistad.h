#ifndef MATRIZAMISTAD_H
#define MATRIZAMISTAD_H

#include "lista.h"

class MatrizAmistad {
private:
    static const int BITS_POR_PALABRA = 64;

    int numUsuarios;                  // filas y columnas de la matriz
    int palabrasPorFila;              // palabras de 64 bits que ocupa cada fila
    unsigned long long* bits;         // todas las filas, una tras otra, en un solo bloque
    int* grados;                      // amigos de cada usuario

    void reservar(int n);             // crea los bloques en ceros para n usuarios
    void liberar();
    bool indiceValido(int i) const;
    static int bitMasBajo(unsigned long long x);   // posicion del bit mas bajo en 1

public:
    MatrizAmistad();                          // sin usuarios
    MatrizAmistad(int n);                     // n usuarios, sin amistades
    MatrizAmistad(const MatrizAmistad& otra);
    ~MatrizAmistad();

    // --- Getters ---
    int getNumUsuarios() const;
    int getPalabrasPorFila() const;
    void reiniciar(int n);

    // Amistades
    bool agregarAmistad(int i, int j);
    bool sonAmigos(int i, int j) const;
    int contarAmigos(int i) const;                  // sin recorrer nada
    int contarEnComun(int i, int j) const;          // amigos que tienen los dos
    Lista<int> obtenerAmigos(int i) const;          // numeros de los amigos de i

    MatrizAmistad& operator=(const MatrizAmistad& otra);
    bool operator==(const MatrizAmistad& otra) const;

    // Memoria total (objeto + bloque de bits + arreglo de grados), en bytes
    long calcularMemoria() const;
};

#endif