#ifndef ALMACENAMIENTO_H
#define ALMACENAMIENTO_H

#include <fstream>
#include "cadena.h"
#include "lista.h"
#include "usuario.h"
#include "mensaje.h"
#include "matrizamistad.h"

class Almacenamiento {
private:
    Cadena subRutaFija;            // la "(sub-ruta-X-fija)" de las rutas de fotos
    Cadena rutaArchivos;           // carpeta donde estan los seis archivos
    int cambiosPendientes;         // cambios en lote aun sin guardar
    int umbralCambios;             // al llegar a esta cantidad conviene guardar
    bool mensajesModificados;      // cambio el estado leido de algun mensaje
    bool fotosModificadas;         // cambio el conteo de likes de alguna foto

    // Un cargador por archivo
    bool cargarUsuarios(Lista<Usuario>& usuarios);
    bool cargarAmistades(MatrizAmistad& amistades, int numUsuarios);
    bool cargarSolicitudes(Lista<Usuario>& usuarios);
    bool cargarMensajes(Lista<Usuario>& usuarios, Lista<Mensaje>& mensajes);
    bool cargarAlbumes(Lista<Usuario>& usuarios);
    bool cargarFotos(Lista<Usuario>& usuarios);

    // Auxiliares
    void rutaDe(const char* nombreArchivo, char* destino, int max) const;
    bool abrirParaAgregar(const char* nombreArchivo, std::fstream& archivo) const;
    // Arma la ruta en un arreglo de char y devuelve su largo (sin copias intermedias)
    int armarRutaFoto(char* destino, int max, const Cadena& usuario, const Cadena& album, const char* foto) const;
    Cadena construirRutaFoto(const Cadena& usuario, const Cadena& album, const Cadena& foto) const;
    int armarRutaAlbum(char* destino, int max, const Cadena& usuario, const Cadena& album) const;
    Cadena construirRutaAlbum(const Cadena& usuario, const Cadena& album) const;

public:

    Almacenamiento();
    Almacenamiento(const Cadena& subRuta, const Cadena& carpetaArchivos, int umbral);
    Almacenamiento(const Almacenamiento& otro);
    ~Almacenamiento();

    const Cadena& getSubRutaFija() const;
    const Cadena& getRutaArchivos() const;
    int getCambiosPendientes() const;
    int getUmbralCambios() const;
    bool getMensajesModificados() const;
    bool getFotosModificadas() const;


    void setSubRutaFija(const Cadena& nuevaSubRuta);
    void setRutaArchivos(const Cadena& nuevaRuta);
    void setCambiosPendientes(int nuevosCambios);
    void setUmbralCambios(int nuevoUmbral);
    void setMensajesModificados(bool valor);
    void setFotosModificadas(bool valor);

    bool cargarDatos(Lista<Usuario>& usuarios, Lista<Mensaje>& mensajes, MatrizAmistad& amistades);

    bool guardarMensajeNuevo(const Mensaje& mensaje);
    bool guardarSolicitudNueva(const Solicitud& solicitud);
    bool guardarAmistadNueva(int i, int j);
    // Al aceptar una solicitud hay que reescribir el archivo (es pequeno)
    bool guardarSolicitudes(const Lista<Usuario>& usuarios);

    //
    void anotarCambioMensajes();     // se cambio el estado leido de algun mensaje
    void anotarCambioFotos();        // se cambio el conteo de likes de alguna foto
    bool debeGuardarse() const;      // true si los cambios acumulados llegaron al umbral
    bool guardarPendientes(const Lista<Usuario>& usuarios, const Lista<Mensaje>& mensajes);

    //  Operadores sobrecargados
    Almacenamiento& operator=(const Almacenamiento& otro);
    bool operator==(const Almacenamiento& otro) const;

    // Memoria total del objeto, en bytes
    long calcularMemoria() const;
};

#endif