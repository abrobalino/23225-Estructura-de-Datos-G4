#include "Bridge.hpp"
#include <random>
#include <ctime>

Bridge::Bridge()
    : islaA(-1), islaB(-1), costo(0), propietario("Ninguno") {}

Bridge::Bridge(int islaA, int islaB)
    : islaA(islaA), islaB(islaB), propietario("Ninguno") {
    static bool semillaInicializada = false;
    if (!semillaInicializada) {
        srand(static_cast<unsigned>(time(0)));
        semillaInicializada = true;
    }
    costo = 1 + rand() % 20;
}

int Bridge::obtenerIslaA() const {
    return islaA;
}

int Bridge::obtenerIslaB() const {
    return islaB;
}

int Bridge::obtenerCosto() const {
    return costo;
}

std::string Bridge::obtenerPropietario() const {
    return propietario;
}

void Bridge::establecerPropietario(const std::string& _propietario) {
    propietario = _propietario;
}

void Bridge::establecerCosto(int _costo) {
    costo = _costo;
}

bool Bridge::operator<(const Bridge& otro) const {
    return costo < otro.costo;
}
