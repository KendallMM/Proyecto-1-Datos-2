#include <iostream>
#include <string.h>
#include <vector>
#include <fstream>
#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace std;
using std::fstream;
using std::ofstream;
using std::ios;

struct Tarjeta {
    int X;
    int Y;
    char base64[133000];
};

class memoryManagement
{
private:
    fstream memoria;
    int filas = 5, columnas = 6;
    int numTarjetas = filas*columnas;
    vector <Tarjeta> vectorMatriz; //usar para la paginación
    string tarjetaTemp1; //propenso a cambiar
    string tajetaTemp2; //propenso a cambiar
    bool temp = true;
    int pageHits = 0;
    int pageFaults = 0;


public:
    memoryManagement();
    int cartasRestantes();
    void cargarMemoria();
    void crearMatriz(int fila,int columna);
    vector <string> shuffleTarjetas();
    bool evaluar();
};

memoryManagement::memoryManagement() {
    crearMatriz(filas, columnas);
}

int memoryManagement::cartasRestantes() {
    return numTarjetas;
}

void memoryManagement::cargarMemoria() {

    ofstream archivo;
    string nombreArchivo="matriz";
    archivo.open(nombreArchivo.c_str(),ios::out); //Creamos el archivo

    if(archivo.fail()){ //Si a ocurrido algun error
        cout<<"No se pudo abrir el archivo";
        exit(1);
    }
    fflush(stdin);
    archivo.close(); //Cerramos el archivo
}

void memoryManagement::crearMatriz(int fila, int columna) {
    cargarMemoria();
    vector <string> Tarjetas;
    Tarjeta matriz[fila][columna];
    int n = 0;
    for (int i = 0; i < fila; i++) {
        for (int j = 0; j < columna; j++) {
            matriz[i][j].X = i;
            matriz[i][j].Y = j;
            for (int z = 0; z < sizeof(char[133000]); z++) {
                matriz[i][j].base64[z] = Tarjetas[n][z];
            }
            n++;
        }
    }
    memoria.write((char*) &matriz, sizeof(Tarjeta[fila][columna])); //Escribe en vmem la matriz recién hecha
}



