#include "cadena.h"
#include "medidor.h"

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

// Constructor por defecto
Cadena::Cadena() {
    _longitud = 0;
    _buffer = new char[1];
    _buffer[0] = '\0';
    Medidor::registrarMemoria(sizeof(char));
}

// Constructor desde una cadena literal C (const char*)
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
    Medidor::registrarMemoria((_longitud + 1) * sizeof(char));
}

// Constructor de copia
Cadena::Cadena(const Cadena& otra) {
    _longitud = otra._longitud;
    _buffer = new char[_longitud + 1];
    mi_strcpy(_buffer, otra._buffer);
    Medidor::registrarMemoria((_longitud + 1) * sizeof(char));
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

// Operador de asignacion
Cadena& Cadena::operator=(const Cadena& otra) {
    if (this != &otra) {
        if (_buffer != nullptr) {
            delete[] _buffer;
        }
        _longitud = otra._longitud;
        _buffer = new char[_longitud + 1];
        mi_strcpy(_buffer, otra._buffer);
        Medidor::registrarMemoria((_longitud + 1) * sizeof(char));
    }
    return *this;
}

// Operador de igualdad
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