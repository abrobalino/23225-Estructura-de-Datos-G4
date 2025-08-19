#ifndef GRAPH_HPP
#define GRAPH_HPP
#include "Island.hpp"
#include "Bridge.hpp"
#include "MiVector.hpp"

class Graph {
private:
    MiListaDoble<Island> islas;
    MiListaDoble<Bridge> puentes;

public:
    void agregarIsla(int id, const std::string& nombre = "");
    void agregarPuente(int idA, int idB);
    void mostrarGrafo() const;
    const MiListaDoble<Island>& obtenerIslas() const;
    const MiListaDoble<Bridge>& obtenerPuentes() const;
    void limpiar();
    bool cargarDesdeArchivo(const std::string& nombreArchivo);
};

#endif
