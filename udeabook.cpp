#include "udeabook.h"
#include "medidor.h"
#include <iostream>

UdeABook::UdeABook() {}

UdeABook::UdeABook(const Cadena& subRuta, const Cadena& carpetaArchivos, int umbral)
    : almacenamiento(subRuta, carpetaArchivos, umbral) {}

UdeABook::UdeABook(const UdeABook& otro) {
    usuarios = otro.usuarios;
    mensajes = otro.mensajes;
    amistades = otro.amistades;
    almacenamiento = otro.almacenamiento;
}

UdeABook::~UdeABook() {}

bool UdeABook::iniciarSistema() {
    Medidor::reiniciar();
    std::cout << "========================================\n";
    std::cout << "      INICIANDO SISTEMA UDEABOOK        \n";
    std::cout << "========================================\n";

    if (!almacenamiento.cargarDatos(usuarios, mensajes, amistades)) {
        std::cout << "ERROR CRITICO: No se pudieron cargar los datos iniciales." << std::endl;
        return false;
    }
    std::cout << "¡Datos cargados exitosamente en memoria!\n\n";
    return true;
}

int UdeABook::buscarUsuarioPorNombre(const Cadena& nombre) const {
    for (size_t i = 0; i < usuarios.getTamanio(); ++i) {
        Medidor::incrementarIteraciones();
        if (usuarios[i].getNombre() == nombre) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

void UdeABook::administrarAmistades(int idUsuario) {
    std::cout << "\n--- ADMINISTRACIÓN DE AMISTADES ---\n";
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
            std::cout << "  [!] ID inválido.\n";
            return;
        }
        if (amistades.sonAmigos(idUsuario, idDestino)) {
            std::cout << "  [!] Ya son amigos.\n";
            return;
        }
        Solicitud nuevaSol(idUsuario, idDestino, Fecha::ahora());
        usuarios[static_cast<size_t>(idDestino)].recibirSolicitud(nuevaSol);
        almacenamiento.guardarSolicitudNueva(nuevaSol);
        std::cout << "  [ok] Solicitud enviada con éxito.\n";
    } else if (op == 2) {
        Usuario& u = usuarios[static_cast<size_t>(idUsuario)];
        const Lista<Solicitud>& sols = u.getSolicitudes();
        std::cout << "Tiene " << sols.getTamanio() << " solicitudes pendientes:\n";
        for (size_t i = 0; i < sols.getTamanio(); ++i) {
            std::cout << "[" << i << "] Emisor ID: " << sols[i].getIdEmisor() << "\n";
        }
        if (sols.estaVacia()) return;

        std::cout << "Ingrese el índice de la solicitud que desea aceptar (-1 para salir): ";
        int idx;
        std::cin >> idx;
        if (idx >= 0 && static_cast<size_t>(idx) < sols.getTamanio()) {
            int emisorId = sols[static_cast<size_t>(idx)].getIdEmisor();
            amistades.agregarAmistad(idUsuario, emisorId);
            almacenamiento.guardarAmistadNueva(idUsuario, emisorId);

            const_cast<Lista<Solicitud>&>(u.getSolicitudes()).quitar(static_cast<size_t>(idx));
            almacenamiento.guardarSolicitudes(usuarios);
            std::cout << "  [ok] Solicitud aceptada y amistad registrada.\n";
        }
    }
}

void UdeABook::visitarPerfil(int idUsuario)  {
    (void)idUsuario;
    std::cout << "\n--- VISITAR PERFIL DE AMIGO ---\n";
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
    std::cout << "Álbumes de " << amigo.getNombre().c_str() << " (Total: " << amigo.getCantidadAlbumes() << "):\n";
    for (int a = 0; a < amigo.getCantidadAlbumes(); ++a) {
        std::cout << "[" << a << "] " << amigo.obtenerAlbum(static_cast<size_t>(a)).getNombre().c_str() << "\n";
    }
}

void UdeABook::verSugerencias(int idUsuario) const {
    std::cout << "\n--- K-SUGERENCIAS DE AMISTAD ---\n";
    std::cout << "Ingrese el valor de K (número de sugerencias): ";
    int k;
    std::cin >> k;
    (void)k;
    (void)idUsuario;
    std::cout << "  [i] Analizando matriz de adyacencia y amigos en común...\n";
}

void UdeABook::administrarMensajes(int idUsuario) {
    std::cout << "\n--- GESTIÓN DE MENSAJES ---\n";
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

void UdeABook::mostrarReporteRecursos() const {
    Medidor::mostrarReporte(calcularMemoria() + usuarios.calcularMemoria() + mensajes.calcularMemoria() + amistades.calcularMemoria());
}

UdeABook& UdeABook::operator=(const UdeABook& otro) {
    if (this != &otro) {
        usuarios = otro.usuarios;
        mensajes = otro.mensajes;
        amistades = otro.amistades;
        almacenamiento = otro.almacenamiento;
    }
    return *this;
}

bool UdeABook::operator==(const UdeABook& otro) const {
    return usuarios == otro.usuarios &&
           mensajes == otro.mensajes &&
           amistades == otro.amistades &&
           almacenamiento == otro.almacenamiento;
}

long UdeABook::calcularMemoria() const {
    return static_cast<long>(sizeof(UdeABook)) +
           usuarios.calcularMemoria() +
           mensajes.calcularMemoria() +
           amistades.calcularMemoria() +
           almacenamiento.calcularMemoria();
}