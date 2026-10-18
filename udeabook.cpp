#include "udeabook.h"
#include "medidor.h"
#include <iostream>
#include <cstdlib>
#include <ctime>

static const int MAX_NOMBRE = 128;       // buffer para nombres de usuario
static const int MAX_TEXTO = 1100;       // buffer para un mensaje (el limite real es 512)
static const int TAM_SELECCION = 6;      // 3 fotos al ingresar + 3 para la actualizacion
static const int GRUPO_FOTOS = 3;


static bool esEspacio(char c) { return c == ' ' || c == '\t'; }
static bool esDigito(char c)  { return c >= '0' && c <= '9'; }

static bool esBisiesto(int anio) {
    return (anio % 4 == 0 && anio % 100 != 0) || (anio % 400 == 0);
}

static int diasDelMes(int mes, int anio) {
    if (mes == 2) return esBisiesto(anio) ? 29 : 28;
    if (mes == 4 || mes == 6 || mes == 9 || mes == 11) return 30;
    return 31;
}

static void escribirDosDigitos(int valor) {
    std::cout << (valor / 10) % 10 << valor % 10;
}

// dd/mm/aaaa hh:mm
static void imprimirFecha(const Fecha& f) {
    escribirDosDigitos(f.getDia());
    std::cout << '/';
    escribirDosDigitos(f.getMes());
    std::cout << '/' << f.getAnio() << ' ';
    escribirDosDigitos(f.getHora());
    std::cout << ':';
    escribirDosDigitos(f.getMinuto());
}

static const char* textoPrivacidad(Privacidad p) {
    if (p == PUBLICO) return "publico";
    if (p == AMIGOS) return "amigos";
    return "privado";
}

static bool interpretarFecha(const char* t, int& dia, int& mes, int& anio) {
    int valores[3];
    int i = 0;
    for (int n = 0; n < 3; n++) {
        Medidor::incrementarIteraciones();
        while (esEspacio(t[i])) { Medidor::incrementarIteraciones(); i++; }
        if (!esDigito(t[i])) return false;
        long long numero = 0;
        while (esDigito(t[i])) {
            Medidor::incrementarIteraciones();
            numero = numero * 10 + (t[i] - '0');
            if (numero > 100000) return false;
            i++;
        }
        valores[n] = static_cast<int>(numero);
        while (esEspacio(t[i])) { Medidor::incrementarIteraciones(); i++; }
        if (n < 2) {
            if (t[i] != '/') return false;
            i++;
        }
    }
    if (t[i] != '\0') return false;
    dia = valores[0];
    mes = valores[1];
    anio = valores[2];
    if (anio < 1 || anio > 9999 || mes < 1 || mes > 12) return false;
    return dia >= 1 && dia <= diasDelMes(mes, anio);
}

// Lee "dd/mm/aaaa" de la consola (repite hasta que sea valida). La hora y el
// minuto se fijan para poder usarla como inicio (00:00) o fin (23:59) de rango.
static bool leerFechaConsola(const char* mensaje, int hora, int minuto, Fecha& resultado) {
    char linea[64];
    Medidor::registrarMemoriaLocal(sizeof(linea) + 5 * sizeof(int));
    while (true) {
        Medidor::incrementarIteraciones();
        std::cout << mensaje;
        if (!UdeABook::leerLinea(linea, 64)) return false;
        int d = 0, m = 0, a = 0;
        if (interpretarFecha(linea, d, m, a)) {
            resultado = Fecha(d, m, a, hora, minuto);
            return true;
        }
        std::cout << "  [!] Fecha invalida. Use el formato dd/mm/aaaa (por ejemplo 25/12/2025).\n";
    }
}

// ---------------------------------------------------------------------------
// Entrada por consola
// ---------------------------------------------------------------------------

// Lee una linea completa. Si es mas larga que el buffer, el resto se descarta.
bool UdeABook::leerLinea(char* destino, int max) {
    if (!std::cin.getline(destino, max)) {
        if (std::cin.eof()) {
            destino[0] = '\0';
            return false;
        }
        // Linea demasiado larga: getline dejo el inicio y activo failbit
        std::cin.clear();
        char c;
        while (std::cin.get(c) && c != '\n') {
            Medidor::incrementarIteraciones();
        }
    }
    int n = 0;
    while (destino[n] != '\0') n++;
    if (n > 0 && destino[n - 1] == '\r') destino[n - 1] = '\0';
    return true;
}

// Pide un entero entre minimo y maximo (inclusive). Repite hasta que sea valido.
bool UdeABook::leerEntero(const char* mensaje, int minimo, int maximo, int& valor) {
    char buffer[32];
    Medidor::registrarMemoriaLocal(sizeof(buffer) + 6 * sizeof(int) + sizeof(long long) + sizeof(bool));
    while (true) {
        Medidor::incrementarIteraciones();
        std::cout << mensaje;
        if (!leerLinea(buffer, 32)) return false;

        int i = 0;
        while (esEspacio(buffer[i])) { Medidor::incrementarIteraciones(); i++; }
        bool negativo = false;
        if (buffer[i] == '-') { negativo = true; i++; }
        bool valido = esDigito(buffer[i]);
        long long numero = 0;
        while (valido && esDigito(buffer[i])) {
            Medidor::incrementarIteraciones();
            numero = numero * 10 + (buffer[i] - '0');
            if (numero > 1000000000LL) valido = false;
            i++;
        }
        while (valido && esEspacio(buffer[i])) { Medidor::incrementarIteraciones(); i++; }
        if (valido && buffer[i] != '\0') valido = false;
        if (!valido) {
            std::cout << "  [!] Entrada no valida. Escriba un numero entero.\n";
            continue;
        }
        if (negativo) numero = -numero;
        if (numero < minimo || numero > maximo) {
            std::cout << "  [!] El valor debe estar entre " << minimo << " y " << maximo << ".\n";
            continue;
        }
        valor = static_cast<int>(numero);
        return true;
    }
}

// ---------------------------------------------------------------------------
// Constructores, destructor y operadores
// ---------------------------------------------------------------------------
UdeABook::UdeABook() {
    fotosActualizadas = false;
}

UdeABook::UdeABook(const Cadena& subRuta, const Cadena& carpetaArchivos, int umbral)
    : almacenamiento(subRuta, carpetaArchivos, umbral) {
    fotosActualizadas = false;
}

UdeABook::UdeABook(const UdeABook& otro) {
    usuarios = otro.usuarios;
    mensajes = otro.mensajes;
    amistades = otro.amistades;
    almacenamiento = otro.almacenamiento;
    seleccionAmigos = otro.seleccionAmigos;
    fotosActualizadas = otro.fotosActualizadas;
}

UdeABook::~UdeABook() {}

UdeABook& UdeABook::operator=(const UdeABook& otro) {
    if (this != &otro) {
        usuarios = otro.usuarios;
        mensajes = otro.mensajes;
        amistades = otro.amistades;
        almacenamiento = otro.almacenamiento;
        seleccionAmigos = otro.seleccionAmigos;
        fotosActualizadas = otro.fotosActualizadas;
    }
    return *this;
}

bool UdeABook::operator==(const UdeABook& otro) const {
    return usuarios == otro.usuarios &&
           mensajes == otro.mensajes &&
           amistades == otro.amistades &&
           almacenamiento == otro.almacenamiento &&
           seleccionAmigos == otro.seleccionAmigos &&
           fotosActualizadas == otro.fotosActualizadas;
}

// Memoria de todo lo que posee. Las listas solo cuentan su arreglo, asi que se
// suma aparte lo dinamico de cada elemento (restando su parte fija, que ya esta
// contada en el arreglo de la lista, igual que hacen Album y Usuario).
long UdeABook::calcularMemoria() const {
    long total = static_cast<long>(sizeof(UdeABook));

    total += usuarios.calcularMemoria() - static_cast<long>(sizeof(Lista<Usuario>));
    for (size_t i = 0; i < usuarios.getTamanio(); i++) {
        total += usuarios[i].calcularMemoria() - static_cast<long>(sizeof(Usuario));
    }

    total += mensajes.calcularMemoria() - static_cast<long>(sizeof(Lista<Mensaje>));
    for (size_t i = 0; i < mensajes.getTamanio(); i++) {
        total += mensajes[i].calcularMemoria() - static_cast<long>(sizeof(Mensaje));
    }

    total += amistades.calcularMemoria() - static_cast<long>(sizeof(MatrizAmistad));
    total += almacenamiento.calcularMemoria() - static_cast<long>(sizeof(Almacenamiento));
    total += seleccionAmigos.calcularMemoria() - static_cast<long>(sizeof(Lista<int>));
    return total;
}


// Auxiliares


// Fecha actual. Fecha::ahora() usa time() y localtime() de la libreria estandar.
Fecha UdeABook::fechaActual() {
    Medidor::registrarFuncionExterna("time");
    Medidor::registrarFuncionExterna("localtime");
    Medidor::registrarMemoriaLocal(sizeof(Fecha));
    return Fecha::ahora();
}

int UdeABook::numeroAleatorio(int limite) {
    Medidor::registrarFuncionExterna("rand");
    if (limite <= 1) return 0;
    return std::rand() % limite;
}

void UdeABook::guardarSiCorresponde() {
    if (almacenamiento.debeGuardarse()) {
        Medidor::registrarFuncionExterna("ofstream (archivo)");
        almacenamiento.guardarPendientes(usuarios, mensajes);
    }
}

void UdeABook::guardarCambiosPendientes() {
    if (almacenamiento.getCambiosPendientes() > 0) {
        std::cout << "Guardando cambios pendientes...\n";
        Medidor::registrarFuncionExterna("ofstream (archivo)");
        almacenamiento.guardarPendientes(usuarios, mensajes);
    }
}


void UdeABook::reportarConsumo() const {
    Medidor::mostrarReporte(calcularMemoria());
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


// Carga inicial (I)
bool UdeABook::iniciarSistema() {
    Medidor::reiniciar();
    std::cout << "========================================\n";
    std::cout << "      INICIANDO SISTEMA UDEABOOK        \n";
    std::cout << "========================================\n";

    // Semilla de la libreria de aleatorios (una sola vez por ejecucion)
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    Medidor::registrarFuncionExterna("srand");
    Medidor::registrarFuncionExterna("time");

    // La carga abre los seis archivos con ifstream
    for (int i = 0; i < 6; i++) {
        Medidor::incrementarIteraciones();
        Medidor::registrarFuncionExterna("ifstream (archivo)");
    }

    if (!almacenamiento.cargarDatos(usuarios, mensajes, amistades)) {
        std::cout << "ERROR CRITICO: No se pudieron cargar los datos iniciales." << std::endl;
        return false;
    }
    seleccionAmigos = Lista<int>();
    fotosActualizadas = false;
    std::cout << "Datos cargados exitosamente en memoria: " << usuarios.getTamanio()
              << " usuarios y " << mensajes.getTamanio() << " mensajes.\n\n";
    reportarConsumo();
    return true;
}

// ---------------------------------------------------------------------------
// II. Ingreso y mejores amigos
// ---------------------------------------------------------------------------

// Prioridad de un amigo segun los amigos que tiene en comun con el usuario:
// hasta 10 -> 1; de 11 a 20 -> 2.5; mas de 20 -> 3.5. Se devuelve multiplicado
// por 2 (2, 5 y 7) para trabajar solo con enteros.
int UdeABook::pesoDeAmigo(int idUsuario, int idAmigo) const {
    int comunes = amistades.contarEnComun(idUsuario, idAmigo);
    if (comunes <= 10) return 2;
    if (comunes <= 20) return 5;
    return 7;
}

// Elige hasta 6 amigos distintos al azar, sin repetir, cada uno con su
// probabilidad proporcional a su peso. Cada eleccion recorre los pesos
// acumulados; el elegido pasa a peso 0 para no volver a salir.
void UdeABook::elegirAmigosPonderados(int idUsuario) {
    seleccionAmigos = Lista<int>();
    Lista<int> amigos = amistades.obtenerAmigos(idUsuario);
    int n = static_cast<int>(amigos.getTamanio());
    if (n == 0) return;

    int* pesos = new int[static_cast<size_t>(n)];
    long long total = 0;
    for (int i = 0; i < n; i++) {
        Medidor::incrementarIteraciones();
        pesos[i] = pesoDeAmigo(idUsuario, amigos[static_cast<size_t>(i)]);
        total += pesos[i];
    }

    int cuantos = (n < TAM_SELECCION) ? n : TAM_SELECCION;
    for (int k = 0; k < cuantos; k++) {
        Medidor::incrementarIteraciones();
        int r = numeroAleatorio(static_cast<int>(total));
        int acumulado = 0;
        int elegido = 0;
        for (int i = 0; i < n; i++) {
            Medidor::incrementarIteraciones();
            acumulado += pesos[i];
            if (r < acumulado) { elegido = i; break; }
        }
        seleccionAmigos.agregar(amigos[static_cast<size_t>(elegido)]);
        total -= pesos[elegido];
        pesos[elegido] = 0;
    }

    Medidor::registrarMemoriaLocal(amigos.calcularMemoria() +
                                   static_cast<size_t>(n) * sizeof(int) +
                                   sizeof(int*) + sizeof(long long) + 8 * sizeof(int));
    delete[] pesos;
}

void UdeABook::mostrarFotosAmigos(int desde, int hasta) const {
    int total = static_cast<int>(seleccionAmigos.getTamanio());
    for (int k = desde; k < hasta && k < total; k++) {
        Medidor::incrementarIteraciones();
        const Usuario& amigo = usuarios[static_cast<size_t>(seleccionAmigos[static_cast<size_t>(k)])];
        std::cout << "   - " << amigo.getNombre().c_str() << "  ->  " << amigo.getRutaFotoPerfil().c_str() << "\n";
    }
}

int UdeABook::iniciarSesion(const Cadena& nombre, const Cadena& clave) {
    Medidor::reiniciar();
    Medidor::registrarMemoriaLocal(sizeof(int));

    int id = buscarUsuarioPorNombre(nombre);
    if (id == -1 || !usuarios[static_cast<size_t>(id)].verificarClave(clave)) {
        std::cout << "  [!] Usuario o contrasena incorrectos.\n";
        return -1;
    }

    fotosActualizadas = false;
    elegirAmigosPonderados(id);

    std::cout << "\n  Bienvenido, " << usuarios[static_cast<size_t>(id)].getNombre().c_str() << ".\n";
    if (seleccionAmigos.estaVacia()) {
        std::cout << "  Aun no tiene amigos: no hay fotos de perfil para mostrar.\n";
    } else {
        std::cout << "  Fotos de perfil de algunos de sus amigos:\n";
        mostrarFotosAmigos(0, GRUPO_FOTOS);
    }
    reportarConsumo();
    return id;
}

bool UdeABook::puedeActualizarFotos() const {
    return !fotosActualizadas && static_cast<int>(seleccionAmigos.getTamanio()) > GRUPO_FOTOS;
}

// La actualizacion solo puede hacerse una vez: muestra los 3 amigos que
// quedaron elegidos de reserva (distintos a los primeros).
void UdeABook::actualizarFotosAmigos(int idUsuario) {
    (void)idUsuario;
    Medidor::reiniciar();
    Medidor::registrarMemoriaLocal(sizeof(int));

    if (fotosActualizadas) {
        std::cout << "  [!] Las fotos ya se actualizaron una vez; no se puede repetir.\n";
        return;
    }
    if (!puedeActualizarFotos()) {
        std::cout << "  [!] No hay mas amigos distintos para mostrar.\n";
        return;
    }
    fotosActualizadas = true;
    std::cout << "  Nuevas fotos de perfil de sus amigos:\n";
    mostrarFotosAmigos(GRUPO_FOTOS, TAM_SELECCION);
    reportarConsumo();
}

// ---------------------------------------------------------------------------
// III. Administrar amistades
// ---------------------------------------------------------------------------

// Validaciones del enunciado: no enviar a quien ya es amigo, a quien ya nos envio
// una solicitud, ni repetir una solicitud pendiente. Devuelve true si se envio.
bool UdeABook::enviarSolicitud(int idEmisor, int idDestino) {
    if (idDestino < 0 || idDestino >= static_cast<int>(usuarios.getTamanio()) || idDestino == idEmisor) {
        std::cout << "  [!] Usuario destino invalido.\n";
        return false;
    }
    if (amistades.sonAmigos(idEmisor, idDestino)) {
        std::cout << "  [!] Ya son amigos.\n";
        return false;
    }
    if (usuarios[static_cast<size_t>(idEmisor)].tieneSolicitudDe(idDestino)) {
        std::cout << "  [!] Ese usuario ya le envio una solicitud: aceptela en 'Aceptar solicitudes'.\n";
        return false;
    }
    if (usuarios[static_cast<size_t>(idDestino)].tieneSolicitudDe(idEmisor)) {
        std::cout << "  [!] Ya envio una solicitud pendiente a ese usuario.\n";
        return false;
    }

    Solicitud nueva(idEmisor, idDestino, fechaActual());
    Medidor::registrarMemoriaLocal(sizeof(Solicitud));
    Medidor::registrarFuncionExterna("fstream (archivo)");
    if (!almacenamiento.guardarSolicitudNueva(nueva)) {
        std::cout << "  [!] No se pudo guardar la solicitud.\n";
        return false;
    }
    usuarios[static_cast<size_t>(idDestino)].recibirSolicitud(nueva);
    std::cout << "  [ok] Solicitud de amistad enviada a "
              << usuarios[static_cast<size_t>(idDestino)].getNombre().c_str() << ".\n";
    return true;
}

void UdeABook::enviarSolicitudPorNombre(int idUsuario) {
    Medidor::reiniciar();
    char nombre[MAX_NOMBRE];
    Medidor::registrarMemoriaLocal(sizeof(nombre) + sizeof(int) + sizeof(Cadena));

    std::cout << "Nombre de usuario destino (vacio para cancelar): ";
    if (!leerLinea(nombre, MAX_NOMBRE) || nombre[0] == '\0') return;

    int idDestino = buscarUsuarioPorNombre(Cadena(nombre));
    if (idDestino == -1) {
        std::cout << "  [!] Usuario no encontrado.\n";
    } else {
        enviarSolicitud(idUsuario, idDestino);
    }
    reportarConsumo();
}

void UdeABook::aceptarSolicitudes(int idUsuario) {
    Medidor::reiniciar();
    Usuario& yo = usuarios[static_cast<size_t>(idUsuario)];
    const Lista<Solicitud>& pendientes = yo.getSolicitudes();
    int total = static_cast<int>(pendientes.getTamanio());
    Medidor::registrarMemoriaLocal(sizeof(Usuario*) + sizeof(void*) + 6 * sizeof(int));

    if (total == 0) {
        std::cout << "  No tiene solicitudes de amistad pendientes.\n";
        reportarConsumo();
        return;
    }
    std::cout << "Solicitudes recibidas (" << total << "):\n";
    for (int i = 0; i < total; i++) {
        Medidor::incrementarIteraciones();
        const Solicitud& s = pendientes[static_cast<size_t>(i)];
        std::cout << "  [" << (i + 1) << "] " << usuarios[static_cast<size_t>(s.getIdEmisor())].getNombre().c_str()
                  << "  (enviada el ";
        imprimirFecha(s.getFecha());
        std::cout << ")\n";
    }

    int opcion = 0;
    if (!leerEntero("Numero de la solicitud a aceptar (0 para cancelar): ", 0, total, opcion) || opcion == 0) {
        return;
    }

    int emisor = pendientes[static_cast<size_t>(opcion - 1)].getIdEmisor();
    if (amistades.agregarAmistad(idUsuario, emisor)) {
        Medidor::registrarFuncionExterna("fstream (archivo)");
        if (!almacenamiento.guardarAmistadNueva(idUsuario, emisor)) {
            std::cout << "  [!] La amistad se registro, pero no se pudo guardar en el archivo.\n";
        }
    }
    yo.quitarSolicitud(emisor);
    usuarios[static_cast<size_t>(emisor)].quitarSolicitud(idUsuario);   // por si habia una cruzada
    Medidor::registrarFuncionExterna("ofstream (archivo)");
    almacenamiento.guardarSolicitudes(usuarios);

    std::cout << "  [ok] Ahora usted y " << usuarios[static_cast<size_t>(emisor)].getNombre().c_str()
              << " son amigos.\n";
    reportarConsumo();
}

void UdeABook::administrarAmistades(int idUsuario) {
    std::cout << "\n--- ADMINISTRAR AMISTADES ---\n";
    std::cout << "1. Enviar solicitud de amistad\n";
    std::cout << "2. Aceptar solicitudes recibidas\n";
    std::cout << "0. Volver\n";
    int opcion = 0;
    if (!leerEntero("Seleccione una opcion: ", 0, 2, opcion)) return;

    if (opcion == 1) enviarSolicitudPorNombre(idUsuario);
    else if (opcion == 2) aceptarSolicitudes(idUsuario);
}

// ---------------------------------------------------------------------------
// IV. Visitar perfil de un amigo
// ---------------------------------------------------------------------------

// Carrusel circular de las fotos de un album: salir, siguiente, previa, like.
void UdeABook::recorrerAlbum(Usuario& dueno, int posAlbum) {
    Album& album = dueno.obtenerAlbum(posAlbum);
    int total = album.getCantidadFotos();
    Medidor::registrarMemoriaLocal(sizeof(Album*) + sizeof(Foto*) + 6 * sizeof(int) + sizeof(bool));

    if (total == 0) {
        std::cout << "  El album esta vacio.\n";
        return;
    }
    int actual = 0;
    bool salir = false;
    while (!salir) {
        Medidor::incrementarIteraciones();
        Foto& foto = album.obtenerFoto(actual);
        std::cout << "\n  Album \"" << album.getNombre().c_str() << "\" - foto " << (actual + 1) << " de " << total << "\n";
        std::cout << "  Ruta:   " << foto.getRuta().c_str() << "\n";
        std::cout << "  Fecha:  ";
        imprimirFecha(foto.getFechaCaptura());
        std::cout << "\n  Tamano: " << foto.getTamano() << "   Likes: " << foto.getLikes() << "\n";
        std::cout << "  1. Siguiente   2. Previa   3. Dar/quitar like   0. Salir del album\n";

        int opcion = 0;
        if (!leerEntero("  Opcion: ", 0, 3, opcion)) return;
        if (opcion == 0) {
            salir = true;
        } else if (opcion == 1) {
            actual = (actual + 1) % total;
        } else if (opcion == 2) {
            actual = (actual - 1 + total) % total;
        } else {
            int accion = 0;
            if (!leerEntero("  1. Dar like   2. Quitar like   0. Cancelar: ", 0, 2, accion)) return;
            if (accion == 1) {
                foto.darLike();
                almacenamiento.anotarCambioFotos();
                std::cout << "  [ok] Like agregado. Likes: " << foto.getLikes() << "\n";
            } else if (accion == 2) {
                if (foto.getLikes() == 0) {
                    std::cout << "  [!] La foto no tiene likes que quitar.\n";
                } else {
                    foto.quitarLike();
                    almacenamiento.anotarCambioFotos();
                    std::cout << "  [ok] Like quitado. Likes: " << foto.getLikes() << "\n";
                }
            }
            guardarSiCorresponde();
        }
    }
}

void UdeABook::visitarPerfil(int idUsuario) {
    Medidor::reiniciar();
    char nombre[MAX_NOMBRE];
    Medidor::registrarMemoriaLocal(sizeof(nombre) + sizeof(Cadena) + sizeof(Usuario*) + 8 * sizeof(int));

    std::cout << "\n--- VISITAR PERFIL DE UN AMIGO ---\n";
    std::cout << "Nombre de usuario del amigo (vacio para cancelar): ";
    if (!leerLinea(nombre, MAX_NOMBRE) || nombre[0] == '\0') return;

    int idAmigo = buscarUsuarioPorNombre(Cadena(nombre));
    if (idAmigo == -1) {
        std::cout << "  [!] Usuario no encontrado.\n";
        return;
    }
    if (!amistades.sonAmigos(idUsuario, idAmigo)) {
        std::cout << "  [!] Ese usuario no es su amigo: solo puede visitar perfiles de amigos.\n";
        return;
    }

    Usuario& amigo = usuarios[static_cast<size_t>(idAmigo)];
    int cantidad = amigo.getCantidadAlbumes();
    bool terminar = false;
    while (!terminar) {
        Medidor::incrementarIteraciones();
        // Solo se listan los albumes "publicos" o "amigos" (el visitante es amigo, no el dueno)
        const Lista<Album>& albumes = amigo.getAlbumes();
        int visibles = 0;
        std::cout << "\nAlbumes de " << amigo.getNombre().c_str() << ":\n";
        for (int a = 0; a < cantidad; a++) {
            Medidor::incrementarIteraciones();
            const Album& album = albumes[static_cast<size_t>(a)];
            if (album.esVisiblePara(false, true)) {
                std::cout << "  [" << a << "] " << album.getNombre().c_str() << "  ("
                          << textoPrivacidad(album.getPrivacidad()) << ", "
                          << album.getCantidadFotos() << " fotos)\n";
                visibles++;
            }
        }
        if (visibles == 0) {
            std::cout << "  No tiene albumes disponibles para usted.\n";
            break;
        }

        int pos = 0;
        if (!leerEntero("Numero del album a abrir (-1 para terminar la visita): ", -1, cantidad - 1, pos)) return;
        if (pos == -1) {
            terminar = true;
        } else if (!albumes[static_cast<size_t>(pos)].esVisiblePara(false, true)) {
            std::cout << "  [!] Ese album no esta disponible.\n";
        } else {
            recorrerAlbum(amigo, pos);
        }
    }
    reportarConsumo();
}

// ---------------------------------------------------------------------------
// V. Sugerencias de amistad
// ---------------------------------------------------------------------------

// Las sugerencias son los amigos de los amigos que aun no son amigos del usuario.
// Cada vez que un candidato aparece en la lista de amigos de un amigo, suma 1:
// al final ese contador es exactamente la cantidad de amigos en comun, asi que
// no hace falta calcularla por separado. Luego se queda con los K mejores
// insertandolos en un arreglo ordenado de tamano K (K <= 20).
void UdeABook::verSugerencias(int idUsuario) {
    Medidor::reiniciar();
    std::cout << "\n--- SUGERENCIAS DE AMISTAD ---\n";
    int k = 0;
    if (!leerEntero("Cantidad de sugerencias K (1 a 20): ", 1, 20, k)) return;

    int n = static_cast<int>(usuarios.getTamanio());
    Lista<int> amigos = amistades.obtenerAmigos(idUsuario);
    Lista<int> candidatos;
    int* comunes = new int[static_cast<size_t>(n)]();
    int* mejores = new int[static_cast<size_t>(k)];
    int cantidad = 0;
    long mayorTemporal = 0;

    for (size_t i = 0; i < amigos.getTamanio(); i++) {
        Medidor::incrementarIteraciones();
        Lista<int> deAmigo = amistades.obtenerAmigos(amigos[i]);
        if (deAmigo.calcularMemoria() > mayorTemporal) mayorTemporal = deAmigo.calcularMemoria();
        for (size_t j = 0; j < deAmigo.getTamanio(); j++) {
            Medidor::incrementarIteraciones();
            int c = deAmigo[j];
            if (c == idUsuario || amistades.sonAmigos(idUsuario, c)) continue;
            if (comunes[c] == 0) candidatos.agregar(c);
            comunes[c]++;
        }
    }

    for (size_t i = 0; i < candidatos.getTamanio(); i++) {
        Medidor::incrementarIteraciones();
        int c = candidatos[i];
        // Posicion que le toca: despues de los que tienen mas amigos en comun
        // (en empate, el de menor numero primero, para que el orden sea fijo)
        int pos = cantidad;
        while (pos > 0 && (comunes[mejores[pos - 1]] < comunes[c] ||
                           (comunes[mejores[pos - 1]] == comunes[c] && mejores[pos - 1] > c))) {
            Medidor::incrementarIteraciones();
            pos--;
        }
        if (pos < k) {
            int fin = (cantidad < k) ? cantidad : k - 1;
            for (int j = fin; j > pos; j--) {
                Medidor::incrementarIteraciones();
                mejores[j] = mejores[j - 1];
            }
            mejores[pos] = c;
            if (cantidad < k) cantidad++;
        }
    }

    Medidor::registrarMemoriaLocal(amigos.calcularMemoria() + candidatos.calcularMemoria() +
                                   static_cast<size_t>(mayorTemporal) +
                                   static_cast<size_t>(n) * sizeof(int) + static_cast<size_t>(k) * sizeof(int) +
                                   2 * sizeof(int*) + 10 * sizeof(int) + sizeof(long));

    if (cantidad == 0) {
        std::cout << "  No hay sugerencias: sus amigos no tienen otros amigos que usted no tenga.\n";
    } else {
        std::cout << "Sugerencias (ordenadas por amigos en comun):\n";
        for (int i = 0; i < cantidad; i++) {
            Medidor::incrementarIteraciones();
            std::cout << "  [" << (i + 1) << "] " << usuarios[static_cast<size_t>(mejores[i])].getNombre().c_str()
                      << "  - " << comunes[mejores[i]] << " amigos en comun\n";
        }
        int opcion = 0;
        if (leerEntero("Numero de la sugerencia a la que enviar solicitud (0 para ninguna): ", 0, cantidad, opcion)
            && opcion > 0) {
            enviarSolicitud(idUsuario, mejores[opcion - 1]);
        }
    }

    delete[] comunes;
    delete[] mejores;
    reportarConsumo();
}

// ---------------------------------------------------------------------------
// VI. Mensajes
// ---------------------------------------------------------------------------

// Merge sort ascendente por fecha de envio sobre un arreglo de posiciones de
// la lista central de mensajes. Es estable y hace O(n log n) comparaciones.
void UdeABook::ordenarPorFecha(int* posiciones, int cantidad) const {
    if (cantidad < 2) return;
    int* auxiliar = new int[static_cast<size_t>(cantidad)];
    int* origen = posiciones;
    int* destino = auxiliar;
    Medidor::registrarMemoriaLocal(static_cast<size_t>(cantidad) * sizeof(int) + 2 * sizeof(int*) + 9 * sizeof(int));

    for (int ancho = 1; ancho < cantidad; ancho *= 2) {
        Medidor::incrementarIteraciones();
        for (int inicio = 0; inicio < cantidad; inicio += 2 * ancho) {
            Medidor::incrementarIteraciones();
            int medio = (inicio + ancho < cantidad) ? inicio + ancho : cantidad;
            int fin = (inicio + 2 * ancho < cantidad) ? inicio + 2 * ancho : cantidad;
            int a = inicio, b = medio, k = inicio;
            while (a < medio && b < fin) {
                Medidor::incrementarIteraciones();
                const Fecha& fa = mensajes[static_cast<size_t>(origen[a])].getFechaEnvio();
                const Fecha& fb = mensajes[static_cast<size_t>(origen[b])].getFechaEnvio();
                if (fb < fa) destino[k++] = origen[b++];
                else         destino[k++] = origen[a++];
            }
            while (a < medio) { Medidor::incrementarIteraciones(); destino[k++] = origen[a++]; }
            while (b < fin)   { Medidor::incrementarIteraciones(); destino[k++] = origen[b++]; }
        }
        int* temporal = origen;
        origen = destino;
        destino = temporal;
    }
    if (origen != posiciones) {
        for (int i = 0; i < cantidad; i++) {
            Medidor::incrementarIteraciones();
            posiciones[i] = origen[i];
        }
    }
    delete[] auxiliar;
}

void UdeABook::mostrarMensaje(int posicion) const {
    const Mensaje& m = mensajes[static_cast<size_t>(posicion)];
    std::cout << "  [";
    imprimirFecha(m.getFechaEnvio());
    std::cout << "] De " << usuarios[static_cast<size_t>(m.getIdEmisor())].getNombre().c_str()
              << ": " << m.getContenido().c_str() << "\n";
}

// a. Enviar un mensaje (hasta 512 caracteres) a un amigo
void UdeABook::enviarMensaje(int idUsuario) {
    Medidor::reiniciar();
    char nombre[MAX_NOMBRE];
    char texto[MAX_TEXTO];
    Medidor::registrarMemoriaLocal(sizeof(nombre) + sizeof(texto) + sizeof(Cadena) + 4 * sizeof(int) + sizeof(bool));

    std::cout << "Nombre del amigo destinatario (vacio para cancelar): ";
    if (!leerLinea(nombre, MAX_NOMBRE) || nombre[0] == '\0') return;

    int idReceptor = buscarUsuarioPorNombre(Cadena(nombre));
    if (idReceptor == -1) {
        std::cout << "  [!] Usuario no encontrado.\n";
        return;
    }
    if (!amistades.sonAmigos(idUsuario, idReceptor)) {
        std::cout << "  [!] Solo puede enviar mensajes a sus amigos.\n";
        return;
    }

    std::cout << "Texto del mensaje (maximo " << Mensaje::getMaxCaracteres() << " caracteres): ";
    if (!leerLinea(texto, MAX_TEXTO)) return;

    bool vacio = true;
    for (int i = 0; texto[i] != '\0'; i++) {
        Medidor::incrementarIteraciones();
        if (!esEspacio(texto[i])) { vacio = false; break; }
    }
    if (vacio) {
        std::cout << "  [!] El mensaje no puede estar vacio.\n";
        return;
    }

    Mensaje nuevo(idUsuario, idReceptor, Cadena(texto), fechaActual());
    Medidor::registrarMemoriaLocal(static_cast<size_t>(nuevo.calcularMemoria()));
    if (nuevo.getContenido().estaVacia()) {      // Mensaje rechaza los textos demasiado largos
        std::cout << "  [!] El mensaje supera el maximo de " << Mensaje::getMaxCaracteres() << " caracteres.\n";
        return;
    }

    Medidor::registrarFuncionExterna("fstream (archivo)");
    if (!almacenamiento.guardarMensajeNuevo(nuevo)) {
        std::cout << "  [!] No se pudo guardar el mensaje; no se envio.\n";
        return;
    }
    mensajes.agregar(nuevo);
    int posicion = static_cast<int>(mensajes.getTamanio()) - 1;
    usuarios[static_cast<size_t>(idUsuario)].registrarEnviado(posicion);
    usuarios[static_cast<size_t>(idReceptor)].registrarRecibido(posicion);
    std::cout << "  [ok] Mensaje enviado a " << usuarios[static_cast<size_t>(idReceptor)].getNombre().c_str() << ".\n";
    reportarConsumo();
}

// b. Leer los mensajes recibidos que no se han leido, ordenados por fecha
void UdeABook::leerMensajesNuevos(int idUsuario) {
    Medidor::reiniciar();
    const Lista<int>& recibidos = usuarios[static_cast<size_t>(idUsuario)].getIdxRecibidos();
    int total = static_cast<int>(recibidos.getTamanio());
    int* nuevos = new int[static_cast<size_t>(total > 0 ? total : 1)];
    int cantidad = 0;
    Medidor::registrarMemoriaLocal(static_cast<size_t>(total > 0 ? total : 1) * sizeof(int) +
                                   sizeof(int*) + sizeof(void*) + 4 * sizeof(int));

    for (int i = 0; i < total; i++) {
        Medidor::incrementarIteraciones();
        int posicion = recibidos[static_cast<size_t>(i)];
        if (!mensajes[static_cast<size_t>(posicion)].getLeido()) {
            nuevos[cantidad++] = posicion;
        }
    }

    if (cantidad == 0) {
        std::cout << "  No tiene mensajes nuevos.\n";
    } else {
        ordenarPorFecha(nuevos, cantidad);
        std::cout << "Mensajes nuevos (" << cantidad << "):\n";
        for (int i = 0; i < cantidad; i++) {
            Medidor::incrementarIteraciones();
            mostrarMensaje(nuevos[i]);
        }
        // Despues de mostrarlos se marcan como leidos
        for (int i = 0; i < cantidad; i++) {
            Medidor::incrementarIteraciones();
            mensajes[static_cast<size_t>(nuevos[i])].marcarLeido();
            almacenamiento.anotarCambioMensajes();
        }
        guardarSiCorresponde();
    }
    delete[] nuevos;
    reportarConsumo();
}

// c. Mensajes recibidos entre dos fechas (ambas inclusive)
void UdeABook::buscarMensajesPorFechas(int idUsuario) {
    Medidor::reiniciar();
    Fecha inicio, fin;
    Medidor::registrarMemoriaLocal(2 * sizeof(Fecha));

    std::cout << "Rango de fechas (ambas inclusive).\n";
    if (!leerFechaConsola("Fecha de inicio (dd/mm/aaaa): ", 0, 0, inicio)) return;
    if (!leerFechaConsola("Fecha de fin    (dd/mm/aaaa): ", 23, 59, fin)) return;
    if (fin < inicio) {
        std::cout << "  [!] La fecha de fin es anterior a la de inicio.\n";
        return;
    }

    const Lista<int>& recibidos = usuarios[static_cast<size_t>(idUsuario)].getIdxRecibidos();
    int total = static_cast<int>(recibidos.getTamanio());
    int* encontrados = new int[static_cast<size_t>(total > 0 ? total : 1)];
    int cantidad = 0;
    Medidor::registrarMemoriaLocal(static_cast<size_t>(total > 0 ? total : 1) * sizeof(int) +
                                   sizeof(int*) + sizeof(void*) + 4 * sizeof(int));

    for (int i = 0; i < total; i++) {
        Medidor::incrementarIteraciones();
        int posicion = recibidos[static_cast<size_t>(i)];
        const Fecha& f = mensajes[static_cast<size_t>(posicion)].getFechaEnvio();
        if (inicio <= f && f <= fin) {
            encontrados[cantidad++] = posicion;
        }
    }

    if (cantidad == 0) {
        std::cout << "  No hay mensajes recibidos en ese rango.\n";
    } else {
        ordenarPorFecha(encontrados, cantidad);
        std::cout << "Mensajes recibidos en el rango (" << cantidad << "):\n";
        for (int i = 0; i < cantidad; i++) {
            Medidor::incrementarIteraciones();
            mostrarMensaje(encontrados[i]);
        }
    }
    delete[] encontrados;
    reportarConsumo();
}

void UdeABook::administrarMensajes(int idUsuario) {
    std::cout << "\n--- ADMINISTRAR MENSAJES ---\n";
    std::cout << "1. Enviar mensaje\n";
    std::cout << "2. Leer mensajes nuevos\n";
    std::cout << "3. Buscar mensajes por rango de fechas\n";
    std::cout << "0. Volver\n";
    int opcion = 0;
    if (!leerEntero("Seleccione una opcion: ", 0, 3, opcion)) return;

    if (opcion == 1) enviarMensaje(idUsuario);
    else if (opcion == 2) leerMensajesNuevos(idUsuario);
    else if (opcion == 3) buscarMensajesPorFechas(idUsuario);
}