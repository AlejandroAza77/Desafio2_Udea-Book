#ifndef FOTO_H
#define FOTO_H

#include "Fecha.h"
#include "Cadena.h"

class Foto {
private:
    Fecha fechaCaptura;
    int tamano;          // el enunciado no define la unidad
    Cadena ruta;         // ruta del archivo .png en el almacenamiento
    int likes;

public:
    Foto();
    Foto(const Fecha& fecha, int tam, const Cadena& rutaArchivo, int cantidadLikes);
    Foto(const Foto& otra);
    ~Foto();

    const Fecha& getFechaCaptura() const;
    int getTamano() const;
    const Cadena& getRuta() const;
    int getLikes() const;

    void setFechaCaptura(const Fecha& nuevaFecha);
    void setTamano(int nuevoTamano);
    void setRuta(const Cadena& nuevaRuta);
    void setLikes(int nuevosLikes);


    void darLike();          // suma 1
    void quitarLike();       // resta 1, sin bajar de 0

    Foto& operator=(const Foto& otra);
    bool operator==(const Foto& otra) const;

    long calcularMemoria() const;
};

#endif