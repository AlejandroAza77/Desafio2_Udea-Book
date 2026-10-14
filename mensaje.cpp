#include "mensaje.h"

// Constructor por defecto
Mensaje::Mensaje() {
    idEmisor = -1;
    idReceptor = -1;
}

// Constructor con parametros
Mensaje::Mensaje(int emisor, int receptor, const Cadena& texto, const Fecha& fecha) {
    idEmisor = -1;
    idReceptor = -1;
    setIdEmisor(emisor);
    setIdReceptor(receptor);
    contenido = texto;
    fechaEnvio = fecha;
}

// Constructor de copia
Mensaje::Mensaje(const Mensaje& otro) {
    idEmisor = otro.idEmisor;
    idReceptor = otro.idReceptor;
    contenido = otro.contenido;
    fechaEnvio = otro.fechaEnvio;
}

// Destructor
Mensaje::~Mensaje() {
}

// Getters
int Mensaje::getIdEmisor() const { return idEmisor; }
int Mensaje::getIdReceptor() const { return idReceptor; }
const Cadena& Mensaje::getContenido() const { return contenido; }
const Fecha& Mensaje::getFechaEnvio() const { return fechaEnvio; }

// Setters con validacion basica de IDs
void Mensaje::setIdEmisor(int nuevoEmisor) {
    if (nuevoEmisor >= 0) {
        idEmisor = nuevoEmisor;
    }
}

void Mensaje::setIdReceptor(int nuevoReceptor) {
    if (nuevoReceptor >= 0) {
        idReceptor = nuevoReceptor;
    }
}

void Mensaje::setContenido(const Cadena& nuevoContenido) {
    contenido = nuevoContenido;
}

void Mensaje::setFechaEnvio(const Fecha& nuevaFecha) {
    fechaEnvio = nuevaFecha;
}

// Sobrecarga del operador de asignacion
Mensaje& Mensaje::operator=(const Mensaje& otro) {
    if (this != &otro) {
        idEmisor = otro.idEmisor;
        idReceptor = otro.idReceptor;
        contenido = otro.contenido;
        fechaEnvio = otro.fechaEnvio;
    }
    return *this;
}

// Sobrecarga del operador de igualdad
bool Mensaje::operator==(const Mensaje& otro) const {
    return idEmisor == otro.idEmisor &&
           idReceptor == otro.idReceptor &&
           contenido == otro.contenido &&
           fechaEnvio == otro.fechaEnvio;
}

// Calculo de la memoria ocupada por el mensaje
long Mensaje::calcularMemoria() const {
    return static_cast<long>(sizeof(Mensaje)) +
           (contenido.calcularMemoria() - static_cast<long>(sizeof(Cadena)));
}