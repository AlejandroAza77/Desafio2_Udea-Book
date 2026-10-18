#include "Album.h"

Album::Album() {
    privacidad = PRIVADO;
}

Album::Album(const Cadena& nombreAlbum, const Cadena& rutaAlbum, Privacidad priv) {
    nombre = nombreAlbum;
    ruta = rutaAlbum;
    privacidad = priv;
}

Album::Album(const Album& otro) {
    nombre = otro.nombre;
    ruta = otro.ruta;
    privacidad = otro.privacidad;
    fotos = otro.fotos;          // copia profunda de la lista de fotos
}

Album::~Album() {
}

const Cadena& Album::getNombre() const        { return nombre; }
const Cadena& Album::getRuta() const          { return ruta; }
Privacidad Album::getPrivacidad() const       { return privacidad; }
const Lista<Foto>& Album::getFotos() const    { return fotos; }

void Album::setNombre(const Cadena& nuevoNombre)   { nombre = nuevoNombre; }
void Album::setRuta(const Cadena& nuevaRuta)       { ruta = nuevaRuta; }
void Album::setNombre(const char* nuevoNombre, size_t largo) { nombre.setTexto(nuevoNombre, largo); }
void Album::setRuta(const char* nuevaRuta, size_t largo)     { ruta.setTexto(nuevaRuta, largo); }
void Album::setPrivacidad(Privacidad nuevaPrivacidad) { privacidad = nuevaPrivacidad; }
void Album::setFotos(const Lista<Foto>& nuevasFotos)  { fotos = nuevasFotos; }

void Album::agregarFoto(const Foto& foto) {
    fotos.agregar(foto);
}

Foto& Album::agregarFotoVacia() {
    return fotos.agregarVacio();
}

Foto& Album::obtenerFoto(int posicion) {
    if (posicion < 0 || posicion >= getCantidadFotos()) {
        static Foto fotoVacia;      // se devuelve si la posicion no existe
        return fotoVacia;
    }
    return fotos[static_cast<size_t>(posicion)];
}

int Album::getCantidadFotos() const {
    return static_cast<int>(fotos.getTamanio());
}

// Visibilidad segun la privacidad
bool Album::esVisiblePara(bool esDueno, bool esAmigo) const {
    if (esDueno) return true;                 // el dueno ve todos sus albumes
    if (privacidad == PUBLICO) return true;
    if (privacidad == AMIGOS) return esAmigo;
    return false;                             // PRIVADO: solo el dueno
}

// Operadores

Album& Album::operator=(const Album& otro) {
    if (this != &otro) {
        nombre = otro.nombre;
        ruta = otro.ruta;
        privacidad = otro.privacidad;
        fotos = otro.fotos;
    }
    return *this;
}

bool Album::operator==(const Album& otro) const {
    return nombre == otro.nombre &&
           ruta == otro.ruta &&
           privacidad == otro.privacidad &&
           fotos == otro.fotos;
}


long Album::calcularMemoria() const {
    long total = static_cast<long>(sizeof(Album));
    total += nombre.calcularMemoria() - static_cast<long>(sizeof(Cadena));
    total += ruta.calcularMemoria() - static_cast<long>(sizeof(Cadena));
    total += fotos.calcularMemoria() - static_cast<long>(sizeof(Lista<Foto>));
    for (size_t i = 0; i < fotos.getTamanio(); i++) {
        total += fotos[i].calcularMemoria() - static_cast<long>(sizeof(Foto));
    }
    return total;
}