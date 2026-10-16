#ifndef MENSAJE_H
#define MENSAJE_H

#include "cadena.h"
#include "fecha.h"

class Mensaje {
private:
    int idEmisor;
    int idReceptor;
    Cadena contenido;    // texto del mensaje
    Fecha fechaEnvio;
    bool leido;

    static int maxCaracteres;   // limite de caracteres del texto (512)

public:
    // --- Constructores y destructor ---
    Mensaje();
    Mensaje(int emisor, int receptor, const Cadena& texto, const Fecha& fecha);
    Mensaje(const Mensaje& otro);
    ~Mensaje();

    // --- Getters ---
    int getIdEmisor() const;
    int getIdReceptor() const;
    const Cadena& getContenido() const;
    const Fecha& getFechaEnvio() const;
    bool getLeido() const;

    // --- Setters ---
    void setIdEmisor(int nuevoEmisor);
    void setIdReceptor(int nuevoReceptor);
    void setContenido(const Cadena& nuevoContenido);   // rechaza vacio o demasiado largo
    void setFechaEnvio(const Fecha& nuevaFecha);
    void setLeido(bool nuevoEstado);

    // Marca el mensaje como leido
    void marcarLeido();

    // imite de caracteres (miembro estatico)
    static int getMaxCaracteres();
    static void setMaxCaracteres(int maximo);

    //
    Mensaje& operator=(const Mensaje& otro);
    bool operator==(const Mensaje& otro) const;

    // Memoria total del objeto (parte fija + texto), en bytes
    long calcularMemoria() const;
};

#endif // MENSAJE_H