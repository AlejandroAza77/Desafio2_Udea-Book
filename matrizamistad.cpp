#include "matrizamistad.h"
#include "medidor.h"

//
// Auxiliares privados
//

// Crea los bloques para n usuarios, todos en cero (los "()" ponen el bloque en ceros)
void MatrizAmistad::reservar(int n) {
    numUsuarios = (n > 0) ? n : 0;
    palabrasPorFila = (numUsuarios + BITS_POR_PALABRA - 1) / BITS_POR_PALABRA;
    bits = new unsigned long long[static_cast<size_t>(numUsuarios) * palabrasPorFila]();
    grados = new int[numUsuarios]();
}

void MatrizAmistad::liberar() {
    delete[] bits;
    delete[] grados;
    bits = nullptr;
    grados = nullptr;
}

bool MatrizAmistad::indiceValido(int i) const {
    return i >= 0 && i < numUsuarios;
}

// Posicion (0 a 63) del bit mas bajo que vale 1. x no puede ser 0.
// Busca por mitades: 6 pasos, sin recorrer los 64 bits.
int MatrizAmistad::bitMasBajo(unsigned long long x) {
    int posicion = 0;
    if ((x & 0xFFFFFFFFULL) == 0) { posicion += 32; x >>= 32; }
    if ((x & 0xFFFFULL) == 0)     { posicion += 16; x >>= 16; }
    if ((x & 0xFFULL) == 0)       { posicion += 8;  x >>= 8; }
    if ((x & 0xFULL) == 0)        { posicion += 4;  x >>= 4; }
    if ((x & 0x3ULL) == 0)        { posicion += 2;  x >>= 2; }
    if ((x & 0x1ULL) == 0)        { posicion += 1; }
    return posicion;
}

MatrizAmistad::MatrizAmistad() {
    reservar(0);
}

MatrizAmistad::MatrizAmistad(int n) {
    reservar(n);
}

MatrizAmistad::MatrizAmistad(const MatrizAmistad& otra) {
    reservar(otra.numUsuarios);
    size_t total = static_cast<size_t>(numUsuarios) * palabrasPorFila;
    for (size_t k = 0; k < total; k++) {
        Medidor::incrementarIteraciones();
        bits[k] = otra.bits[k];
    }
    for (int i = 0; i < numUsuarios; i++) {
        Medidor::incrementarIteraciones();
        grados[i] = otra.grados[i];
    }
}

MatrizAmistad::~MatrizAmistad() {
    liberar();
}

// Getters y reinicio

int MatrizAmistad::getNumUsuarios() const     { return numUsuarios; }
int MatrizAmistad::getPalabrasPorFila() const { return palabrasPorFila; }

void MatrizAmistad::reiniciar(int n) {
    liberar();
    reservar(n);
}


// Amistades

// Consulta directa a un bit: no recorre nada
bool MatrizAmistad::sonAmigos(int i, int j) const {
    if (!indiceValido(i) || !indiceValido(j) || i == j) return false;
    unsigned long long palabra = bits[static_cast<size_t>(i) * palabrasPorFila + j / BITS_POR_PALABRA];
    return ((palabra >> (j % BITS_POR_PALABRA)) & 1ULL) == 1ULL;
}

// Enciende el bit (i, j) y el bit (j, i). No recorre nada.
bool MatrizAmistad::agregarAmistad(int i, int j) {
    if (!indiceValido(i) || !indiceValido(j) || i == j) return false;
    if (sonAmigos(i, j)) return false;
    bits[static_cast<size_t>(i) * palabrasPorFila + j / BITS_POR_PALABRA] |= (1ULL << (j % BITS_POR_PALABRA));
    bits[static_cast<size_t>(j) * palabrasPorFila + i / BITS_POR_PALABRA] |= (1ULL << (i % BITS_POR_PALABRA));
    grados[i]++;
    grados[j]++;
    return true;
}

int MatrizAmistad::contarAmigos(int i) const {
    if (!indiceValido(i)) return 0;
    return grados[i];
}

int MatrizAmistad::contarEnComun(int i, int j) const {
    if (!indiceValido(i) || !indiceValido(j) || i == j) return 0;
    const unsigned long long* filaI = bits + static_cast<size_t>(i) * palabrasPorFila;
    const unsigned long long* filaJ = bits + static_cast<size_t>(j) * palabrasPorFila;
    int enComun = 0;
    for (int k = 0; k < palabrasPorFila; k++) {
        Medidor::incrementarIteraciones();
        unsigned long long x = filaI[k] & filaJ[k];
        while (x != 0) {
            Medidor::incrementarIteraciones();
            x &= (x - 1);             // apaga el bit encendido mas bajo
            enComun++;
        }
    }
    return enComun;
}


Lista<int> MatrizAmistad::obtenerAmigos(int i) const {
    Lista<int> amigos;
    if (!indiceValido(i)) return amigos;
    const unsigned long long* fila = bits + static_cast<size_t>(i) * palabrasPorFila;
    for (int k = 0; k < palabrasPorFila; k++) {
        Medidor::incrementarIteraciones();
        unsigned long long x = fila[k];
        while (x != 0) {
            Medidor::incrementarIteraciones();
            amigos.agregar(k * BITS_POR_PALABRA + bitMasBajo(x));
            x &= (x - 1);
        }
    }
    return amigos;
}

MatrizAmistad& MatrizAmistad::operator=(const MatrizAmistad& otra) {
    if (this != &otra) {
        MatrizAmistad copia(otra);              // copia completa primero
        unsigned long long* bitsViejos = bits;  // se intercambian los bloques
        int* gradosViejos = grados;
        bits = copia.bits;
        grados = copia.grados;
        numUsuarios = copia.numUsuarios;
        palabrasPorFila = copia.palabrasPorFila;
        copia.bits = bitsViejos;                // la copia temporal libera lo viejo
        copia.grados = gradosViejos;
    }
    return *this;
}

bool MatrizAmistad::operator==(const MatrizAmistad& otra) const {
    if (numUsuarios != otra.numUsuarios) return false;
    size_t total = static_cast<size_t>(numUsuarios) * palabrasPorFila;
    for (size_t k = 0; k < total; k++) {
        Medidor::incrementarIteraciones();
        if (bits[k] != otra.bits[k]) return false;
    }
    return true;       // los grados salen de los bits, no hace falta compararlos
}

long MatrizAmistad::calcularMemoria() const {
    return static_cast<long>(sizeof(MatrizAmistad)) +
           static_cast<long>(numUsuarios) * palabrasPorFila * static_cast<long>(sizeof(unsigned long long)) +
           static_cast<long>(numUsuarios) * static_cast<long>(sizeof(int));
}