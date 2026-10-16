#include "usuario.h"
#include "medidor.h"


Usuario::Usuario() {
    id = -1;
}

Usuario::Usuario(int idUsuario, const Cadena& nombreUsuario, const Cadena& correoUsuario, const Fecha& nacimiento) {
    id = -1;
    setId(idUsuario);
    nombre = nombreUsuario;
    correo = correoUsuario;
    fechaNacimiento = nacimiento;
}

Usuario::Usuario(const Usuario& otro) {
    id = otro.id;
    nombre = otro.nombre;
    contrasena = otro.contrasena;
    fechaNacimiento = otro.fechaNacimiento;
    correo = otro.correo;
    rutaFotoPerfil = otro.rutaFotoPerfil;
    ciudad = otro.ciudad;
    pais = otro.pais;
    fechaInscripcion = otro.fechaInscripcion;
    albumes = otro.albumes;
    solicitudes = otro.solicitudes;
    idxEnviados = otro.idxEnviados;
    idxRecibidos = otro.idxRecibidos;
}

Usuario::~Usuario() {
}


// Getters

const Cadena& Usuario::getContrasena() const            { return contrasena; }   // privado
int Usuario::getId() const                              { return id; }
const Cadena& Usuario::getNombre() const                { return nombre; }
const Fecha& Usuario::getFechaNacimiento() const        { return fechaNacimiento; }
const Cadena& Usuario::getCorreo() const                { return correo; }
const Cadena& Usuario::getRutaFotoPerfil() const        { return rutaFotoPerfil; }
const Cadena& Usuario::getCiudad() const                { return ciudad; }
const Cadena& Usuario::getPais() const                  { return pais; }
const Fecha& Usuario::getFechaInscripcion() const       { return fechaInscripcion; }
const Lista<Album>& Usuario::getAlbumes() const         { return albumes; }
const Lista<Solicitud>& Usuario::getSolicitudes() const { return solicitudes; }
const Lista<int>& Usuario::getIdxEnviados() const       { return idxEnviados; }
const Lista<int>& Usuario::getIdxRecibidos() const      { return idxRecibidos; }


// Setters

void Usuario::setId(int nuevoId) {
    if (nuevoId >= 0) {
        id = nuevoId;
    }
}

void Usuario::setNombre(const Cadena& nuevoNombre)            { nombre = nuevoNombre; }
void Usuario::setContrasena(const Cadena& nuevaContrasena)    { contrasena = nuevaContrasena; }
void Usuario::setFechaNacimiento(const Fecha& nuevaFecha)     { fechaNacimiento = nuevaFecha; }
void Usuario::setCorreo(const Cadena& nuevoCorreo)            { correo = nuevoCorreo; }
void Usuario::setRutaFotoPerfil(const Cadena& nuevaRuta)      { rutaFotoPerfil = nuevaRuta; }
void Usuario::setCiudad(const Cadena& nuevaCiudad)            { ciudad = nuevaCiudad; }
void Usuario::setPais(const Cadena& nuevoPais)                { pais = nuevoPais; }
void Usuario::setFechaInscripcion(const Fecha& nuevaFecha)    { fechaInscripcion = nuevaFecha; }
void Usuario::setAlbumes(const Lista<Album>& nuevosAlbumes)   { albumes = nuevosAlbumes; }
void Usuario::setSolicitudes(const Lista<Solicitud>& nuevas)  { solicitudes = nuevas; }
void Usuario::setIdxEnviados(const Lista<int>& nuevos)        { idxEnviados = nuevos; }
void Usuario::setIdxRecibidos(const Lista<int>& nuevos)       { idxRecibidos = nuevos; }


// Credenciales: la contrasena nunca sale de la clase, solo se compara

bool Usuario::verificarClave(const Cadena& clave) const {
    return getContrasena() == clave;
}

// Albumes

void Usuario::agregarAlbum(const Album& album) {
    albumes.agregar(album);
}

Album& Usuario::obtenerAlbum(int posicion) {
    if (posicion < 0 || posicion >= getCantidadAlbumes()) {
        static Album albumVacio;       // se devuelve si la posicion no existe
        return albumVacio;
    }
    return albumes[static_cast<size_t>(posicion)];
}

int Usuario::getCantidadAlbumes() const {
    return static_cast<int>(albumes.getTamanio());
}

// Solicitudes de amistad recibidas

void Usuario::recibirSolicitud(const Solicitud& solicitud) {
    solicitudes.agregar(solicitud);
}

// Posicion de la solicitud enviada por idEmisor, o -1 si no hay (auxiliar)
static int buscarSolicitud(const Lista<Solicitud>& lista, int idEmisor) {
    for (size_t i = 0; i < lista.getTamanio(); i++) {
        Medidor::incrementarIteraciones();
        if (lista[i].getIdEmisor() == idEmisor) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

bool Usuario::tieneSolicitudDe(int idEmisor) const {
    return buscarSolicitud(solicitudes, idEmisor) != -1;
}

void Usuario::quitarSolicitud(int idEmisor) {
    int posicion = buscarSolicitud(solicitudes, idEmisor);
    if (posicion != -1) {
        solicitudes.quitar(static_cast<size_t>(posicion));
    }
}

void Usuario::registrarEnviado(int idxMensaje) {
    if (idxMensaje >= 0) {
        idxEnviados.agregar(idxMensaje);
    }
}

void Usuario::registrarRecibido(int idxMensaje) {
    if (idxMensaje >= 0) {
        idxRecibidos.agregar(idxMensaje);
    }
}

Usuario& Usuario::operator=(const Usuario& otro) {
    if (this != &otro) {
        id = otro.id;
        nombre = otro.nombre;
        contrasena = otro.contrasena;
        fechaNacimiento = otro.fechaNacimiento;
        correo = otro.correo;
        rutaFotoPerfil = otro.rutaFotoPerfil;
        ciudad = otro.ciudad;
        pais = otro.pais;
        fechaInscripcion = otro.fechaInscripcion;
        albumes = otro.albumes;
        solicitudes = otro.solicitudes;
        idxEnviados = otro.idxEnviados;
        idxRecibidos = otro.idxRecibidos;
    }
    return *this;
}

bool Usuario::operator==(const Usuario& otro) const {
    return id == otro.id &&
           nombre == otro.nombre &&
           contrasena == otro.contrasena &&
           fechaNacimiento == otro.fechaNacimiento &&
           correo == otro.correo &&
           rutaFotoPerfil == otro.rutaFotoPerfil &&
           ciudad == otro.ciudad &&
           pais == otro.pais &&
           fechaInscripcion == otro.fechaInscripcion &&
           albumes == otro.albumes &&
           solicitudes == otro.solicitudes &&
           idxEnviados == otro.idxEnviados &&
           idxRecibidos == otro.idxRecibidos;
}

long Usuario::calcularMemoria() const {
    const long fijaCadena = static_cast<long>(sizeof(Cadena));
    long total = static_cast<long>(sizeof(Usuario));
    total += nombre.calcularMemoria() - fijaCadena;
    total += contrasena.calcularMemoria() - fijaCadena;
    total += correo.calcularMemoria() - fijaCadena;
    total += rutaFotoPerfil.calcularMemoria() - fijaCadena;
    total += ciudad.calcularMemoria() - fijaCadena;
    total += pais.calcularMemoria() - fijaCadena;

    total += albumes.calcularMemoria() - static_cast<long>(sizeof(Lista<Album>));
    for (size_t i = 0; i < albumes.getTamanio(); i++) {
        total += albumes[i].calcularMemoria() - static_cast<long>(sizeof(Album));
    }
    total += solicitudes.calcularMemoria() - static_cast<long>(sizeof(Lista<Solicitud>));
    total += idxEnviados.calcularMemoria() - static_cast<long>(sizeof(Lista<int>));
    total += idxRecibidos.calcularMemoria() - static_cast<long>(sizeof(Lista<int>));
    return total;
}