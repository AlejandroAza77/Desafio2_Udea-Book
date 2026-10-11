#ifndef LISTA_H
#define LISTA_H

#include <cstddef>
#include "medidor.h"

template <typename T>
class Lista {
private:
    T* _datos;
    size_t _tamanio;
    size_t _capacidad;

    void redimensionar(size_t nuevaCapacidad) {
        Medidor::incrementarIteraciones();
        T* nuevosDatos = new T[nuevaCapacidad];

        for (size_t i = 0; i < _tamanio; ++i) {
            Medidor::incrementarIteraciones();
            nuevosDatos[i] = _datos[i];
        }

        if (_datos != nullptr) {
            delete[] _datos;
        }

        _datos = nuevosDatos;
        _capacidad = nuevaCapacidad;
        Medidor::registrarMemoria(_capacidad * sizeof(T));
    }

public:
    // Constructor por defecto
    Lista(size_t capacidadInicial = 4) {
        _tamanio = 0;
        _capacidad = capacidadInicial > 0 ? capacidadInicial : 4;
        _datos = new T[_capacidad];
        Medidor::registrarMemoria(_capacidad * sizeof(T));
    }

    // Destructor explicito
    ~Lista() {
        if (_datos != nullptr) {
            delete[] _datos;
            _datos = nullptr;
        }
    }

    // Constructor de copia
    Lista(const Lista<T>& otra) {
        _tamanio = otra._tamanio;
        _capacidad = otra._capacidad;
        _datos = new T[_capacidad];
        Medidor::registrarMemoria(_capacidad * sizeof(T));

        for (size_t i = 0; i < _tamanio; ++i) {
            Medidor::incrementarIteraciones();
            _datos[i] = otra._datos[i];
        }
    }

    // Sobrecarga del operador de asignación (=)
    Lista<T>& operator=(const Lista<T>& otra) {
        if (this == &otra) {
            return *this;
        }

        if (_datos != nullptr) {
            delete[] _datos;
        }

        _tamanio = otra._tamanio;
        _capacidad = otra._capacidad;
        _datos = new T[_capacidad];
        Medidor::registrarMemoria(_capacidad * sizeof(T));

        for (size_t i = 0; i < _tamanio; ++i) {
            Medidor::incrementarIteraciones();
            _datos[i] = otra._datos[i];
        }

        return *this;
    }

    // Agregar un elemento al final
    void agregar(const T& elemento) {
        Medidor::incrementarIteraciones();
        if (_tamanio >= _capacidad) {
            redimensionar(_capacidad * 2);
        }
        _datos[_tamanio++] = elemento;
    }

    // Sobrecarga de operadores []
    T& operator[](size_t indice) {
        return _datos[indice];
    }

    const T& operator[](size_t indice) const {
        return _datos[indice];
    }

    // Metodos de consulta
    size_t getTamanio() const {
        return _tamanio;
    }

    size_t getCapacidad() const {
        return _capacidad;
    }

    bool estaVacia() const {
        return _tamanio == 0;
    }
};

#endif