#include "Cadena.h"
#include "Medidor.h"

static size_t mi_strlen(const char* str) {
    size_t len = 0;
    while (str && str[len] != '\0') {
        Medidor::incrementarIteraciones();
        len++;
    }
    return len;
}

static void mi_strcpy(char* destino, const char* origen) {
    size_t i = 0;
    while (origen && origen[i] != '\0') {
        Medidor::incrementarIteraciones();
        destino[i] = origen[i];
        i++;
    }
    destino[i] = '\0';
}

Cadena::Cadena() {
    _longitud = 0;
    _buffer = new char[1];
    _buffer[0] = '\0';
}

Cadena::Cadena(const char* str) {
    if (str == nullptr) {
        _longitud = 0;
        _buffer = new char[1];
        _buffer[0] = '\0';
    } else {
        _longitud = mi_strlen(str);
        _buffer = new char[_longitud + 1];
        mi_strcpy(_buffer, str);
    }
}
// Con la longitud conocida no hace falta recorrer el texto para medirlo
Cadena::Cadena(const char* str, size_t longitud) {
    _longitud = longitud;
    _buffer = new char[longitud + 1];
    for (size_t i = 0; i < longitud; ++i) {
        Medidor::incrementarIteraciones();
        _buffer[i] = str[i];
    }
    _buffer[longitud] = '\0';
}

// Constructor de copia
Cadena::Cadena(const Cadena& otra) {
    _longitud = otra._longitud;
    _buffer = new char[_longitud + 1];
    mi_strcpy(_buffer, otra._buffer);
}

// Destructor
Cadena::~Cadena() {
    if (_buffer != nullptr) {
        delete[] _buffer;
        _buffer = nullptr;
    }
}

size_t Cadena::getLongitud() const {
    return _longitud;
}

const char* Cadena::c_str() const {
    return _buffer;
}

void Cadena::setTexto(const char* nuevoTexto) {
    size_t nuevaLongitud = mi_strlen(nuevoTexto);   // nullptr se trata como ""
    char* nuevo = new char[nuevaLongitud + 1];
    mi_strcpy(nuevo, nuevoTexto);
    delete[] _buffer;
    _buffer = nuevo;
    _longitud = nuevaLongitud;
}

void Cadena::setTexto(const char* nuevoTexto, size_t longitud) {
    char* nuevo = new char[longitud + 1];
    for (size_t i = 0; i < longitud; ++i) {
        Medidor::incrementarIteraciones();
        nuevo[i] = nuevoTexto[i];
    }
    nuevo[longitud] = '\0';
    delete[] _buffer;
    _buffer = nuevo;
    _longitud = longitud;
}

bool Cadena::estaVacia() const {
    return _longitud == 0;
}

// Asignacion: tambien reserva antes de liberar
Cadena& Cadena::operator=(const Cadena& otra) {
    if (this != &otra) {
        char* nuevo = new char[otra._longitud + 1];
        mi_strcpy(nuevo, otra._buffer);
        delete[] _buffer;
        _buffer = nuevo;
        _longitud = otra._longitud;
    }
    return *this;
}

// Igualdad: mismo largo y mismos caracteres
bool Cadena::operator==(const Cadena& otra) const {
    if (_longitud != otra._longitud) return false;
    for (size_t i = 0; i < _longitud; ++i) {
        Medidor::incrementarIteraciones();
        if (_buffer[i] != otra._buffer[i]) return false;
    }
    return true;
}

char& Cadena::operator[](size_t indice) {
    return _buffer[indice];
}

const char& Cadena::operator[](size_t indice) const {
    return _buffer[indice];
}

long Cadena::calcularMemoria() const {
    return static_cast<long>((_longitud + 1) * sizeof(char) + sizeof(Cadena));
}

std::ostream& operator<<(std::ostream& os, const Cadena& cad) {
    os << cad._buffer;
    return os;
}