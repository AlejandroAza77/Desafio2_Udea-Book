#include "Medidor.h"
#include <iostream>

// Inicialización de las variables estáticas globales de la clase
long long Medidor::_iteraciones = 0;
size_t Medidor::_memoriaBytes = 0;
int Medidor::_usosFuncionesExternas = 0;

void Medidor::reiniciar() {
    _iteraciones = 0;
    _memoriaBytes = 0;
    _usosFuncionesExternas = 0;
}

void Medidor::incrementarIteraciones(long long cantidad) {
    _iteraciones += cantidad;
}

void Medidor::registrarMemoria(size_t bytes) {
    _memoriaBytes = bytes;
}

void Medidor::registrarFuncionExterna() {
    _usosFuncionesExternas++;
}

void Medidor::mostrarReporte() {
    std::cout << "        REPORTE DE CONSUMO DE RECURSOS   " << std::endl;
    std::cout << "a. Iteraciones totales requeridas: " << _iteraciones << std::endl;
    std::cout << "b. Memoria total consumida (bytes): " << _memoriaBytes << std::endl;
    std::cout << "   Invocaciones a componentes externos: " << _usosFuncionesExternas << std::endl;
}

long long Medidor::getIteraciones() {
    return _iteraciones;
}

size_t Medidor::getMemoriaBytes() {
    return _memoriaBytes;
}

int Medidor::getUsosFuncionesExternas() {
    return _usosFuncionesExternas;
}