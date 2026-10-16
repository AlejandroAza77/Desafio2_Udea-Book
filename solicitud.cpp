#include "Solicitud.h"


Solicitud::Solicitud() {
    idEmisor = -1;
    idReceptor = -1;
}

Solicitud::Solicitud(int emisor, int receptor, const Fecha& fechaEnvio) {
    idEmisor = -1;
    idReceptor = -1;
    setIdEmisor(emisor);
    setIdReceptor(receptor);
    fecha = fechaEnvio;        // usa el operador = de Fecha
}

// Constructor de copia
Solicitud::Solicitud(const Solicitud& otra) {
    idEmisor = otra.idEmisor;
    idReceptor = otra.idReceptor;
    fecha = otra.fecha;
}

Solicitud::~Solicitud() {
}

int Solicitud::getIdEmisor() const   { return idEmisor; }
int Solicitud::getIdReceptor() const { return idReceptor; }
Fecha Solicitud::getFecha() const    { return fecha; }

void Solicitud::setIdEmisor(int nuevoEmisor) {
    if (nuevoEmisor >= 0) {
        idEmisor = nuevoEmisor;
    }
}

void Solicitud::setIdReceptor(int nuevoReceptor) {
    if (nuevoReceptor >= 0) {
        idReceptor = nuevoReceptor;
    }
}

void Solicitud::setFecha(const Fecha& nuevaFecha) {
    fecha = nuevaFecha;
}


// Asignacion con proteccion contra la autoasignacion
Solicitud& Solicitud::operator=(const Solicitud& otra) {
    if (this != &otra) {
        idEmisor = otra.idEmisor;
        idReceptor = otra.idReceptor;
        fecha = otra.fecha;
    }
    return *this;
}

// Igualdad: mismo emisor, mismo receptor y misma fecha
bool Solicitud::operator==(const Solicitud& otra) const {
    return idEmisor == otra.idEmisor &&
           idReceptor == otra.idReceptor &&
           fecha == otra.fecha;
}

long Solicitud::calcularMemoria() const {
    return static_cast<long>(sizeof(Solicitud));
}