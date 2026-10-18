#include <iostream>
#include "cadena.h"
#include "udeabook.h"


static void mostrarMenuSesion(const char* nombre, bool fotosDisponibles) {
    std::cout << "\n==================================================\n";
    std::cout << "  SESION ACTIVA: " << nombre << "\n";
    std::cout << "==================================================\n";
    std::cout << "  1. Administrar amistades\n";
    std::cout << "  2. Visitar perfil de un amigo\n";
    std::cout << "  3. Ver sugerencias de amistad\n";
    std::cout << "  4. Administrar mensajes\n";
    if (fotosDisponibles) {
        std::cout << "  5. Actualizar fotos de amigos (solo una vez)\n";
    }
    std::cout << "  0. Cerrar sesion\n";
}

int main() {
    std::cout << "==================================================\n";
    std::cout << "        PROYECTO DESAFIO 2: UDE@BOOK\n";
    std::cout << "==================================================\n";


    UdeABook app("", "datos", 20);
    bool cargado = app.iniciarSistema();
    if (!cargado) {
        std::cout << "Se intentara cargar desde la carpeta actual...\n";
        app = UdeABook("", ".", 20);
        cargado = app.iniciarSistema();
    }
    if (!cargado) {
        std::cout << "Error al iniciar el sistema UdeABook.\n";
        return 1;
    }

    bool salirSistema = false;
    while (!salirSistema) {
        char nombre[128];
        char clave[128];

        std::cout << "\n================ INGRESO A UDE@BOOK ================\n";
        std::cout << "Nombre de usuario (o SALIR para cerrar): ";
        if (!UdeABook::leerLinea(nombre, 128)) break;

        Cadena usuarioIngresado(nombre);
        if (usuarioIngresado == Cadena("SALIR") || usuarioIngresado == Cadena("salir")) {
            break;
        }
        std::cout << "Contrasena: ";
        if (!UdeABook::leerLinea(clave, 128)) break;

        // II. Ingreso: valida las credenciales y muestra 3 fotos de amigos
        int idUsuario = app.iniciarSesion(usuarioIngresado, Cadena(clave));
        if (idUsuario == -1) {
            continue;
        }

        bool sesionActiva = true;
        while (sesionActiva) {
            mostrarMenuSesion(nombre, app.puedeActualizarFotos());

            int opcion = 0;
            if (!UdeABook::leerEntero("Seleccione una opcion: ", 0, 5, opcion)) {
                salirSistema = true;        // se acabo la entrada
                break;
            }

            switch (opcion) {
            case 1:
                app.administrarAmistades(idUsuario);
                break;
            case 2:
                app.visitarPerfil(idUsuario);
                break;
            case 3:
                app.verSugerencias(idUsuario);
                break;
            case 4:
                app.administrarMensajes(idUsuario);
                break;
            case 5:
                if (app.puedeActualizarFotos()) {
                    app.actualizarFotosAmigos(idUsuario);
                } else {
                    std::cout << "  [!] Opcion no valida.\n";
                }
                break;
            case 0:
                sesionActiva = false;
                std::cout << "  Sesion de " << nombre << " cerrada.\n";
                break;
            }
        }

        app.guardarCambiosPendientes();
    }

    app.guardarCambiosPendientes();
    std::cout << "\n   GRACIAS POR USAR UDE@BOOK. HASTA PRONTO!\n";
    return 0;
}