#ifndef LISTA_CPP
#define LISTA_CPP

#include "lista.h"

template<typename T>
void Lista<T>::redimensionar(size_t nuevaCapacidad)
{
    Medidor::incrementarIteraciones();
    T *nuevosDatos = new T[nuevaCapacidad];

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

template<typename T>
Lista<T>::Lista(size_t capacidadInicial)
{
    _tamanio = 0;
    _capacidad = capacidadInicial > 0 ? capacidadInicial : 4;
    _datos = new T[_capacidad];
    Medidor::registrarMemoria(_capacidad * sizeof(T));
}

template<typename T>
Lista<T>::~Lista()
{
    if (_datos != nullptr) {
        delete[] _datos;
        _datos = nullptr;
    }
}

template<typename T>
Lista<T>::Lista(const Lista<T> &otra)
{
    _tamanio = otra._tamanio;
    _capacidad = otra._capacidad;
    _datos = new T[_capacidad];
    Medidor::registrarMemoria(_capacidad * sizeof(T));

    for (size_t i = 0; i < _tamanio; ++i) {
        Medidor::incrementarIteraciones();
        _datos[i] = otra._datos[i];
    }
}

template<typename T>
Lista<T> &Lista<T>::operator=(const Lista<T> &otra)
{
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

template<typename T>
void Lista<T>::agregar(const T &elemento)
{
    Medidor::incrementarIteraciones();
    if (_tamanio >= _capacidad) {
        redimensionar(_capacidad * 2);
    }
    _datos[_tamanio++] = elemento;
}

template<typename T>
T &Lista<T>::operator[](size_t indice)
{
    return _datos[indice];
}

template<typename T>
const T &Lista<T>::operator[](size_t indice) const
{
    return _datos[indice];
}

template<typename T>
size_t Lista<T>::getTamanio() const
{
    return _tamanio;
}

template<typename T>
size_t Lista<T>::getCapacidad() const
{
    return _capacidad;
}

template<typename T>
bool Lista<T>::estaVacia() const
{
    return _tamanio == 0;
}

#endif