#include <iostream>
#include "udeabook.h"
#include "medidor.h"

int main() {
    std::cout << "        PROYECTO DESAFIO 2: UDEABOOK             \n";

    // Instanciacion del controlador principal (sub-ruta vacia, directorio actual ".", umbral 20)
    UdeABook app("", ".", 20);

    // Carga inicial de datos desde archivos de texto
    if (!app.iniciarSistema()) {
        std::cout << "Error al iniciar el sistema UdeABook.\n";
        return 1;
    }

    bool salirSistema = false;
    while (!salirSistema) {
        std::cout << "            AUTENTICACION UDEABOOK              \n";
        std::cout << "Ingrese su nombre de usuario (o 'SALIR' para cerrar): ";

        char nombreInput[128];
        std::cin >> nombreInput;

        Cadena usuarioActual(nombreInput);
        if (usuarioActual == Cadena("SALIR") || usuarioActual == Cadena("salir")) {
            break;
        }

        // Busqueda del usuario autenticado
        int idUsuario = app.buscarUsuarioPorNombre(usuarioActual);
        if (idUsuario == -1) {
            std::cout << "  [!] Usuario no encontrado. Intente nuevamente.\n";
            continue;
        }

        // Menú de Sesión Activa
        bool sesionActiva = true;
        while (sesionActiva) {
            std::cout << "  SESION ACTIVA: " << nombreInput << "\n";
            std::cout << "  1. Menu Principal de Servicios\n";
            std::cout << "  2. Rotar muestra de fotos de amigos\n";
            std::cout << "  3. Cerrar Sesion\n";
            std::cout << "Seleccione una opcion: ";

            char op;
            std::cin >> op;

            if (op == '1') {
                bool enSubmenu = true;
                while (enSubmenu) {
                    std::cout << "\n MENU PRINCIPAL DE SERVICIOS UDEABOOK \n";
                    std::cout << "1. Administrar Amistades (Solicitudes/Amigos)\n";
                    std::cout << "2. Visitar Perfil de Amigos (Albumes y Fotos)\n";
                    std::cout << "3. Ver K-Sugerencias de Amistad\n";
                    std::cout << "4. Gestion de Mensajeria Directa\n";
                    std::cout << "5. Reporte de Consumo de Recursos (Medidor)\n";
                    std::cout << "6. Volver al menuu de usuario\n";
                    std::cout << "Seleccione una opcion: ";

                    int opServicio;
                    std::cin >> opServicio;

                    switch (opServicio) {
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
                        app.mostrarReporteRecursos();
                        break;
                    case 6:
                        enSubmenu = false;
                        break;
                    default:
                        std::cout << "  [!] Opcion no valida.\n";
                    }
                }
            } else if (op == '2') {
                std::cout << "  [i] Rotando fotos ponderadas de amigos...\n";
            } else if (op == '3') {
                sesionActiva = false;
                std::cout << "  [i] Sesion de " << nombreInput << " cerrada.\n";
            } else {
                std::cout << "  [!] Opción invalida.\n";
            }
        }
    }

    std::cout << "      GRACIAS POR USAR UDEABOOK. ¡HASTA PRONTO!    \n";

    return 0;
}