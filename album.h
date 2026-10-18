#ifndef ALBUM_H
#define ALBUM_H

#include "Cadena.h"
#include "Foto.h"
#include "Lista.h"

// Quien puede ver un album
enum Privacidad {
    PUBLICO,    // cualquier usuario de la plataforma
    PRIVADO,    // solo el dueno
    AMIGOS      // solo los amigos del dueno (y el dueno)
};


class Album {
private:
    Cadena nombre;
    Cadena ruta;
    Privacidad privacidad;
    Lista<Foto> fotos;

public:
    Album();                                                        // vacio y privado
    Album(const Cadena& nombreAlbum, const Cadena& rutaAlbum, Privacidad priv);
    Album(const Album& otro);
    ~Album();

    const Cadena& getNombre() const;
    const Cadena& getRuta() const;
    Privacidad getPrivacidad() const;
    const Lista<Foto>& getFotos() const;

    void setNombre(const Cadena& nuevoNombre);
    void setNombre(const char* nuevoNombre, size_t largo);   // copia el texto una sola vez
    void setRuta(const Cadena& nuevaRuta);
    void setRuta(const char* nuevaRuta, size_t largo);       // copia el texto una sola vez
    void setPrivacidad(Privacidad nuevaPrivacidad);
    void setFotos(const Lista<Foto>& nuevasFotos);

    // Fotos
    void agregarFoto(const Foto& foto);
    Foto& agregarFotoVacia();            // agrega una foto vacia y devuelve su referencia

    Foto& obtenerFoto(int posicion);
    int getCantidadFotos() const;

    bool esVisiblePara(bool esDueno, bool esAmigo) const;

    // --- Operadores sobrecargados ---
    Album& operator=(const Album& otro);
    bool operator==(const Album& otro) const;

    long calcularMemoria() const;
};

#endif