#include "Foto.h"


Foto::Foto() {
    tamano = 0;
    likes = 0;
}

Foto::Foto(const Fecha& fecha, int tam, const Cadena& rutaArchivo, int cantidadLikes) {
    tamano = 0;
    likes = 0;
    fechaCaptura = fecha;
    ruta = rutaArchivo;
    setTamano(tam);
    setLikes(cantidadLikes);
}

Foto::Foto(const Foto& otra) {
    fechaCaptura = otra.fechaCaptura;
    tamano = otra.tamano;
    ruta = otra.ruta;
    likes = otra.likes;
}


Foto::~Foto() {
}

const Fecha& Foto::getFechaCaptura() const { return fechaCaptura; }
int Foto::getTamano() const                { return tamano; }
const Cadena& Foto::getRuta() const        { return ruta; }
int Foto::getLikes() const                 { return likes; }

void Foto::setFechaCaptura(const Fecha& nuevaFecha) {
    fechaCaptura = nuevaFecha;
}

void Foto::setTamano(int nuevoTamano) {
    if (nuevoTamano >= 0) {
        tamano = nuevoTamano;
    }
}

void Foto::setRuta(const Cadena& nuevaRuta) {
    ruta = nuevaRuta;
}

void Foto::setRuta(const char* nuevaRuta, size_t largo) {
    ruta.setTexto(nuevaRuta, largo);
}

void Foto::setLikes(int nuevosLikes) {
    if (nuevosLikes >= 0) {
        likes = nuevosLikes;
    }
}


void Foto::darLike() {
    likes++;
}

void Foto::quitarLike() {
    if (likes > 0) {
        likes--;
    }
}

Foto& Foto::operator=(const Foto& otra) {
    if (this != &otra) {
        fechaCaptura = otra.fechaCaptura;
        tamano = otra.tamano;
        ruta = otra.ruta;
        likes = otra.likes;
    }
    return *this;
}

bool Foto::operator==(const Foto& otra) const {
    return fechaCaptura == otra.fechaCaptura &&
           tamano == otra.tamano &&
           ruta == otra.ruta &&
           likes == otra.likes;
}

long Foto::calcularMemoria() const {
    return static_cast<long>(sizeof(Foto)) +
           (ruta.calcularMemoria() - static_cast<long>(sizeof(Cadena)));
}