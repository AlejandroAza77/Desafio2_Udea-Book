#ifndef UDEABOOK_H
#define UDEABOOK_H

#include "lista.h"
#include "usuario.h"
#include "mensaje.h"
#include "matrizamistad.h"
#include "almacenamiento.h"

class UdeABook {
private:
    Lista<Usuario> usuarios;
    Lista<Mensaje> mensajes;
    MatrizAmistad amistades;
    Almacenamiento almacenamiento;

public:
    // Constructores y destructor
    UdeABook();
    UdeABook(const Cadena& subRuta, const Cadena& carpetaArchivos, int umbral);
    UdeABook(const UdeABook& otro);
    ~UdeABook();

    // Metodos expuestos para la interacción directa desde el main
    bool iniciarSistema();
    int buscarUsuarioPorNombre(const Cadena& nombre) const;
    void administrarAmistades(int idUsuario);
    void visitarPerfil(int idUsuario) ;
    void verSugerencias(int idUsuario) const;
    void administrarMensajes(int idUsuario);
    void mostrarReporteRecursos() const;

    // Operadores y memoria
    UdeABook& operator=(const UdeABook& otro);
    bool operator==(const UdeABook& otro) const;
    long calcularMemoria() const;
};

#endif // UDEABOOK_H