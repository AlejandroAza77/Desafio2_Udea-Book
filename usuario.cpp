#include "usuario.h"

// Constructor por defecto
Usuario::Usuario() {
    id = -1;
}

// Constructor con parametros
Usuario::Usuario(int idUsuario, const Cadena& nombreUsuario, const Cadena& correoUsuario, const Fecha& nacimiento) {
    id = -1;
    setId(idUsuario);
    nombre = nombreUsuario;
    correo = correoUsuario;
    fechaNacimiento = nacimiento;
}

// Constructor de copia
Usuario::Usuario(const Usuario& otro) {
    id = otro.id;
    nombre = otro.nombre;
    correo = otro.correo;
    fechaNacimiento = otro.fechaNacimiento;
}

// Destructor
Usuario::~Usuario() {
}

// Getters
int Usuario::getId() const { return id; }
const Cadena& Usuario::getNombre() const { return nombre; }
const Cadena& Usuario::getCorreo() const { return correo; }
const Fecha& Usuario::getFechaNacimiento() const { return fechaNacimiento; }

// Setters con validacion basica
void Usuario::setId(int nuevoId) {
    if (nuevoId >= 0) {
        id = nuevoId;
    }
}

void Usuario::setNombre(const Cadena& nuevoNombre) {
    nombre = nuevoNombre;
}

void Usuario::setCorreo(const Cadena& nuevoCorreo) {
    correo = nuevoCorreo;
}

void Usuario::setFechaNacimiento(const Fecha& nuevaFecha) {
    fechaNacimiento = nuevaFecha;
}

// Sobrecarga del operador de asignacion
Usuario& Usuario::operator=(const Usuario& otro) {
    if (this != &otro) {
        id = otro.id;
        nombre = otro.nombre;
        correo = otro.correo;
        fechaNacimiento = otro.fechaNacimiento;
    }
    return *this;
}

// Sobrecarga del operador de igualdad
bool Usuario::operator==(const Usuario& otro) const {
    return id == otro.id &&
           nombre == otro.nombre &&
           correo == otro.correo &&
           fechaNacimiento == otro.fechaNacimiento;
}

// Calculo de memoria del usuario (incluyendo la memoria dinámica de sus cadenas)
long Usuario::calcularMemoria() const {
    return static_cast<long>(sizeof(Usuario)) +
           (nombre.calcularMemoria() - static_cast<long>(sizeof(Cadena))) +
           (correo.calcularMemoria() - static_cast<long>(sizeof(Cadena)));
}