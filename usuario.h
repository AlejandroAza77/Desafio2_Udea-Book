#ifndef USUARIO_H
#define USUARIO_H

#include "cadena.h"
#include "Fecha.h"

class Usuario {
private:
    int id;
    Cadena nombre;
    Cadena correo;
    Fecha fechaNacimiento;

public:
    // Constructores y destructor
    Usuario();
    Usuario(int idUsuario, const Cadena& nombreUsuario, const Cadena& correoUsuario, const Fecha& nacimiento);
    Usuario(const Usuario& otro);
    ~Usuario();

    // Getters
    int getId() const;
    const Cadena& getNombre() const;
    const Cadena& getCorreo() const;
    const Fecha& getFechaNacimiento() const;

    // Setters
    void setId(int nuevoId);
    void setNombre(const Cadena& nuevoNombre);
    void setCorreo(const Cadena& nuevoCorreo);
    void setFechaNacimiento(const Fecha& nuevaFecha);

    // Sobrecarga de operadores
    Usuario& operator=(const Usuario& otro);
    bool operator==(const Usuario& otro) const;

    // Metodo para medir el consumo de memoria en bytes
    long calcularMemoria() const;
};

#endif // USUARIO_H