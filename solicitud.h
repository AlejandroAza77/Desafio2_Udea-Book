#ifndef SOLICITUD_H
#define SOLICITUD_H

#include "Fecha.h"

class Solicitud {
private:
    int idEmisor;
    int idReceptor;
    Fecha fecha;

public:
    // Constructores y destructor
    Solicitud();                                                  // sin asignar (-1, -1)
    Solicitud(int emisor, int receptor, const Fecha& fechaEnvio); // con datos
    Solicitud(const Solicitud& otra);                             // constructor de copia
    ~Solicitud();                                                 // destructor

    // --- Getters ---
    int getIdEmisor() const;
    int getIdReceptor() const;
    Fecha getFecha() const;

    // Setters (un numero de usuario negativo no se acepta
    void setIdEmisor(int nuevoEmisor);
    void setIdReceptor(int nuevoReceptor);
    void setFecha(const Fecha& nuevaFecha);

    //
    Solicitud& operator=(const Solicitud& otra);
    bool operator==(const Solicitud& otra) const;

    //
    long calcularMemoria() const;
};

#endif