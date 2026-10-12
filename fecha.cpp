#include "Fecha.h"
#include <ctime>

// Constructor por defecto: 01/01/1970 a las 00:00
Fecha::Fecha() {
    dia = 1;
    mes = 1;
    anio = 1970;
    hora = 0;
    minuto = 0;
}

Fecha::Fecha(int d, int m, int a) {
    dia = 1;
    mes = 1;
    anio = 1970;
    hora = 0;
    minuto = 0;
    setDia(d);
    setMes(m);
    setAnio(a);
}

// Constructor con fecha y hora completas
Fecha::Fecha(int d, int m, int a, int h, int min) {
    dia = 1;
    mes = 1;
    anio = 1970;
    hora = 0;
    minuto = 0;
    setDia(d);
    setMes(m);
    setAnio(a);
    setHora(h);
    setMinuto(min);
}

// Constructor de copia: copia cada atributo
Fecha::Fecha(const Fecha& otra) {
    dia = otra.dia;
    mes = otra.mes;
    anio = otra.anio;
    hora = otra.hora;
    minuto = otra.minuto;
}

// Destructor:
Fecha::~Fecha() {
}

int Fecha::getDia() const    { return dia; }
int Fecha::getMes() const    { return mes; }
int Fecha::getAnio() const   { return anio; }
int Fecha::getHora() const   { return hora; }
int Fecha::getMinuto() const { return minuto; }

void Fecha::setDia(int nuevoDia) {
    if (nuevoDia >= 1 && nuevoDia <= 31) {
        dia = nuevoDia;
    }
}

void Fecha::setMes(int nuevoMes) {
    if (nuevoMes >= 1 && nuevoMes <= 12) {
        mes = nuevoMes;
    }
}

void Fecha::setAnio(int nuevoAnio) {
    if (nuevoAnio >= 1) {
        anio = nuevoAnio;
    }
}

void Fecha::setHora(int nuevaHora) {
    if (nuevaHora >= 0 && nuevaHora <= 23) {
        hora = nuevaHora;
    }
}

void Fecha::setMinuto(int nuevoMinuto) {
    if (nuevoMinuto >= 0 && nuevoMinuto <= 59) {
        minuto = nuevoMinuto;
    }
}


// Operadores

Fecha& Fecha::operator=(const Fecha& otra) {
    if (this != &otra) {
        dia = otra.dia;
        mes = otra.mes;
        anio = otra.anio;
        hora = otra.hora;
        minuto = otra.minuto;
    }
    return *this;
}

bool Fecha::operator==(const Fecha& otra) const {
    return dia == otra.dia && mes == otra.mes && anio == otra.anio &&
           hora == otra.hora && minuto == otra.minuto;
}
bool Fecha::operator<(const Fecha& otra) const {
    return compararFechas(*this, otra) < 0;
}

// Menor o igual que
bool Fecha::operator<=(const Fecha& otra) const {
    return compararFechas(*this, otra) <= 0;
}

Fecha Fecha::ahora() {
    Fecha resultado;                                  // 01/01/1970 por defecto
    std::time_t segundos = std::time(nullptr);
    std::tm* local = std::localtime(&segundos);       // fecha local descompuesta
    if (local != nullptr) {
        resultado.dia = local->tm_mday;
        resultado.mes = local->tm_mon + 1;
        resultado.anio = local->tm_year + 1900;
        resultado.hora = local->tm_hour;
        resultado.minuto = local->tm_min;
    }
    return resultado;
}

// Memoria
long Fecha::calcularMemoria() const {
    return static_cast<long>(sizeof(Fecha));
}

int compararFechas(Fecha a, Fecha b) {
    if (a.anio != b.anio)     return (a.anio < b.anio) ? -1 : 1;
    if (a.mes != b.mes)       return (a.mes < b.mes) ? -1 : 1;
    if (a.dia != b.dia)       return (a.dia < b.dia) ? -1 : 1;
    if (a.hora != b.hora)     return (a.hora < b.hora) ? -1 : 1;
    if (a.minuto != b.minuto) return (a.minuto < b.minuto) ? -1 : 1;
    return 0;
}