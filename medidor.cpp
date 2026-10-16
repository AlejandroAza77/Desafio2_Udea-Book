#include "Medidor.h"
#include <iostream>

long long Medidor::_iteraciones = 0;
size_t Medidor::_memoriaLocales = 0;
char Medidor::_nombres[Medidor::MAX_FUNCIONES][Medidor::LARGO_NOMBRE];
int Medidor::_usos[Medidor::MAX_FUNCIONES];
int Medidor::_cantidadFunciones = 0;

Medidor::Medidor() {}
Medidor::Medidor(const Medidor& otro) { (void)otro; }
Medidor::~Medidor() {}

bool Medidor::mismoNombre(const char* a, const char* b) {
    int i = 0;
    while (a[i] != '\0' && b[i] != '\0') {
        if (a[i] != b[i]) return false;
        i++;
    }
    return a[i] == b[i];       // iguales solo si los dos terminaron a la vez
}

void Medidor::copiarNombre(char* destino, const char* origen) {
    int i = 0;
    while (origen[i] != '\0' && i < LARGO_NOMBRE - 1) {
        destino[i] = origen[i];
        i++;
    }
    destino[i] = '\0';
}

// Control y registro

void Medidor::reiniciar() {
    _iteraciones = 0;
    _memoriaLocales = 0;
    for (int i = 0; i < _cantidadFunciones; i++) {
        _usos[i] = 0;
        _nombres[i][0] = '\0';
    }
    _cantidadFunciones = 0;
}

void Medidor::incrementarIteraciones(long long cantidad) {
    _iteraciones += cantidad;
}

void Medidor::registrarMemoriaLocal(size_t bytes) {
    _memoriaLocales += bytes;              // se ACUMULA (antes se sustituia)
}

void Medidor::registrarFuncionExterna(const char* nombre) {
    if (nombre == nullptr) return;
    // Si el nombre ya existe, se suma una invocacion
    for (int i = 0; i < _cantidadFunciones; i++) {
        if (mismoNombre(_nombres[i], nombre)) {
            _usos[i]++;
            return;
        }
    }

    if (_cantidadFunciones < MAX_FUNCIONES) {
        copiarNombre(_nombres[_cantidadFunciones], nombre);
        _usos[_cantidadFunciones] = 1;
        _cantidadFunciones++;
    }
}

// Reporte

void Medidor::mostrarReporte(long long memoriaEstructuras) {
    std::cout << "        REPORTE DE CONSUMO DE RECURSOS" << std::endl;
    std::cout << "a. Iteraciones totales requeridas: " << _iteraciones << std::endl;
    std::cout << "b. Memoria total consumida (bytes): "
              << (memoriaEstructuras + static_cast<long long>(_memoriaLocales))
              << "  (estructuras: " << memoriaEstructuras
              << ", variables locales: " << _memoriaLocales << ")" << std::endl;
    if (_cantidadFunciones == 0) {
        std::cout << "c. Funciones externas invocadas: ninguna" << std::endl;
    } else {
        std::cout << "c. Funciones externas invocadas:" << std::endl;
        for (int i = 0; i < _cantidadFunciones; i++) {
            std::cout << "   - " << _nombres[i] << ": " << _usos[i]
                      << (_usos[i] == 1 ? " invocacion" : " invocaciones") << std::endl;
        }
    }
}

// Consultas
long long Medidor::getIteraciones() { return _iteraciones; }
size_t Medidor::getMemoriaLocales() { return _memoriaLocales; }
int Medidor::getCantidadFuncionesExternas() { return _cantidadFunciones; }

int Medidor::getUsosFuncion(const char* nombre) {
    if (nombre == nullptr) return 0;
    for (int i = 0; i < _cantidadFunciones; i++) {
        if (mismoNombre(_nombres[i], nombre)) return _usos[i];
    }
    return 0;
}