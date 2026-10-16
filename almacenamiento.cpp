#include "almacenamiento.h"
#include "medidor.h"
#include <iostream>

static const int MAX_RUTA = 512;
static const int MAX_LINEA = 1100;      // un mensaje de 512 caracteres + sus campos
static const int MAX_CORTO = 128;       // campos cortos (nombres, correos, fechas...)
static const int MAX_TEXTO = 700;       // campo del texto de un mensaje

static bool esDigito(char c) { return c >= '0' && c <= '9'; }


static int aEntero(const char* t, bool& ok) {
    ok = false;
    int i = 0;
    long long valor = 0;
    if (!esDigito(t[i])) return 0;
    while (esDigito(t[i])) {
        valor = valor * 10 + (t[i] - '0');
        if (valor > 2000000000LL) return 0;
        i++;
    }
    if (t[i] != '\0') return 0;
    ok = true;
    return static_cast<int>(valor);
}


static const char* tomarCampo(const char* pos, char* destino, int max) {
    int i = 0;
    while (*pos != '\0' && *pos != '|') {
        if (i < max - 1) destino[i++] = *pos;
        pos++;
    }
    destino[i] = '\0';
    if (*pos == '|') return pos + 1;
    return nullptr;
}

// Copia todo lo que queda de la linea (el texto del mensaje puede contener '|')
static void tomarResto(const char* pos, char* destino, int max) {
    int i = 0;
    while (*pos != '\0' && i < max - 1) {
        destino[i++] = *pos++;
    }
    destino[i] = '\0';
}

// Lee "dd/mm/aaaa" o "dd/mm/aaaa hh:mm". Devuelve false si no es una fecha valida.
static bool leerFecha(const char* t, Fecha& resultado) {
    int v[5];
    int n = 0;
    int i = 0;
    while (t[i] != '\0' && n < 5) {
        while (t[i] != '\0' && !esDigito(t[i])) i++;
        if (t[i] == '\0') break;
        int numero = 0;
        while (esDigito(t[i])) {
            numero = numero * 10 + (t[i] - '0');
            i++;
        }
        v[n++] = numero;
    }
    if (n != 3 && n != 5) return false;
    int hora = (n == 5) ? v[3] : 0;
    int minuto = (n == 5) ? v[4] : 0;
    Fecha f(v[0], v[1], v[2], hora, minuto);
    // Los setters de Fecha ignoran valores fuera de rango: se comprueba que se aplicaron
    if (f.getDia() != v[0] || f.getMes() != v[1] || f.getAnio() != v[2] ||
        f.getHora() != hora || f.getMinuto() != minuto) {
        return false;
    }
    resultado = f;
    return true;
}

// Agrega texto al final de un arreglo de char que se va armando
static void agregarTexto(char* destino, int& pos, int max, const char* texto) {
    for (int i = 0; texto[i] != '\0' && pos < max - 1; i++) {
        destino[pos++] = texto[i];
    }
    destino[pos] = '\0';
}

// Lee la siguiente linea sin el salto de linea final ('\n' o '\r\n')
static bool leerLinea(std::istream& f, char* linea) {
    if (!f.getline(linea, MAX_LINEA)) return false;
    int n = 0;
    while (linea[n] != '\0') n++;
    if (n > 0 && linea[n - 1] == '\r') linea[n - 1] = '\0';
    return true;
}

static void errorArchivo(const char* archivo, int numeroLinea, const char* motivo) {
    std::cout << "  ERROR en " << archivo << ", linea " << numeroLinea << ": " << motivo << std::endl;
}

// Escritura: numero con ceros a la izquierda
static void escribirNumero(std::ostream& out, int valor, int digitos) {
    char t[16];
    int n = 0;
    if (valor < 0) valor = 0;
    for (int d = 0; d < digitos; d++) {
        t[n++] = static_cast<char>('0' + valor % 10);
        valor /= 10;
    }
    while (n > 0) out << t[--n];
}

static void escribirCantidad(std::ostream& out, int cantidad) {
    escribirNumero(out, cantidad, 10);
    out << '\n';
}

static void escribirFecha(std::ostream& out, const Fecha& f, bool conHora) {
    escribirNumero(out, f.getDia(), 2);
    out << '/';
    escribirNumero(out, f.getMes(), 2);
    out << '/';
    escribirNumero(out, f.getAnio(), 4);
    if (conHora) {
        out << ' ';
        escribirNumero(out, f.getHora(), 2);
        out << ':';
        escribirNumero(out, f.getMinuto(), 2);
    }
}

// Nombre de una foto a partir de su ruta: lo que hay entre la ultima '/' y el '.'
static void extraerNombre(const Cadena& ruta, char* destino, int max) {
    const char* r = ruta.c_str();
    int inicio = 0;
    for (int i = 0; r[i] != '\0'; i++) {
        if (r[i] == '/') inicio = i + 1;
    }
    int n = 0;
    for (int i = inicio; r[i] != '\0' && r[i] != '.' && n < max - 1; i++) {
        destino[n++] = r[i];
    }
    destino[n] = '\0';
}

static void escribirRegistroMensaje(std::ostream& out, const Mensaje& m) {
    out << m.getIdEmisor() << '|' << m.getIdReceptor() << '|';
    escribirFecha(out, m.getFechaEnvio(), true);
    out << '|' << (m.getLeido() ? 1 : 0) << '|' << m.getContenido().c_str() << '\n';
}

static void escribirRegistroSolicitud(std::ostream& out, const Solicitud& s) {
    out << s.getIdEmisor() << '|' << s.getIdReceptor() << '|';
    escribirFecha(out, s.getFecha(), true);
    out << '\n';
}

Almacenamiento::Almacenamiento() {
    cambiosPendientes = 0;
    umbralCambios = 20;
    mensajesModificados = false;
    fotosModificadas = false;
}

Almacenamiento::Almacenamiento(const Cadena& subRuta, const Cadena& carpetaArchivos, int umbral) {
    cambiosPendientes = 0;
    umbralCambios = 20;
    mensajesModificados = false;
    fotosModificadas = false;
    subRutaFija = subRuta;
    rutaArchivos = carpetaArchivos;
    setUmbralCambios(umbral);
}

Almacenamiento::Almacenamiento(const Almacenamiento& otro) {
    subRutaFija = otro.subRutaFija;
    rutaArchivos = otro.rutaArchivos;
    cambiosPendientes = otro.cambiosPendientes;
    umbralCambios = otro.umbralCambios;
    mensajesModificados = otro.mensajesModificados;
    fotosModificadas = otro.fotosModificadas;
}

Almacenamiento::~Almacenamiento() {
}

// ---------------------------------------------------------------------------
// Getters y setters
// ---------------------------------------------------------------------------
const Cadena& Almacenamiento::getSubRutaFija() const  { return subRutaFija; }
const Cadena& Almacenamiento::getRutaArchivos() const { return rutaArchivos; }
int Almacenamiento::getCambiosPendientes() const      { return cambiosPendientes; }
int Almacenamiento::getUmbralCambios() const          { return umbralCambios; }
bool Almacenamiento::getMensajesModificados() const   { return mensajesModificados; }
bool Almacenamiento::getFotosModificadas() const      { return fotosModificadas; }

void Almacenamiento::setSubRutaFija(const Cadena& nuevaSubRuta) { subRutaFija = nuevaSubRuta; }
void Almacenamiento::setRutaArchivos(const Cadena& nuevaRuta)   { rutaArchivos = nuevaRuta; }
void Almacenamiento::setMensajesModificados(bool valor)         { mensajesModificados = valor; }
void Almacenamiento::setFotosModificadas(bool valor)            { fotosModificadas = valor; }

void Almacenamiento::setCambiosPendientes(int nuevosCambios) {
    if (nuevosCambios >= 0) {
        cambiosPendientes = nuevosCambios;
    }
}

void Almacenamiento::setUmbralCambios(int nuevoUmbral) {
    if (nuevoUmbral > 0) {
        umbralCambios = nuevoUmbral;
    }
}

// ---------------------------------------------------------------------------
// Auxiliares privados
// ---------------------------------------------------------------------------

// Arma "carpeta/archivo" en destino (agrega la '/' si la carpeta no la trae)
void Almacenamiento::rutaDe(const char* nombreArchivo, char* destino, int max) const {
    int pos = 0;
    destino[0] = '\0';
    agregarTexto(destino, pos, max, rutaArchivos.c_str());
    if (pos > 0 && destino[pos - 1] != '/' && destino[pos - 1] != '\\') {
        agregarTexto(destino, pos, max, "/");
    }
    agregarTexto(destino, pos, max, nombreArchivo);
}

// Abre un archivo existente para agregarle un registro al final. Suma 1 al
// contador de registros del encabezado (tiene ancho fijo, asi se reescribe sin
// mover nada) y deja el archivo listo para escribir al final.
bool Almacenamiento::abrirParaAgregar(const char* nombreArchivo, std::fstream& archivo) const {
    char ruta[MAX_RUTA];
    rutaDe(nombreArchivo, ruta, MAX_RUTA);
    archivo.open(ruta, std::ios::in | std::ios::out);
    if (!archivo.is_open()) {
        std::cout << "  ERROR: no se pudo abrir " << ruta << std::endl;
        return false;
    }
    char linea[64];
    if (!leerLinea(archivo, linea)) return false;
    bool ok;
    int cantidad = aEntero(linea, ok);
    if (!ok) {
        std::cout << "  ERROR: encabezado invalido en " << ruta << std::endl;
        return false;
    }
    archivo.clear();
    archivo.seekp(0);
    escribirCantidad(archivo, cantidad + 1);
    archivo.seekp(0, std::ios::end);
    return archivo.good();
}

// /users/<sub-ruta>/<usuario>/albums/<album>/<foto>.png
// Si album esta vacio: /users/<sub-ruta>/<usuario>/albums/<foto>.png
Cadena Almacenamiento::construirRutaFoto(const Cadena& usuario, const Cadena& album, const Cadena& foto) const {
    char ruta[MAX_RUTA];
    int pos = 0;
    ruta[0] = '\0';
    agregarTexto(ruta, pos, MAX_RUTA, "/users/");
    if (!subRutaFija.estaVacia()) {
        agregarTexto(ruta, pos, MAX_RUTA, subRutaFija.c_str());
        agregarTexto(ruta, pos, MAX_RUTA, "/");
    }
    agregarTexto(ruta, pos, MAX_RUTA, usuario.c_str());
    agregarTexto(ruta, pos, MAX_RUTA, "/albums/");
    if (!album.estaVacia()) {
        agregarTexto(ruta, pos, MAX_RUTA, album.c_str());
        agregarTexto(ruta, pos, MAX_RUTA, "/");
    }
    agregarTexto(ruta, pos, MAX_RUTA, foto.c_str());
    agregarTexto(ruta, pos, MAX_RUTA, ".png");
    return Cadena(ruta);
}

// /users/<sub-ruta>/<usuario>/albums/<album>
Cadena Almacenamiento::construirRutaAlbum(const Cadena& usuario, const Cadena& album) const {
    char ruta[MAX_RUTA];
    int pos = 0;
    ruta[0] = '\0';
    agregarTexto(ruta, pos, MAX_RUTA, "/users/");
    if (!subRutaFija.estaVacia()) {
        agregarTexto(ruta, pos, MAX_RUTA, subRutaFija.c_str());
        agregarTexto(ruta, pos, MAX_RUTA, "/");
    }
    agregarTexto(ruta, pos, MAX_RUTA, usuario.c_str());
    agregarTexto(ruta, pos, MAX_RUTA, "/albums/");
    agregarTexto(ruta, pos, MAX_RUTA, album.c_str());
    return Cadena(ruta);
}

// ---------------------------------------------------------------------------
// Cargadores. Todos: abren el archivo, leen la cantidad de registros y luego
// un registro por linea. Cada registro se enlaza en un solo paso porque
// referencia a los usuarios por su numero.
// ---------------------------------------------------------------------------

// usuarios.txt: nombre|contrasena|nacimiento|correo|ciudad|pais|inscripcion
bool Almacenamiento::cargarUsuarios(Lista<Usuario>& usuarios) {
    char ruta[MAX_RUTA];
    rutaDe("usuarios.txt", ruta, MAX_RUTA);
    std::ifstream f(ruta);
    if (!f.is_open()) {
        std::cout << "  ERROR: no se pudo abrir " << ruta << std::endl;
        return false;
    }
    char linea[MAX_LINEA];
    bool ok;
    if (!leerLinea(f, linea)) return false;
    int total = aEntero(linea, ok);
    if (!ok) { errorArchivo("usuarios.txt", 1, "cantidad de registros invalida"); return false; }

    for (int n = 0; n < total; n++) {
        Medidor::incrementarIteraciones();
        if (!leerLinea(f, linea)) { errorArchivo("usuarios.txt", n + 2, "faltan registros"); return false; }
        char nombre[MAX_CORTO], clave[MAX_CORTO], nac[MAX_CORTO], correo[MAX_CORTO], ciudad[MAX_CORTO], pais[MAX_CORTO], insc[MAX_CORTO];
        const char* p = tomarCampo(linea, nombre, MAX_CORTO);
        if (p) p = tomarCampo(p, clave, MAX_CORTO);
        if (p) p = tomarCampo(p, nac, MAX_CORTO);
        if (p) p = tomarCampo(p, correo, MAX_CORTO);
        if (p) p = tomarCampo(p, ciudad, MAX_CORTO);
        if (p) p = tomarCampo(p, pais, MAX_CORTO);
        if (p) tomarCampo(p, insc, MAX_CORTO);
        Fecha fNac, fInsc;
        if (!p || nombre[0] == '\0' || !leerFecha(nac, fNac) || !leerFecha(insc, fInsc)) {
            errorArchivo("usuarios.txt", n + 2, "registro mal formado");
            return false;
        }
        Usuario u(n, Cadena(nombre), Cadena(correo), fNac);
        u.setContrasena(Cadena(clave));
        u.setCiudad(Cadena(ciudad));
        u.setPais(Cadena(pais));
        u.setFechaInscripcion(fInsc);
        u.setRutaFotoPerfil(construirRutaFoto(Cadena(nombre), Cadena(), Cadena("profile")));
        usuarios.agregar(u);
    }
    std::cout << "  usuarios.txt: " << total << " registros" << std::endl;
    return true;
}

// amistades.txt: idA|idB
bool Almacenamiento::cargarAmistades(MatrizAmistad& amistades, int numUsuarios) {
    char ruta[MAX_RUTA];
    rutaDe("amistades.txt", ruta, MAX_RUTA);
    std::ifstream f(ruta);
    if (!f.is_open()) {
        std::cout << "  ERROR: no se pudo abrir " << ruta << std::endl;
        return false;
    }
    char linea[MAX_LINEA];
    bool ok;
    if (!leerLinea(f, linea)) return false;
    int total = aEntero(linea, ok);
    if (!ok) { errorArchivo("amistades.txt", 1, "cantidad de registros invalida"); return false; }

    amistades.reiniciar(numUsuarios);
    for (int n = 0; n < total; n++) {
        Medidor::incrementarIteraciones();
        if (!leerLinea(f, linea)) { errorArchivo("amistades.txt", n + 2, "faltan registros"); return false; }
        char a[MAX_CORTO], b[MAX_CORTO];
        const char* p = tomarCampo(linea, a, MAX_CORTO);
        if (p) tomarCampo(p, b, MAX_CORTO);
        bool okA, okB;
        int idA = aEntero(a, okA);
        int idB = aEntero(b, okB);
        if (!p || !okA || !okB || !amistades.agregarAmistad(idA, idB)) {
            errorArchivo("amistades.txt", n + 2, "amistad invalida o repetida");
            return false;
        }
    }
    std::cout << "  amistades.txt: " << total << " registros" << std::endl;
    return true;
}

// solicitudes.txt: idEmisor|idReceptor|fecha
bool Almacenamiento::cargarSolicitudes(Lista<Usuario>& usuarios) {
    char ruta[MAX_RUTA];
    rutaDe("solicitudes.txt", ruta, MAX_RUTA);
    std::ifstream f(ruta);
    if (!f.is_open()) {
        std::cout << "  ERROR: no se pudo abrir " << ruta << std::endl;
        return false;
    }
    char linea[MAX_LINEA];
    bool ok;
    if (!leerLinea(f, linea)) return false;
    int total = aEntero(linea, ok);
    if (!ok) { errorArchivo("solicitudes.txt", 1, "cantidad de registros invalida"); return false; }

    int n_usuarios = static_cast<int>(usuarios.getTamanio());
    for (int n = 0; n < total; n++) {
        Medidor::incrementarIteraciones();
        if (!leerLinea(f, linea)) { errorArchivo("solicitudes.txt", n + 2, "faltan registros"); return false; }
        char e[MAX_CORTO], r[MAX_CORTO], fe[MAX_CORTO];
        const char* p = tomarCampo(linea, e, MAX_CORTO);
        if (p) p = tomarCampo(p, r, MAX_CORTO);
        if (p) tomarCampo(p, fe, MAX_CORTO);
        bool okE, okR;
        int emisor = aEntero(e, okE);
        int receptor = aEntero(r, okR);
        Fecha fecha;
        if (!p || !okE || !okR || emisor >= n_usuarios || receptor >= n_usuarios || !leerFecha(fe, fecha)) {
            errorArchivo("solicitudes.txt", n + 2, "registro mal formado");
            return false;
        }
        usuarios[static_cast<size_t>(receptor)].recibirSolicitud(Solicitud(emisor, receptor, fecha));
    }
    std::cout << "  solicitudes.txt: " << total << " registros" << std::endl;
    return true;
}

// mensajes.txt: idEmisor|idReceptor|fecha|leido|texto
bool Almacenamiento::cargarMensajes(Lista<Usuario>& usuarios, Lista<Mensaje>& mensajes) {
    char ruta[MAX_RUTA];
    rutaDe("mensajes.txt", ruta, MAX_RUTA);
    std::ifstream f(ruta);
    if (!f.is_open()) {
        std::cout << "  ERROR: no se pudo abrir " << ruta << std::endl;
        return false;
    }
    char linea[MAX_LINEA];
    bool ok;
    if (!leerLinea(f, linea)) return false;
    int total = aEntero(linea, ok);
    if (!ok) { errorArchivo("mensajes.txt", 1, "cantidad de registros invalida"); return false; }

    int n_usuarios = static_cast<int>(usuarios.getTamanio());
    for (int n = 0; n < total; n++) {
        Medidor::incrementarIteraciones();
        if (!leerLinea(f, linea)) { errorArchivo("mensajes.txt", n + 2, "faltan registros"); return false; }
        char e[MAX_CORTO], r[MAX_CORTO], fe[MAX_CORTO], le[MAX_CORTO], texto[MAX_TEXTO];
        const char* p = tomarCampo(linea, e, MAX_CORTO);
        if (p) p = tomarCampo(p, r, MAX_CORTO);
        if (p) p = tomarCampo(p, fe, MAX_CORTO);
        if (p) p = tomarCampo(p, le, MAX_CORTO);
        if (p) tomarResto(p, texto, MAX_TEXTO);
        bool okE, okR, okL;
        int emisor = aEntero(e, okE);
        int receptor = aEntero(r, okR);
        int leido = aEntero(le, okL);
        Fecha fecha;
        if (!p || !okE || !okR || !okL || leido > 1 || emisor >= n_usuarios || receptor >= n_usuarios || !leerFecha(fe, fecha)) {
            errorArchivo("mensajes.txt", n + 2, "registro mal formado");
            return false;
        }
        Mensaje m(emisor, receptor, Cadena(texto), fecha);
        if (m.getContenido().estaVacia()) {
            errorArchivo("mensajes.txt", n + 2, "texto vacio o de mas de 512 caracteres");
            return false;
        }
        m.setLeido(leido == 1);
        mensajes.agregar(m);
        int posicion = static_cast<int>(mensajes.getTamanio()) - 1;
        usuarios[static_cast<size_t>(emisor)].registrarEnviado(posicion);
        usuarios[static_cast<size_t>(receptor)].registrarRecibido(posicion);
    }
    std::cout << "  mensajes.txt: " << total << " registros" << std::endl;
    return true;
}

// albumes.txt: idUsuario|nombreAlbum|privacidad
bool Almacenamiento::cargarAlbumes(Lista<Usuario>& usuarios) {
    char ruta[MAX_RUTA];
    rutaDe("albumes.txt", ruta, MAX_RUTA);
    std::ifstream f(ruta);
    if (!f.is_open()) {
        std::cout << "  ERROR: no se pudo abrir " << ruta << std::endl;
        return false;
    }
    char linea[MAX_LINEA];
    bool ok;
    if (!leerLinea(f, linea)) return false;
    int total = aEntero(linea, ok);
    if (!ok) { errorArchivo("albumes.txt", 1, "cantidad de registros invalida"); return false; }

    int n_usuarios = static_cast<int>(usuarios.getTamanio());
    for (int n = 0; n < total; n++) {
        Medidor::incrementarIteraciones();
        if (!leerLinea(f, linea)) { errorArchivo("albumes.txt", n + 2, "faltan registros"); return false; }
        char u[MAX_CORTO], nombre[MAX_CORTO], pr[MAX_CORTO];
        const char* p = tomarCampo(linea, u, MAX_CORTO);
        if (p) p = tomarCampo(p, nombre, MAX_CORTO);
        if (p) tomarCampo(p, pr, MAX_CORTO);
        bool okU, okP;
        int idUsuario = aEntero(u, okU);
        int privacidad = aEntero(pr, okP);
        if (!p || !okU || !okP || idUsuario >= n_usuarios || privacidad > 2 || nombre[0] == '\0') {
            errorArchivo("albumes.txt", n + 2, "registro mal formado");
            return false;
        }
        Usuario& dueno = usuarios[static_cast<size_t>(idUsuario)];
        Album album(Cadena(nombre), construirRutaAlbum(dueno.getNombre(), Cadena(nombre)), static_cast<Privacidad>(privacidad));
        dueno.agregarAlbum(album);
    }
    std::cout << "  albumes.txt: " << total << " registros" << std::endl;
    return true;
}

// fotos.txt: idUsuario|posicionAlbum|fecha|tamano|nombreFoto|likes
bool Almacenamiento::cargarFotos(Lista<Usuario>& usuarios) {
    char ruta[MAX_RUTA];
    rutaDe("fotos.txt", ruta, MAX_RUTA);
    std::ifstream f(ruta);
    if (!f.is_open()) {
        std::cout << "  ERROR: no se pudo abrir " << ruta << std::endl;
        return false;
    }
    char linea[MAX_LINEA];
    bool ok;
    if (!leerLinea(f, linea)) return false;
    int total = aEntero(linea, ok);
    if (!ok) { errorArchivo("fotos.txt", 1, "cantidad de registros invalida"); return false; }

    int n_usuarios = static_cast<int>(usuarios.getTamanio());
    for (int n = 0; n < total; n++) {
        Medidor::incrementarIteraciones();
        if (!leerLinea(f, linea)) { errorArchivo("fotos.txt", n + 2, "faltan registros"); return false; }
        char u[MAX_CORTO], a[MAX_CORTO], fe[MAX_CORTO], ta[MAX_CORTO], nombre[MAX_CORTO], li[MAX_CORTO];
        const char* p = tomarCampo(linea, u, MAX_CORTO);
        if (p) p = tomarCampo(p, a, MAX_CORTO);
        if (p) p = tomarCampo(p, fe, MAX_CORTO);
        if (p) p = tomarCampo(p, ta, MAX_CORTO);
        if (p) p = tomarCampo(p, nombre, MAX_CORTO);
        if (p) tomarCampo(p, li, MAX_CORTO);
        bool okU, okA, okT, okL;
        int idUsuario = aEntero(u, okU);
        int posAlbum = aEntero(a, okA);
        int tamano = aEntero(ta, okT);
        int likes = aEntero(li, okL);
        Fecha fecha;
        if (!p || !okU || !okA || !okT || !okL || idUsuario >= n_usuarios || nombre[0] == '\0' || !leerFecha(fe, fecha)) {
            errorArchivo("fotos.txt", n + 2, "registro mal formado");
            return false;
        }
        Usuario& dueno = usuarios[static_cast<size_t>(idUsuario)];
        if (posAlbum >= dueno.getCantidadAlbumes()) {
            errorArchivo("fotos.txt", n + 2, "el album no existe");
            return false;
        }
        Album& album = dueno.obtenerAlbum(posAlbum);
        Foto foto(fecha, tamano, construirRutaFoto(dueno.getNombre(), album.getNombre(), Cadena(nombre)), likes);
        album.agregarFoto(foto);
    }
    std::cout << "  fotos.txt: " << total << " registros" << std::endl;
    return true;
}

// Carga completa

bool Almacenamiento::cargarDatos(Lista<Usuario>& usuarios, Lista<Mensaje>& mensajes, MatrizAmistad& amistades) {
    std::cout << "Cargando datos desde " << rutaArchivos.c_str() << " ..." << std::endl;
    usuarios = Lista<Usuario>();
    mensajes = Lista<Mensaje>();
    if (!cargarUsuarios(usuarios)) return false;
    if (!cargarAmistades(amistades, static_cast<int>(usuarios.getTamanio()))) return false;
    if (!cargarSolicitudes(usuarios)) return false;
    if (!cargarMensajes(usuarios, mensajes)) return false;
    if (!cargarAlbumes(usuarios)) return false;
    if (!cargarFotos(usuarios)) return false;
    cambiosPendientes = 0;
    mensajesModificados = false;
    fotosModificadas = false;
    std::cout << "Carga terminada." << std::endl;
    return true;
}

bool Almacenamiento::guardarMensajeNuevo(const Mensaje& mensaje) {
    std::fstream f;
    if (!abrirParaAgregar("mensajes.txt", f)) return false;
    escribirRegistroMensaje(f, mensaje);
    std::cout << "  Guardado un mensaje nuevo en mensajes.txt" << std::endl;
    return f.good();
}

bool Almacenamiento::guardarSolicitudNueva(const Solicitud& solicitud) {
    std::fstream f;
    if (!abrirParaAgregar("solicitudes.txt", f)) return false;
    escribirRegistroSolicitud(f, solicitud);
    std::cout << "  Guardada una solicitud nueva en solicitudes.txt" << std::endl;
    return f.good();
}

bool Almacenamiento::guardarAmistadNueva(int i, int j) {
    if (i < 0 || j < 0 || i == j) return false;
    std::fstream f;
    if (!abrirParaAgregar("amistades.txt", f)) return false;
    int menor = (i < j) ? i : j;
    int mayor = (i < j) ? j : i;
    f << menor << '|' << mayor << '\n';
    std::cout << "  Guardada una amistad nueva en amistades.txt" << std::endl;
    return f.good();
}

// Al aceptar una solicitud: se reescribe solicitudes.txt con las que quedan
bool Almacenamiento::guardarSolicitudes(const Lista<Usuario>& usuarios) {
    char ruta[MAX_RUTA];
    rutaDe("solicitudes.txt", ruta, MAX_RUTA);
    std::ofstream f(ruta);
    if (!f.is_open()) {
        std::cout << "  ERROR: no se pudo escribir " << ruta << std::endl;
        return false;
    }
    int total = 0;
    for (size_t u = 0; u < usuarios.getTamanio(); u++) {
        Medidor::incrementarIteraciones();
        total += static_cast<int>(usuarios[u].getSolicitudes().getTamanio());
    }
    escribirCantidad(f, total);
    for (size_t u = 0; u < usuarios.getTamanio(); u++) {
        const Lista<Solicitud>& lista = usuarios[u].getSolicitudes();
        for (size_t s = 0; s < lista.getTamanio(); s++) {
            Medidor::incrementarIteraciones();
            escribirRegistroSolicitud(f, lista[s]);
        }
    }
    std::cout << "  Reescrito solicitudes.txt (" << total << " registros)" << std::endl;
    return f.good();
}

void Almacenamiento::anotarCambioMensajes() {
    mensajesModificados = true;
    cambiosPendientes++;
}

void Almacenamiento::anotarCambioFotos() {
    fotosModificadas = true;
    cambiosPendientes++;
}

bool Almacenamiento::debeGuardarse() const {
    return cambiosPendientes >= umbralCambios;
}

// Escribe solo los archivos que cambiaron (mensajes.txt y/o fotos.txt)
bool Almacenamiento::guardarPendientes(const Lista<Usuario>& usuarios, const Lista<Mensaje>& mensajes) {
    bool todoBien = true;
    if (mensajesModificados) {
        char ruta[MAX_RUTA];
        rutaDe("mensajes.txt", ruta, MAX_RUTA);
        std::ofstream f(ruta);
        if (!f.is_open()) {
            std::cout << "  ERROR: no se pudo escribir " << ruta << std::endl;
            todoBien = false;
        } else {
            escribirCantidad(f, static_cast<int>(mensajes.getTamanio()));
            for (size_t i = 0; i < mensajes.getTamanio(); i++) {
                Medidor::incrementarIteraciones();
                escribirRegistroMensaje(f, mensajes[i]);
            }
            std::cout << "  Reescrito mensajes.txt (" << mensajes.getTamanio() << " registros)" << std::endl;
            if (f.good()) mensajesModificados = false; else todoBien = false;
        }
    }
    if (fotosModificadas) {
        char ruta[MAX_RUTA];
        rutaDe("fotos.txt", ruta, MAX_RUTA);
        std::ofstream f(ruta);
        if (!f.is_open()) {
            std::cout << "  ERROR: no se pudo escribir " << ruta << std::endl;
            todoBien = false;
        } else {
            int total = 0;
            for (size_t u = 0; u < usuarios.getTamanio(); u++) {
                const Lista<Album>& albumes = usuarios[u].getAlbumes();
                for (size_t a = 0; a < albumes.getTamanio(); a++) {
                    Medidor::incrementarIteraciones();
                    total += static_cast<int>(albumes[a].getFotos().getTamanio());
                }
            }
            escribirCantidad(f, total);
            for (size_t u = 0; u < usuarios.getTamanio(); u++) {
                const Lista<Album>& albumes = usuarios[u].getAlbumes();
                for (size_t a = 0; a < albumes.getTamanio(); a++) {
                    const Lista<Foto>& fotos = albumes[a].getFotos();
                    for (size_t k = 0; k < fotos.getTamanio(); k++) {
                        Medidor::incrementarIteraciones();
                        char nombre[MAX_CORTO];
                        extraerNombre(fotos[k].getRuta(), nombre, MAX_CORTO);
                        f << u << '|' << a << '|';
                        escribirFecha(f, fotos[k].getFechaCaptura(), true);
                        f << '|' << fotos[k].getTamano() << '|' << nombre << '|' << fotos[k].getLikes() << '\n';
                    }
                }
            }
            std::cout << "  Reescrito fotos.txt (" << total << " registros)" << std::endl;
            if (f.good()) fotosModificadas = false; else todoBien = false;
        }
    }
    if (todoBien) cambiosPendientes = 0;
    return todoBien;
}

Almacenamiento& Almacenamiento::operator=(const Almacenamiento& otro) {
    if (this != &otro) {
        subRutaFija = otro.subRutaFija;
        rutaArchivos = otro.rutaArchivos;
        cambiosPendientes = otro.cambiosPendientes;
        umbralCambios = otro.umbralCambios;
        mensajesModificados = otro.mensajesModificados;
        fotosModificadas = otro.fotosModificadas;
    }
    return *this;
}

bool Almacenamiento::operator==(const Almacenamiento& otro) const {
    return subRutaFija == otro.subRutaFija &&
           rutaArchivos == otro.rutaArchivos &&
           cambiosPendientes == otro.cambiosPendientes &&
           umbralCambios == otro.umbralCambios &&
           mensajesModificados == otro.mensajesModificados &&
           fotosModificadas == otro.fotosModificadas;
}

long Almacenamiento::calcularMemoria() const {
    const long fijaCadena = static_cast<long>(sizeof(Cadena));
    return static_cast<long>(sizeof(Almacenamiento)) +
           (subRutaFija.calcularMemoria() - fijaCadena) +
           (rutaArchivos.calcularMemoria() - fijaCadena);
}