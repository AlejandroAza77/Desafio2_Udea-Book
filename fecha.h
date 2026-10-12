#ifndef FECHA_H
#define FECHA_H


class Fecha {
private:
    int dia;
    int mes;
    int anio;
    int hora;
    int minuto;

public:
    // Constructores y destructor
    Fecha();
    Fecha(int d, int m, int a);
    Fecha(int d, int m, int a, int h, int min);
    Fecha(const Fecha& otra);
    ~Fecha();

    //  Getters
    int getDia() const;
    int getMes() const;
    int getAnio() const;
    int getHora() const;
    int getMinuto() const;

    //  Setters
    void setDia(int nuevoDia);
    void setMes(int nuevoMes);
    void setAnio(int nuevoAnio);
    void setHora(int nuevaHora);
    void setMinuto(int nuevoMinuto);

    //  Operadores sobrecargados
    Fecha& operator=(const Fecha& otra);
    bool operator==(const Fecha& otra) const;
    bool operator<(const Fecha& otra) const;
    bool operator<=(const Fecha& otra) const;


    static Fecha ahora();

    // Memoria que ocupa un objeto Fecha, en bytes
    long calcularMemoria() const;

    friend int compararFechas(Fecha a, Fecha b);
};

int compararFechas(Fecha a, Fecha b);

#endif