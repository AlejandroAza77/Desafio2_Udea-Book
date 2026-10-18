#include <iostream>
#include <fstream>
#include <random>
#include <cstdlib>

using namespace std;

//  aleatorios
static mt19937 motor;

static int azar(int tope) {                 // entero entre 0 y tope-1
    uniform_int_distribution<int> d(0, tope - 1);
    return d(motor);
}

static bool conProbabilidad(double p) {
    uniform_real_distribution<double> d(0.0, 1.0);
    return d(motor) < p;
}

// listas
static const char* NOMBRES[] = {"juan", "ana", "luis", "maria", "carlos", "laura", "pedro", "sofia", "diego", "camila",
                                "andres", "valentina", "jorge", "daniela", "felipe", "paula", "mateo", "isabela", "sebastian", "lucia",
                                "david", "natalia", "nicolas", "carolina", "santiago", "juliana", "alejandro", "manuela", "tomas", "gabriela",
                                "esteban", "mariana", "julian", "catalina", "simon", "andrea", "miguel", "angela", "samuel", "melissa"};
static const int N_NOMBRES = 40;

static const char* CIUDADES[] = {"Medellin", "Bogota", "Cali", "Barranquilla", "Cartagena", "Bucaramanga", "Pereira", "Manizales",
                                 "Lima", "Quito", "Ciudad de Mexico", "Buenos Aires"};
static const char* PAISES[] = {"Colombia", "Colombia", "Colombia", "Colombia", "Colombia", "Colombia", "Colombia", "Colombia",
                               "Peru", "Ecuador", "Mexico", "Argentina"};
static const int N_CIUDADES = 12;

static const char* ALBUMES[] = {"Viaje", "Familia", "Fiesta", "Universidad", "Mascotas", "Comida", "Deporte", "Playa"};
static const int N_ALBUMES = 8;

static const char* SALUDOS[] = {"Hola", "Buenas", "Que mas", "Hey", "Oye"};
static const char* CUERPOS[] = {"como vas con el desafio", "viste las fotos del viaje", "nos vemos manana en la universidad",
                                "te mando el trabajo despues", "gracias por la ayuda", "feliz cumple", "ya llegaste a casa",
                                "vamos por un cafe", "revisa lo que te envie", "hoy estuvo muy bueno el dia"};
static const char* CIERRES[] = {"", " Un abrazo", " Nos hablamos", " Cuidate", " Saludos"};

// ------------------------------------------------------------------ fechas
// Dias desde 1970-01-01 para una fecha (algoritmo de calendario sin librerias)
static long diasDesdeEpoca(int a, int m, int d) {
    a -= (m <= 2) ? 1 : 0;
    long era = (a >= 0 ? a : a - 399) / 400;
    long yoe = a - era * 400;
    long doy = (153L * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 1;
    long doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
    return era * 146097 + doe - 719468;
}

static void fechaDesdeDias(long z, int& a, int& m, int& d) {
    z += 719468;
    long era = (z >= 0 ? z : z - 146096) / 146097;
    long doe = z - era * 146097;
    long yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
    long y = yoe + era * 400;
    long doy = doe - (365 * yoe + yoe / 4 - yoe / 100);
    long mp = (5 * doy + 2) / 153;
    d = static_cast<int>(doy - (153 * mp + 2) / 5 + 1);
    m = static_cast<int>(mp < 10 ? mp + 3 : mp - 9);
    a = static_cast<int>(y + (m <= 2 ? 1 : 0));
}

static void escribirNumero(ostream& out, int valor, int digitos) {
    char t[16];
    int n = 0;
    for (int i = 0; i < digitos; i++) {
        t[n++] = static_cast<char>('0' + valor % 10);
        valor /= 10;
    }
    while (n > 0) out << t[--n];
}

// dd/mm/aaaa o dd/mm/aaaa hh:mm
static void escribirFecha(ostream& out, long dias, int minutoDelDia, bool conHora) {
    int a, m, d;
    fechaDesdeDias(dias, a, m, d);
    escribirNumero(out, d, 2); out << '/';
    escribirNumero(out, m, 2); out << '/';
    escribirNumero(out, a, 4);
    if (conHora) {
        out << ' ';
        escribirNumero(out, minutoDelDia / 60, 2); out << ':';
        escribirNumero(out, minutoDelDia % 60, 2);
    }
}

// archivos
static void abrirArchivo(ofstream& f, const char* carpeta, const char* nombre) {
    char ruta[600];
    int p = 0;
    for (int i = 0; carpeta[i] != '\0' && p < 500; i++) ruta[p++] = carpeta[i];
    if (p > 0 && ruta[p - 1] != '/' && ruta[p - 1] != '\\') ruta[p++] = '/';
    for (int i = 0; nombre[i] != '\0'; i++) ruta[p++] = nombre[i];
    ruta[p] = '\0';
    f.open(ruta);
    if (!f.is_open()) {
        cout << "ERROR: no se pudo crear " << ruta << " (la carpeta debe existir)" << endl;
        exit(1);
    }
    escribirNumero(f, 0, 10);       // espacio para la cantidad de registros
    f << '\n';
}

// Escribe la cantidad real de registros en el encabezado y cierra el archivo
static void cerrarArchivo(ofstream& f, int cantidad) {
    f.seekp(0);
    escribirNumero(f, cantidad, 10);
    f.close();
}

//  grafo
static int N;
static unsigned char* ady;         // N x N, solo para generar (el nucleo usa bits)
static int* cluster;               // grupo de cada usuario

static bool sonAmigos(int i, int j) { return ady[static_cast<long>(i) * N + j] == 1; }

static void hacerAmigos(int i, int j) {
    if (i == j) return;
    ady[static_cast<long>(i) * N + j] = 1;
    ady[static_cast<long>(j) * N + i] = 1;
}

static int grado(int i) {
    int g = 0;
    for (int k = 0; k < N; k++) if (sonAmigos(i, k)) g++;
    return g;
}

static int enComun(int i, int j) {
    int c = 0;
    for (int k = 0; k < N; k++) if (sonAmigos(i, k) && sonAmigos(j, k)) c++;
    return c;
}

int main(int argc, char* argv[]) {
    const char* carpeta = (argc > 1) ? argv[1] : "datos";
    N = (argc > 2) ? atoi(argv[2]) : 1000;
    unsigned int semilla = (argc > 3) ? static_cast<unsigned int>(atoi(argv[3])) : 2026;
    int mensajesPorUsuario = (argc > 4) ? atoi(argv[4]) : 25;
    if (N < 100) N = 100;
    if (mensajesPorUsuario < 1) mensajesPorUsuario = 1;
    motor.seed(semilla);

    cout << "Generando " << N << " usuarios (semilla " << semilla << ") en la carpeta '" << carpeta << "' ..." << endl;
    ady = new unsigned char[static_cast<long>(N) * N]();
    cluster = new int[N];

    // ---- 1. Grupos de amigos. El grupo 0 (con el usuario 0) es grande y muy denso.
    //         Los demas alternan densidad alta, media y baja.
    int inicio = 0, g = 0;
    while (inicio < N) {
        int tam = (g == 0) ? 45 : 40;
        if (inicio + tam > N || N - (inicio + tam) < 10) tam = N - inicio;
        double p;
        if (g == 0) p = 0.85;
        else if (g % 3 == 1) p = 0.80;
        else if (g % 3 == 2) p = 0.60;
        else p = 0.35;
        for (int i = inicio; i < inicio + tam; i++) {
            cluster[i] = g;
            for (int j = i + 1; j < inicio + tam; j++) {
                if (conProbabilidad(p)) hacerAmigos(i, j);
            }
        }
        inicio += tam;
        g++;
    }
    // Algunos amigos fuera del grupo (2 a 4 por usuario)
    for (int i = 0; i < N; i++) {
        int extra = 2 + azar(3);
        for (int e = 0; e < extra; e++) {
            for (int intento = 0; intento < 30; intento++) {
                int j = azar(N);
                if (cluster[j] != cluster[i] && j != i) { hacerAmigos(i, j); break; }
            }
        }
    }

    // 2. Usuario de demostracion (el 0): amigos "puente" para que se vean
    //  los tres pesos del sorteo de mejores amigos.
    const int D = 0;
    int puente25 = 0, puente1 = 0;
    for (int intento = 0; intento < 100000 && (puente25 < 5 || puente1 < 5); intento++) {
        int x = azar(N);
        if (x == D || cluster[x] == cluster[D] || sonAmigos(D, x)) continue;
        bool es25 = puente25 < 5;
        int conectar = es25 ? 15 : 2;          // amigos de D a los que se une x
        int hechos = 0;
        hacerAmigos(D, x);
        for (int t = 0; t < 10000 && hechos < conectar; t++) {
            int f = azar(N);
            if (f != D && f != x && cluster[f] == cluster[D] && sonAmigos(D, f) && !sonAmigos(x, f)) {
                hacerAmigos(x, f);
                hechos++;
            }
        }
        if (es25) puente25++; else puente1++;
    }

    // 3. Resumen del usuario de demostracion
    int totalAristas = 0;
    for (int i = 0; i < N; i++) totalAristas += grado(i);
    totalAristas /= 2;
    int peso1 = 0, peso25 = 0, peso35 = 0, amigosD = 0;
    for (int f = 0; f < N; f++) {
        if (!sonAmigos(D, f)) continue;
        amigosD++;
        int c = enComun(D, f);
        if (c <= 10) peso1++; else if (c <= 20) peso25++; else peso35++;
    }
    int candidatos = 0, mejorCandidato = -1, mejorComun = 0;
    for (int x = 0; x < N; x++) {
        if (x == D || sonAmigos(D, x)) continue;
        int c = enComun(D, x);
        if (c > 0) {
            candidatos++;
            if (c > mejorComun) { mejorComun = c; mejorCandidato = x; }
        }
    }

    // 4. usuarios.txt
    ofstream f;
    abrirArchivo(f, carpeta, "usuarios.txt");
    for (int i = 0; i < N; i++) {
        f << NOMBRES[i % N_NOMBRES] << i << "|clave" << i << '|';
        escribirFecha(f, diasDesdeEpoca(1970, 1, 1) + 365L * (22 + azar(35)) + azar(365), 0, false);   // 1992 a 2026 aprox.
        int c = azar(N_CIUDADES);
        f << '|' << NOMBRES[i % N_NOMBRES] << i << "@udeabook.co|" << CIUDADES[c] << '|' << PAISES[c] << '|';
        escribirFecha(f, diasDesdeEpoca(2024, 1, 1) + azar(640), 0, false);
        f << '\n';
    }
    cerrarArchivo(f, N);

    // 5. amistades.txt (cada pareja una sola vez, idA < idB)
    abrirArchivo(f, carpeta, "amistades.txt");
    int nAmistades = 0;
    for (int i = 0; i < N; i++) {
        for (int j = i + 1; j < N; j++) {
            if (sonAmigos(i, j)) { f << i << '|' << j << '\n'; nAmistades++; }
        }
    }
    cerrarArchivo(f, nAmistades);

    // 6. solicitudes.txt: sin parejas repetidas ni en sentido contrario
    //         y nunca entre amigos. D recibe 6 y envia 2, siempre con desconocidos.
    unsigned char* sol = new unsigned char[static_cast<long>(N) * N]();
    abrirArchivo(f, carpeta, "solicitudes.txt");
    int nSol = 0;
    long finDatos = diasDesdeEpoca(2026, 10, 10);
    int recibidasD = 0, enviadasD = 0;
    for (int intento = 0; intento < 200000 && (recibidasD < 6 || enviadasD < 2); intento++) {
        int x = azar(N);
        if (x == D || sonAmigos(D, x) || enComun(D, x) > 0) continue;
        if (sol[static_cast<long>(D) * N + x] || sol[static_cast<long>(x) * N + D]) continue;
        bool recibe = recibidasD < 6;
        int emisor = recibe ? x : D, receptor = recibe ? D : x;
        sol[static_cast<long>(emisor) * N + receptor] = 1;
        f << emisor << '|' << receptor << '|';
        escribirFecha(f, finDatos - azar(20), azar(1440), true);
        f << '\n';
        nSol++;
        if (recibe) recibidasD++; else enviadasD++;
    }
    for (int k = 0; k < N / 3; k++) {
        int a = azar(N), b = azar(N);
        if (a == b || sonAmigos(a, b) || a == D || b == D) continue;
        if (sol[static_cast<long>(a) * N + b] || sol[static_cast<long>(b) * N + a]) continue;
        sol[static_cast<long>(a) * N + b] = 1;
        f << a << '|' << b << '|';
        escribirFecha(f, finDatos - azar(20), azar(1440), true);
        f << '\n';
        nSol++;
    }
    cerrarArchivo(f, nSol);
    delete[] sol;

    //  7. mensajes.txt: entre amigos, en orden cronologico (10 semanas)
    //         Se arman las listas de amigos de cada usuario para elegir receptor.
    int* inicioAmigos = new int[N + 1];
    int* listaAmigos = new int[2 * totalAristas + 1];
    int pos = 0;
    for (int i = 0; i < N; i++) {
        inicioAmigos[i] = pos;
        for (int j = 0; j < N; j++) if (sonAmigos(i, j)) listaAmigos[pos++] = j;
    }
    inicioAmigos[N] = pos;

    abrirArchivo(f, carpeta, "mensajes.txt");
    long totalMensajes = static_cast<long>(N) * mensajesPorUsuario;
    long inicioMsg = diasDesdeEpoca(2026, 8, 1) * 1440L;       // en minutos
    long duracion = 70L * 1440L;                                // 10 semanas
    long paso = duracion / totalMensajes;
    if (paso < 1) paso = 1;
    int nMsg = 0;
    for (long k = 0; k < totalMensajes; k++) {
        int emisor, receptor;
        if (k % 25 == 0) {               // para D: mensajes recibidos de sus amigos
            receptor = D;
            emisor = listaAmigos[inicioAmigos[D] + azar(inicioAmigos[D + 1] - inicioAmigos[D])];
        } else if (k % 25 == 1) {        // y mensajes enviados a sus amigos
            emisor = D;
            receptor = listaAmigos[inicioAmigos[D] + azar(inicioAmigos[D + 1] - inicioAmigos[D])];
        } else {
            do { emisor = azar(N); } while (inicioAmigos[emisor + 1] == inicioAmigos[emisor]);
            receptor = listaAmigos[inicioAmigos[emisor] + azar(inicioAmigos[emisor + 1] - inicioAmigos[emisor])];
        }
        long minutos = inicioMsg + k * paso + azar(static_cast<int>(paso));
        bool reciente = k >= totalMensajes * 85 / 100;
        int leido = (reciente && conProbabilidad(0.7)) ? 0 : 1;
        f << emisor << '|' << receptor << '|';
        escribirFecha(f, minutos / 1440, static_cast<int>(minutos % 1440), true);
        f << '|' << leido << '|';
        if (k % 4000 == 7) {             // mensaje de 500 caracteres
            for (int c = 0; c < 500; c++) f << static_cast<char>('a' + (c % 26));
        } else if (k % 1000 == 3) {      // texto con una barra, para probar el formato
            f << "Hola | esto lleva una barra";
        } else {
            f << SALUDOS[azar(5)] << ", " << CUERPOS[azar(10)] << '.' << CIERRES[azar(5)];
        }
        f << '\n';
        nMsg++;
    }
    cerrarArchivo(f, nMsg);

    // ---- 8. albumes.txt y fotos.txt: todos tienen entre 2 y 4 albumes de 5 a 12
    //         fotos. Los dos primeros albumes son publicos o de amigos, para que
    //         cualquier amigo pueda visitarlos.
    ofstream fotos;
    abrirArchivo(f, carpeta, "albumes.txt");
    abrirArchivo(fotos, carpeta, "fotos.txt");
    int nAlbumes = 0, nFotos = 0;
    for (int u = 0; u < N; u++) {
        int cantidad = 2 + azar(3);
        for (int a = 0; a < cantidad; a++) {
            int privacidad = (a < 2) ? (conProbabilidad(0.5) ? 0 : 2) : azar(3);
            f << u << '|' << ALBUMES[(u + a * 3) % N_ALBUMES] << '|' << privacidad << '\n';
            nAlbumes++;
            int cuantas = 5 + azar(8);
            for (int k = 1; k <= cuantas; k++) {
                fotos << u << '|' << a << '|';
                escribirFecha(fotos, diasDesdeEpoca(2025, 1, 1) + azar(640), azar(1440), true);
                fotos << '|' << (200 + azar(8800)) << "|foto" << k << '|' << azar(120) << '\n';
                nFotos++;
            }
        }
    }
    cerrarArchivo(f, nAlbumes);
    cerrarArchivo(fotos, nFotos);

    // ---- 9. Resumen
    cout << endl << "Archivos generados:" << endl;
    cout << "  usuarios.txt    " << N << " registros" << endl;
    cout << "  amistades.txt   " << nAmistades << " registros (" << (N > 0 ? (2.0 * nAmistades / N) : 0) << " amigos por usuario en promedio)" << endl;
    cout << "  solicitudes.txt " << nSol << " registros" << endl;
    cout << "  mensajes.txt    " << nMsg << " registros (10 semanas: 01/08/2026 a 10/10/2026)" << endl;
    cout << "  albumes.txt     " << nAlbumes << " registros" << endl;
    cout << "  fotos.txt       " << nFotos << " registros" << endl;
    cout << endl << "Usuario para la demostracion: " << NOMBRES[D % N_NOMBRES] << D << "  (contrasena: clave" << D << ")" << endl;
    cout << "  amigos: " << amigosD << "  (peso 1: " << peso1 << ", peso 2.5: " << peso25 << ", peso 3.5: " << peso35 << ")" << endl;
    cout << "  sugerencias posibles (amigos de amigos): " << candidatos << "  (la de mayor coincidencia: usuario " << mejorCandidato
         << " con " << mejorComun << " amigos en comun)" << endl;
    cout << "  solicitudes recibidas: " << recibidasD << ", enviadas: " << enviadasD << endl;
    cout << "  un amigo con 2 o mas albumes y 5 o mas fotos en cada uno: todos los usuarios lo cumplen" << endl;
    cout << "  mensajes que recibe y envia: cada 25 mensajes uno es suyo, repartidos en las 10 semanas" << endl;
    delete[] ady; delete[] cluster; delete[] inicioAmigos; delete[] listaAmigos;
    return 0;
}