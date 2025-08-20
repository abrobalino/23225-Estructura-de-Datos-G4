#include "Graph.hpp"
#include <iostream>
#include <sstream>
#include <fstream>
using namespace std;

void Graph::agregarIsla(int id, const string& nombre) {
    islas.agregarAlFinal(Island(id, nombre));
}

void Graph::agregarPuente(int idA, int idB) {
    puentes.agregarAlFinal(Bridge(idA, idB));
}

const MiListaDoble<Island>& Graph::obtenerIslas() const {
    return islas;
}

const MiListaDoble<Bridge>& Graph::obtenerPuentes() const {
    return puentes;
}

void Graph::mostrarGrafo() const {
    cout << "Islas:" << endl;
    for (int i = 0; i < islas.tamano(); ++i) {
        cout << " - Isla ID: " << islas[i].obtenerId()
             << ", Nombre: " << islas[i].obtenerNombre() << endl;
    }
    cout << "\nPuentes:" << endl;
    for (int i = 0; i < puentes.tamano(); ++i) {
        cout << " - Conecta Isla " << puentes[i].obtenerIslaA()
             << " e Isla " << puentes[i].obtenerIslaB()
             << ", Costo: " << puentes[i].obtenerCosto()
             << ", Propietario: " << puentes[i].obtenerPropietario() << endl;
    }
}

void Graph::limpiar() {
    islas.limpiar();
    puentes.limpiar();
}

bool Graph::cargarDesdeArchivo(const std::string& ruta) {
    std::ifstream in(ruta.c_str());
    if (!in.is_open()) return false;
    limpiar();
    int n = 0;
    if (!(in >> n)) return false;
    std::string dummy;
    std::getline(in, dummy);
    for (int i = 0; i < n; ++i) {
        std::string nombre;
        std::getline(in, nombre);
        if (!nombre.empty() && nombre.back() == '\r') nombre.pop_back();
        if (nombre.empty()) nombre = "Isla " + std::to_string(i);
        agregarIsla(i, nombre);
    }
    int a, b;
    while (true) {
        in >> std::ws;
        if (in.eof()) break;
        std::streampos pos = in.tellg();
        if (in >> a >> b) {
            if (a >= 0 && a < n && b >= 0 && b < n && a != b) {
                agregarPuente(a, b);
            }
        } else {
            in.clear();
            in.seekg(pos);
            std::string skip;
            std::getline(in, skip);
        }
    }
    return true;
}
