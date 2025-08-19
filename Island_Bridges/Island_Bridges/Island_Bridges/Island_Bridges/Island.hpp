#ifndef ISLAND_HPP
#define ISLAND_HPP
#include <string>

#include <string>

class Island {
private:
    int id;
    std::string nombre;

public:
    Island(int _id = -1, const std::string& _nombre = "");

    void setId(int _id);
    void setNombre(const std::string& _nombre);

    int obtenerId() const;
    std::string obtenerNombre() const;
};

#endif // ISLAND_HPP
