#ifndef MENSAJE_H
#define MENSAJE_H

#include "Fecha.h"
#include "cadena.h"

class Mensaje {
private:
    int idEmisor;     // ID del usuario que envia el mensaje
    int idReceptor;   // ID del usuario que recibe el mensaje
    Cadena contenido; // Texto del mensaje
    Fecha fechaEnvio; // Fecha y hora de envío

public:
    // Constructores y destructor
    Mensaje();
    Mensaje(int emisor, int receptor, const Cadena& texto, const Fecha& fecha);
    Mensaje(const Mensaje& otro);
    ~Mensaje();

    // Getters
    int getIdEmisor() const;
    int getIdReceptor() const;
    const Cadena& getContenido() const;
    const Fecha& getFechaEnvio() const;

    // Setters
    void setIdEmisor(int nuevoEmisor);
    void setIdReceptor(int nuevoReceptor);
    void setContenido(const Cadena& nuevoContenido);
    void setFechaEnvio(const Fecha& nuevaFecha);

    // Sobrecarga de operadores
    Mensaje& operator=(const Mensaje& otro);
    bool operator==(const Mensaje& otro) const;

    // Metodo para medir el consumo de memoria en bytes
    long calcularMemoria() const;
};

#endif // MENSAJE_H