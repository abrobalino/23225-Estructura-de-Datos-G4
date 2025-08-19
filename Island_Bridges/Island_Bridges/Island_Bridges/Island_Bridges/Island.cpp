#include "Island.hpp"

Island::Island(int _id, const std::string& _nombre) {
    id = _id;
    nombre = _nombre;
}

void Island::setId(int _id) {
    id = _id;
}

void Island::setNombre(const std::string& _nombre) {
    nombre = _nombre;
}

int Island::obtenerId() const {
    return id;
}

std::string Island::obtenerNombre() const {
    return nombre;
}
