#ifndef GAME_HPP
#define GAME_HPP

#include <SFML/Graphics.hpp>
#include "Graph.hpp"
#include "UnionFind.hpp"

class Game {
public:
    Game();
    ~Game();
    void run();
    int nivelActual = 1;
    int totalNiveles = 3;
    bool cargarNivel(int idx);
    void reiniciarEstructuras();

private:
    Graph grafo;
    int* ownersJ1;
    int* ownersJ2;
    UnionFind* dsuJ1;
    UnionFind* dsuJ2;
    int aristasJ1, aristasJ2;
    int costoJ1, costoJ2;
    int costoOptimo;
    sf::Vector2f* posIslas;
    int currentIslaJ1;
    int currentIslaJ2;
    float distPuntoSegmento(const sf::Vector2f& p, const sf::Vector2f& a, const sf::Vector2f& b) const;
    bool clickSobrePuente(int mx, int my, int& idxClic) const;
    bool clickSobreIsla(int mx, int my, int& islaId) const;
    void inicializarPosiciones(int n);
    int generarOpciones(int islaActual, bool esJ1, int* opc, int maxOpc) const;
    int calcularCostoMST() const;
    bool mostrarResultados(bool haySiguiente) const;
    void debugGradosIniciales() const;
    void debugOpcionesPorIsla(bool esJ1) const;
};

#endif // GAME_HPP
