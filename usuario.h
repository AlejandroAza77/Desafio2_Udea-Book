#ifndef USUARIO_H
#define USUARIO_H

#include "cadena.h"
#include "fecha.h"
#include "lista.h"
#include "album.h"
#include "solicitud.h"


class Usuario {
private:
    int id;                          // posicion del usuario en la lista de usuarios
    Cadena nombre;                   // nombre de usuario (unico)
    Cadena contrasena;
    Fecha fechaNacimiento;
    Cadena correo;
    Cadena rutaFotoPerfil;
    Cadena ciudad;
    Cadena pais;
    Fecha fechaInscripcion;
    Lista<Album> albumes;
    Lista<Solicitud> solicitudes;    // solicitudes recibidas pendientes
    Lista<int> idxEnviados;          // posiciones en la lista central de mensajes
    Lista<int> idxRecibidos;

    // La contrasena solo se lee desde dentro de la clase (seguridad)
    const Cadena& getContrasena() const;

public:
    // --- Constructores y destructor ---
    Usuario();
    Usuario(int idUsuario, const Cadena& nombreUsuario, const Cadena& correoUsuario, const Fecha& nacimiento);
    Usuario(const Usuario& otro);
    ~Usuario();

    // --- Getters ---
    int getId() const;
    const Cadena& getNombre() const;
    const Fecha& getFechaNacimiento() const;
    const Cadena& getCorreo() const;
    const Cadena& getRutaFotoPerfil() const;
    const Cadena& getCiudad() const;
    const Cadena& getPais() const;
    const Fecha& getFechaInscripcion() const;
    const Lista<Album>& getAlbumes() const;
    const Lista<Solicitud>& getSolicitudes() const;
    const Lista<int>& getIdxEnviados() const;
    const Lista<int>& getIdxRecibidos() const;

    // --- Setters ---
    void setId(int nuevoId);
    void setNombre(const Cadena& nuevoNombre);
    void setContrasena(const Cadena& nuevaContrasena);
    void setFechaNacimiento(const Fecha& nuevaFecha);
    void setCorreo(const Cadena& nuevoCorreo);
    void setRutaFotoPerfil(const Cadena& nuevaRuta);
    void setCiudad(const Cadena& nuevaCiudad);
    void setPais(const Cadena& nuevoPais);
    void setFechaInscripcion(const Fecha& nuevaFecha);
    void setAlbumes(const Lista<Album>& nuevosAlbumes);
    void setSolicitudes(const Lista<Solicitud>& nuevasSolicitudes);
    void setIdxEnviados(const Lista<int>& nuevos);
    void setIdxRecibidos(const Lista<int>& nuevos);

    // Credenciales
    bool verificarClave(const Cadena& clave) const;

    // Albumes
    void agregarAlbum(const Album& album);
    Album& agregarAlbumVacio();          // agrega un album vacio y devuelve su referencia
    // Devuelve el album de esa posicion (0 es el primero); la posicion debe ser
    // valida (usar getCantidadAlbumes). Si no lo es, devuelve un album vacio.
    Album& obtenerAlbum(int posicion);
    int getCantidadAlbumes() const;

    // --- Solicitudes de amistad recibidas ---
    void recibirSolicitud(const Solicitud& solicitud);
    bool tieneSolicitudDe(int idEmisor) const;
    void quitarSolicitud(int idEmisor);

    // --- Mensajes: guardan solo la posicion en la lista central ---
    void registrarEnviado(int idxMensaje);
    void registrarRecibido(int idxMensaje);

    // --- Operadores sobrecargados ---
    Usuario& operator=(const Usuario& otro);
    bool operator==(const Usuario& otro) const;

    // Memoria total del usuario, con todo lo que contiene, en bytes
    long calcularMemoria() const;
};

#endif // USUARIO_H