#ifndef MEDIDOR_H
#define MEDIDOR_H

#include <cstddef> // Para size_t

class Medidor {
private:
    static long long _iteraciones;
    static size_t _memoriaBytes;
    static int _usosFuncionesExternas;

public:
    // Metodos estaticos de control y registro
    static void reiniciar();
    static void incrementarIteraciones(long long cantidad = 1);
    static void registrarMemoria(size_t bytes);
    static void registrarFuncionExterna();
    static void mostrarReporte();

    // Getters de consulta
    static long long getIteraciones();
    static size_t getMemoriaBytes();
    static int getUsosFuncionesExternas();
};

#endif