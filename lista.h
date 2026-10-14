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
        Medidor::registrarMemoria(calcularMemoria());
    }

public:
    // Constructor por defecto
    Lista(size_t capacidadInicial = 4) {
        _tamanio = 0;
        _capacidad = capacidadInicial > 0 ? capacidadInicial : 4;
        _datos = new T[_capacidad];
        Medidor::registrarMemoria(calcularMemoria());
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

        for (size_t i = 0; i < _tamanio; ++i) {
            Medidor::incrementarIteraciones();
            _datos[i] = otra._datos[i];
        }
        Medidor::registrarMemoria(calcularMemoria());
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

        for (size_t i = 0; i < _tamanio; ++i) {
            Medidor::incrementarIteraciones();
            _datos[i] = otra._datos[i];
        }
        Medidor::registrarMemoria(calcularMemoria());

        return *this;
    }

    // Sobrecarga del operador de igualdad (==)
    bool operator==(const Lista<T>& otra) const {
        if (_tamanio != otra._tamanio) return false;
        for (size_t i = 0; i < _tamanio; ++i) {
            Medidor::incrementarIteraciones();
            if (!(_datos[i] == otra._datos[i])) {
                return false;
            }
        }
        return true;
    }

    // Agregar un elemento al final
    void agregar(const T& elemento) {
        Medidor::incrementarIteraciones();
        if (_tamanio >= _capacidad) {
            redimensionar(_capacidad * 2);
        }
        _datos[_tamanio++] = elemento;
        Medidor::registrarMemoria(calcularMemoria());
    }

    // Agregar (insertar) un elemento en una posición específica
    void agregar(size_t indice, const T& elemento) {
        if (indice > _tamanio) return;
        Medidor::incrementarIteraciones();
        if (_tamanio >= _capacidad) {
            redimensionar(_capacidad * 2);
        }
        for (size_t i = _tamanio; i > indice; --i) {
            Medidor::incrementarIteraciones();
            _datos[i] = _datos[i - 1];
        }
        _datos[indice] = elemento;
        _tamanio++;
        Medidor::registrarMemoria(calcularMemoria());
    }

    // Buscar un elemento (retorna el índice o -1 si no se encuentra)
    int buscar(const T& elemento) const {
        for (size_t i = 0; i < _tamanio; ++i) {
            Medidor::incrementarIteraciones();
            if (_datos[i] == elemento) {
                return static_cast<int>(i);
            }
        }
        return -1;
    }

    // Quitar un elemento por su índice
    void quitar(size_t indice) {
        if (indice >= _tamanio) return;
        Medidor::incrementarIteraciones();
        for (size_t i = indice; i < _tamanio - 1; ++i) {
            Medidor::incrementarIteraciones();
            _datos[i] = _datos[i + 1];
        }
        _tamanio--;
        Medidor::registrarMemoria(calcularMemoria());
    }

    // Sobrecarga de operadores []
    T& operator[](size_t indice) {
        return _datos[indice];
    }

    const T& operator[](size_t indice) const {
        return _datos[indice];
    }

    // Métodos de consulta
    size_t getTamanio() const {
        return _tamanio;
    }

    size_t getCapacidad() const {
        return _capacidad;
    }

    bool estaVacia() const {
        return _tamanio == 0;
    }

    // Método para calcular la memoria total ocupada por la lista
    long calcularMemoria() const {
        return static_cast<long>(sizeof(Lista<T>) + (_capacidad * sizeof(T)));
    }
};

#endif // LISTA_H