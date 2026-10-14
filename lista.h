#ifndef LISTA_H
#define LISTA_H

#include <cstddef>
#include "Medidor.h"

template <typename T>
class Lista {
private:
    T* _datos;
    size_t _tamanio;      // elementos guardados
    size_t _capacidad;    // espacios reservados

    // Reserva un arreglo nuevo, copia los elementos y libera el anterior
    void redimensionar(size_t nuevaCapacidad) {
        Medidor::incrementarIteraciones();
        T* nuevosDatos = new T[nuevaCapacidad];

        for (size_t i = 0; i < _tamanio; ++i) {
            Medidor::incrementarIteraciones();
            nuevosDatos[i] = _datos[i];
        }

        delete[] _datos;
        _datos = nuevosDatos;
        _capacidad = nuevaCapacidad;
    }

public:
    // Constructor por defecto
    Lista(size_t capacidadInicial = 4) {
        _tamanio = 0;
        _capacidad = capacidadInicial > 0 ? capacidadInicial : 4;
        _datos = new T[_capacidad];
    }

    // Destructor
    ~Lista() {
        delete[] _datos;
        _datos = nullptr;
    }

    // Constructor de copia (copia profunda)
    Lista(const Lista<T>& otra) {
        _tamanio = otra._tamanio;
        _capacidad = otra._capacidad;
        _datos = new T[_capacidad];

        for (size_t i = 0; i < _tamanio; ++i) {
            Medidor::incrementarIteraciones();
            _datos[i] = otra._datos[i];
        }
    }

    Lista<T>& operator=(const Lista<T>& otra) {
        if (this == &otra) {
            return *this;
        }

        T* nuevosDatos = new T[otra._capacidad];
        for (size_t i = 0; i < otra._tamanio; ++i) {
            Medidor::incrementarIteraciones();
            nuevosDatos[i] = otra._datos[i];
        }

        delete[] _datos;
        _datos = nuevosDatos;
        _tamanio = otra._tamanio;
        _capacidad = otra._capacidad;
        return *this;
    }

    bool operator==(const Lista<T>& otra) const {
        if (_tamanio != otra._tamanio) return false;
        for (size_t i = 0; i < _tamanio; ++i) {
            Medidor::incrementarIteraciones();
            if (!(_datos[i] == otra._datos[i])) return false;
        }
        return true;
    }

    // Agrega un elemento al final
    void agregar(const T& elemento) {
        Medidor::incrementarIteraciones();
        if (_tamanio >= _capacidad) {
            redimensionar(_capacidad * 2);
        }
        _datos[_tamanio++] = elemento;
    }


    void agregar(const T& elemento, size_t posicion) {
        if (posicion >= _tamanio) {
            agregar(elemento);
            return;
        }
        if (_tamanio >= _capacidad) {
            redimensionar(_capacidad * 2);
        }
        for (size_t i = _tamanio; i > posicion; --i) {
            Medidor::incrementarIteraciones();
            _datos[i] = _datos[i - 1];
        }
        _datos[posicion] = elemento;
        _tamanio++;
    }

    // Devuelve la posicion de la primera aparicion, o -1 si no esta
    int buscar(const T& elemento) const {
        for (size_t i = 0; i < _tamanio; ++i) {
            Medidor::incrementarIteraciones();
            if (_datos[i] == elemento) return static_cast<int>(i);
        }
        return -1;
    }

    void quitar(size_t posicion) {
        if (posicion >= _tamanio) return;
        for (size_t i = posicion; i + 1 < _tamanio; ++i) {
            Medidor::incrementarIteraciones();
            _datos[i] = _datos[i + 1];
        }
        _tamanio--;
    }

    // Acceso por posicion (la posicion debe ser valida)
    T& operator[](size_t indice) {
        return _datos[indice];
    }

    const T& operator[](size_t indice) const {
        return _datos[indice];
    }

    // Consultas
    size_t getTamanio() const {
        return _tamanio;
    }

    size_t getCapacidad() const {
        return _capacidad;
    }

    bool estaVacia() const {
        return _tamanio == 0;
    }

    long calcularMemoria() const {
        return static_cast<long>(sizeof(Lista<T>) + _capacidad * sizeof(T));
    }
};

#endif