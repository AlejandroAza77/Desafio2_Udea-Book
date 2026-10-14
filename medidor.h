#ifndef MEDIDOR_H
#define MEDIDOR_H

#include <cstddef>

class Medidor {
private:
    static const int MAX_FUNCIONES = 10;   // funciones externas distintas que se pueden registrar
    static const int LARGO_NOMBRE = 32;    // largo maximo de un nombre (con el '\0')

    static long long _iteraciones;
    static size_t _memoriaLocales;
    static char _nombres[MAX_FUNCIONES][LARGO_NOMBRE];
    static int _usos[MAX_FUNCIONES];
    static int _cantidadFunciones;

    // Constructores y destructor privados: impiden crear objetos Medidor
    Medidor();
    Medidor(const Medidor& otro);
    ~Medidor();

    // Auxiliares propios (no se usa <cstring>)
    static bool mismoNombre(const char* a, const char* b);
    static void copiarNombre(char* destino, const char* origen);

public:

    static void reiniciar();
    static void incrementarIteraciones(long long cantidad = 1);
    static void registrarMemoriaLocal(size_t bytes);
    static void registrarFuncionExterna(const char* nombre);
    static void mostrarReporte(long long memoriaEstructuras);

    // Consultas
    static long long getIteraciones();
    static size_t getMemoriaLocales();
    static int getCantidadFuncionesExternas();
    static int getUsosFuncion(const char* nombre);   // 0 si no se ha usado
};

#endif