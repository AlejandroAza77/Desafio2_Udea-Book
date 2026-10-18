#ifndef UDEABOOK_H
#define UDEABOOK_H

#include "cadena.h"
#include "fecha.h"
#include "lista.h"
#include "usuario.h"
#include "mensaje.h"
#include "matrizamistad.h"
#include "almacenamiento.h"


class UdeABook {
private:
    Lista<Usuario> usuarios;
    Lista<Mensaje> mensajes;          // lista central; los usuarios guardan solo la posicion
    MatrizAmistad amistades;
    Almacenamiento almacenamiento;


    Lista<int> seleccionAmigos;
    bool fotosActualizadas;

    static int numeroAleatorio(int limite);                 // entero en [0, limite)
    int pesoDeAmigo(int idUsuario, int idAmigo) const;      // 2, 5 o 7 (1, 2.5 y 3.5 por 2)
    void elegirAmigosPonderados(int idUsuario);
    void mostrarFotosAmigos(int desde, int hasta) const;

    bool enviarSolicitud(int idEmisor, int idDestino);
    void enviarSolicitudPorNombre(int idUsuario);
    void aceptarSolicitudes(int idUsuario);


    void recorrerAlbum(Usuario& dueno, int posAlbum);


    void enviarMensaje(int idUsuario);
    void leerMensajesNuevos(int idUsuario);
    void buscarMensajesPorFechas(int idUsuario);
    void ordenarPorFecha(int* posiciones, int cantidad) const;   // merge sort por fecha de envio
    void mostrarMensaje(int posicion) const;


    static Fecha fechaActual();                  // Fecha::ahora() registrando las funciones externas
    void guardarSiCorresponde();                 // guarda si los cambios llegaron al umbral
    void reportarConsumo() const;                // funcionalidad VII

public:
    // Constructores y destructor
    UdeABook();
    UdeABook(const Cadena& subRuta, const Cadena& carpetaArchivos, int umbral);
    UdeABook(const UdeABook& otro);
    ~UdeABook();

    // Carga inicial de datos (no es una opcion del menu de usuario)
    bool iniciarSistema();


    int iniciarSesion(const Cadena& nombre, const Cadena& clave);
    bool puedeActualizarFotos() const;           // la actualizacion se permite una sola vez
    void actualizarFotosAmigos(int idUsuario);

    int buscarUsuarioPorNombre(const Cadena& nombre) const;   // -1 si no existe

    // Funcionalidades del menu
    void administrarAmistades(int idUsuario);    // III
    void visitarPerfil(int idUsuario);           // IV
    void verSugerencias(int idUsuario);          // V
    void administrarMensajes(int idUsuario);     // VI

    // Escribe en los archivos los cambios que aun no se han guardado
    void guardarCambiosPendientes();

    // Lectura de consola segura (por linea completa, sin fallar con entradas raras)
    static bool leerLinea(char* destino, int max);   // false si se acabo la entrada
    static bool leerEntero(const char* mensaje, int minimo, int maximo, int& valor);

    // --- Operadores sobrecargados ---
    UdeABook& operator=(const UdeABook& otro);
    bool operator==(const UdeABook& otro) const;

    // Memoria total de todo lo que contiene, en bytes
    long calcularMemoria() const;
};

#endif // UDEABOOK_H