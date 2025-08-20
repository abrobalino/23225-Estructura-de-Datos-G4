#ifndef BRIDGE_HPP
#define BRIDGE_HPP
#include <string>

class Bridge {
private:
    int islaA;
    int islaB;
    int costo;
    std::string propietario;

public:
    Bridge();
    Bridge(int islaA, int islaB);

    int obtenerIslaA() const;
    int obtenerIslaB() const;
    int obtenerCosto() const;
    std::string obtenerPropietario() const;
    void establecerPropietario(const std::string& _propietario);
    void establecerCosto(int _costo);

    bool operator<(const Bridge& otro) const;
};

#endif // BRIDGE_HPP
