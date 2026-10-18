#include "mensaje.h"

// Limite de caracteres compartido por todos los mensajes
int Mensaje::maxCaracteres = 512;

Mensaje::Mensaje() {
    idEmisor = -1;
    idReceptor = -1;
    leido = false;
}

Mensaje::Mensaje(int emisor, int receptor, const Cadena& texto, const Fecha& fecha) {
    idEmisor = -1;
    idReceptor = -1;
    leido = false;
    setIdEmisor(emisor);
    setIdReceptor(receptor);
    setContenido(texto);
    fechaEnvio = fecha;
}

// Constructor de copia
Mensaje::Mensaje(const Mensaje& otro) {
    idEmisor = otro.idEmisor;
    idReceptor = otro.idReceptor;
    contenido = otro.contenido;
    fechaEnvio = otro.fechaEnvio;
    leido = otro.leido;
}

// Destructor
Mensaje::~Mensaje() {
}

// Getters
int Mensaje::getIdEmisor() const { return idEmisor; }
int Mensaje::getIdReceptor() const { return idReceptor; }
const Cadena& Mensaje::getContenido() const { return contenido; }
const Fecha& Mensaje::getFechaEnvio() const { return fechaEnvio; }
bool Mensaje::getLeido() const { return leido; }

// Setters
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

// El texto no puede estar vacio ni tener mas de maxCaracteres caracteres
void Mensaje::setContenido(const Cadena& nuevoContenido) {
    if (nuevoContenido.estaVacia()) return;
    if (nuevoContenido.getLongitud() > static_cast<size_t>(maxCaracteres)) return;
    contenido = nuevoContenido;
}

void Mensaje::setContenido(const char* texto, size_t largo) {
    if (largo == 0) return;
    if (largo > static_cast<size_t>(maxCaracteres)) return;
    contenido.setTexto(texto, largo);
}

void Mensaje::setFechaEnvio(const Fecha& nuevaFecha) {
    fechaEnvio = nuevaFecha;
}

void Mensaje::setLeido(bool nuevoEstado) {
    leido = nuevoEstado;
}

void Mensaje::marcarLeido() {
    leido = true;
}

// Limite estatico
int Mensaje::getMaxCaracteres() { return maxCaracteres; }

void Mensaje::setMaxCaracteres(int maximo) {
    if (maximo > 0) {
        maxCaracteres = maximo;
    }
}

// Operadores
Mensaje& Mensaje::operator=(const Mensaje& otro) {
    if (this != &otro) {
        idEmisor = otro.idEmisor;
        idReceptor = otro.idReceptor;
        contenido = otro.contenido;
        fechaEnvio = otro.fechaEnvio;
        leido = otro.leido;
    }
    return *this;
}

bool Mensaje::operator==(const Mensaje& otro) const {
    return idEmisor == otro.idEmisor &&
           idReceptor == otro.idReceptor &&
           contenido == otro.contenido &&
           fechaEnvio == otro.fechaEnvio &&
           leido == otro.leido;
}

// Memoria: parte fija del objeto + el texto (sin su parte fija, ya contada)
long Mensaje::calcularMemoria() const {
    return static_cast<long>(sizeof(Mensaje)) +
           (contenido.calcularMemoria() - static_cast<long>(sizeof(Cadena)));
}