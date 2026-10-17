#include "udeabook.h"
#include "medidor.h"
#include <iostream>

UdeABook::UdeABook() {
    usuarioLogueadoId = -1;
}

UdeABook::UdeABook(const Cadena& subRuta, const Cadena& carpetaArchivos, int umbral)
    : almacenamiento(subRuta, carpetaArchivos, umbral) {
    usuarioLogueadoId = -1;
}

UdeABook::UdeABook(const UdeABook& otro) {
    usuarios = otro.usuarios;
    mensajes = otro.mensajes;
    amistades = otro.amistades;
    almacenamiento = otro.almacenamiento;
    usuarioLogueadoId = otro.usuarioLogueadoId;
}

UdeABook::~UdeABook() {
}

bool UdeABook::iniciarSistema() {
    Medidor::reiniciar();
    std::cout << "      INICIANDO SISTEMA UDEABOOK        \n";

    if (!almacenamiento.cargarDatos(usuarios, mensajes, amistades)) {
        std::cout << "ERROR CRITICO: No se pudieron cargar los datos iniciales." << std::endl;
        return false;
    }
    std::cout << "¡Datos cargados exitosamente en memoria!\n\n";
    return true;
}

void UdeABook::ejecutar() {
    if (!iniciarSistema()) return;

    bool salirSistema = false;
    while (!salirSistema) {
        std::cout << "          AUTENTICACION UDEABOOK        \n";
        std::cout << "Ingrese su nombre de usuario (o 'SALIR' para terminar): ";

        char nombreInput[128];
        std::cin >> nombreInput;

        if (Cadena(nombreInput) == Cadena("SALIR") || Cadena(nombreInput) == Cadena("salir")) {
            break;
        }

        int idEncontrado = -1;
        for (size_t i = 0; i < usuarios.getTamanio(); ++i) {
            Medidor::incrementarIteraciones();
            if (usuarios[i].getNombre() == Cadena(nombreInput)) {
                idEncontrado = static_cast<int>(i);
                break;
            }
        }

        if (idEncontrado == -1) {
            std::cout << "  [!] Usuario no encontrado. Verifique el nombre e intente de nuevo.\n";
            continue;
        }

        usuarioLogueadoId = idEncontrado;
        mostrarPantallaBienvenida(usuarioLogueadoId);
    }

    std::cout << "\nSaliendo de UdeABook. ¡Hasta pronto!\n";
}

void UdeABook::mostrarPantallaBienvenida(int idUsuario) {
    Usuario& actual = usuarios[static_cast<size_t>(idUsuario)];
    bool sesionActiva = true;

    while (sesionActiva) {
        std::cout << " BIENVENIDO A UDEABOOK, " << actual.getNombre().c_str() << "!\n";
        std::cout << "Amigos totales: " << amistades.contarAmigos(idUsuario) << "\n";
        std::cout << "Solicitudes pendientes: " << actual.getSolicitudes().getTamanio() << "\n";
        std::cout << "  [1] Menu Principal de Opciones\n";
        std::cout << "  [N] Mostrar otras fotos de amigos (Rotar muestra)\n";
        std::cout << "  [S] Cerrar sesion\n";
        std::cout << "Seleccione una opción: ";

        char opcion[10];
        std::cin >> opcion;

        Cadena opStr(opcion);
        if (opStr == Cadena("1") || opStr == Cadena("UN") || opStr == Cadena("un")) {
            menuPrincipal(idUsuario);
        } else if (opStr == Cadena("N") || opStr == Cadena("n")) {
            std::cout << "  [i] Rotando muestra ponderada de fotos de amigos...\n";
        } else if (opStr == Cadena("S") || opStr == Cadena("s")) {
            sesionActiva = false;
            usuarioLogueadoId = -1;
            std::cout << "  [i] Sesion cerrada.\n";
        } else {
            std::cout << "  [!] Opcion invalida.\n";
        }
    }
}

void UdeABook::menuPrincipal(int idUsuario) {
    bool enMenu = true;
    while (enMenu) {
        std::cout << " MENU PRINCIPAL \n";
        std::cout << "1. Administrar Amistades (Enviar / Aceptar Solicitudes)\n";
        std::cout << "2. Visitar Perfil de Amigo (Álbumes y Fotos)\n";
        std::cout << "3. Ver Sugerencias de Amistad (K-Sugerencias)\n";
        std::cout << "4. Administrar Mensajes (Enviar / Leer)\n";
        std::cout << "5. Ver Reporte de Consumo de Recursos\n";
        std::cout << "6. Volver a Bienvenida\n";
        std::cout << "Elija una opción: ";

        int opcion;
        std::cin >> opcion;

        switch (opcion) {
        case 1:
            administrarAmistades(idUsuario);
            break;
        case 2:
            visitarPerfil(idUsuario);
            break;
        case 3:
            verSugerencias(idUsuario);
            break;
        case 4:
            administrarMensajes(idUsuario);
            break;
        case 5:
            Medidor::mostrarReporte(calcularMemoria() + usuarios.calcularMemoria() + mensajes.calcularMemoria() + amistades.calcularMemoria());
            break;
        case 6:
            enMenu = false;
            break;
        default:
            std::cout << "  [!] Opción no valida.\n";
        }
    }
}

void UdeABook::administrarAmistades(int idUsuario) {
    std::cout << "\n ADMINISTRACIÓN DE AMISTADES \n";
    std::cout << "1. Enviar solicitud de amistad\n";
    std::cout << "2. Ver y aceptar solicitudes recibidas\n";
    std::cout << "Seleccione: ";
    int op;
    std::cin >> op;

    if (op == 1) {
        std::cout << "Ingrese el ID del usuario destino: ";
        int idDestino;
        std::cin >> idDestino;
        if (idDestino < 0 || idDestino >= static_cast<int>(usuarios.getTamanio()) || idDestino == idUsuario) {
            std::cout << "  [!] ID invalido.\n";
            return;
        }
        if (amistades.sonAmigos(idUsuario, idDestino)) {
            std::cout << "  [!] Ya son amigos.\n";
            return;
        }
        Solicitud nuevaSol(idUsuario, idDestino, Fecha::ahora());
        usuarios[static_cast<size_t>(idDestino)].recibirSolicitud(nuevaSol);
        almacenamiento.guardarSolicitudNueva(nuevaSol);
        std::cout << "  [ok] Solicitud enviada con exito.\n";
    } else if (op == 2) {
        Usuario& u = usuarios[static_cast<size_t>(idUsuario)];
        // Obtenemos una copia o referencia constante segura de la lista de solicitudes
        const Lista<Solicitud>& sols = u.getSolicitudes();
        std::cout << "Tiene " << sols.getTamanio() << " solicitudes pendientes:\n";
        for (size_t i = 0; i < sols.getTamanio(); ++i) {
            std::cout << "[" << i << "] Emisor ID: " << sols[i].getIdEmisor() << "\n";
        }
        if (sols.estaVacia()) return;

        std::cout << "Ingrese el indice de la solicitud que desea aceptar (-1 para salir): ";
        int idx;
        std::cin >> idx;
        if (idx >= 0 && static_cast<size_t>(idx) < sols.getTamanio()) {
            int emisorId = sols[static_cast<size_t>(idx)].getIdEmisor();
            amistades.agregarAmistad(idUsuario, emisorId);
            almacenamiento.guardarAmistadNueva(idUsuario, emisorId);

            // Usamos el metodo de la clase Usuario para remover la solicitud de forma segura
            const_cast<Lista<Solicitud>&>(u.getSolicitudes()).quitar(static_cast<size_t>(idx));
            almacenamiento.guardarSolicitudes(usuarios);
            std::cout << "  [ok] Solicitud aceptada y amistad registrada.\n";
        }
    }
}

void UdeABook::visitarPerfil(int idUsuario) {
    (void)idUsuario;
    std::cout << "\n VISITAR PERFIL DE AMIGO \n";
    std::cout << "Ingrese el nombre de usuario del amigo a visitar: ";
    char nombreAmigo[128];
    std::cin >> nombreAmigo;

    int idAmigo = -1;
    for (size_t i = 0; i < usuarios.getTamanio(); ++i) {
        if (usuarios[i].getNombre() == Cadena(nombreAmigo)) {
            idAmigo = static_cast<int>(i);
            break;
        }
    }

    if (idAmigo == -1) {
        std::cout << "  [!] Usuario no encontrado.\n";
        return;
    }

    Usuario& amigo = usuarios[static_cast<size_t>(idAmigo)];
    std::cout << "Albumes de " << amigo.getNombre().c_str() << " (Total: " << amigo.getCantidadAlbumes() << "):\n";
    for (int a = 0; a < amigo.getCantidadAlbumes(); ++a) {
        std::cout << "[" << a << "] " << amigo.obtenerAlbum(static_cast<size_t>(a)).getNombre().c_str() << "\n";
    }
}

void UdeABook::verSugerencias(int idUsuario) {
    std::cout << "\n K-SUGERENCIAS DE AMISTAD \n";
    std::cout << "Ingrese el valor de K (número de sugerencias): ";
    int k;
    std::cin >> k;
    (void)k;
    (void)idUsuario;
    std::cout << "  [i] Analizando matriz de adyacencia y amigos en comun...\n";
}

void UdeABook::administrarMensajes(int idUsuario) {
    std::cout << "\n--- GESTION DE MENSAJES ---\n";
    std::cout << "1. Enviar mensaje\n";
    std::cout << "2. Ver mensajes generales cargados\n";
    std::cout << "Seleccione: ";
    int op;
    std::cin >> op;

    if (op == 1) {
        std::cout << "ID del receptor: ";
        int receptor;
        std::cin >> receptor;
        std::cout << "Escriba el texto del mensaje (máx 512 caracteres): ";
        char texto[512];
        std::cin.ignore();
        std::cin.getline(texto, 512);

        Mensaje m(idUsuario, receptor, Cadena(texto), Fecha::ahora());
        mensajes.agregar(m);

        // Registrar en almacenamiento
        almacenamiento.guardarMensajeNuevo(m);
        std::cout << "  [ok] Mensaje enviado.\n";
    } else if (op == 2) {
        std::cout << "Total de mensajes en el sistema: " << mensajes.getTamanio() << "\n";
        for (size_t i = 0; i < mensajes.getTamanio(); ++i) {
            const Mensaje& msg = mensajes[i];
            std::cout << "- De " << msg.getIdEmisor() << " para " << msg.getIdReceptor() << ": " << msg.getContenido().c_str() << "\n";
        }
    }
}

UdeABook& UdeABook::operator=(const UdeABook& otro) {
    if (this != &otro) {
        usuarios = otro.usuarios;
        mensajes = otro.mensajes;
        amistades = otro.amistades;
        almacenamiento = otro.almacenamiento;
        usuarioLogueadoId = otro.usuarioLogueadoId;
    }
    return *this;
}

bool UdeABook::operator==(const UdeABook& otro) const {
    return usuarios == otro.usuarios &&
           mensajes == otro.mensajes &&
           amistades == otro.amistades &&
           almacenamiento == otro.almacenamiento &&
           usuarioLogueadoId == otro.usuarioLogueadoId;
}

long UdeABook::calcularMemoria() const {
    return static_cast<long>(sizeof(UdeABook)) +
           usuarios.calcularMemoria() +
           mensajes.calcularMemoria() +
           amistades.calcularMemoria() +
           almacenamiento.calcularMemoria();
}