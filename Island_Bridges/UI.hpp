#ifndef UI_HPP
#define UI_HPP
#include <SFML/Graphics.hpp>
#include "Graph.hpp"
#include "UnionFind.hpp"

class UI {
public:
    static int generarOpciones2(const Graph& grafo,
                                const int* ownersJugador,
                                UnionFind& dsuJugador,
                                const sf::Vector2f* posIslas,
                                int islaActual,
                                int outOpc[2],
                                int outDest[2],
                                bool outBuildable[2]);

    static void dibujarTurno(sf::RenderWindow& window,
                             const Graph& grafo,
                             const int* ownersJugador,
                             const sf::Vector2f* posIslas,
                             const int* opciones, int numOpc,
                             int islaActual,
                             const sf::Font& font,
                             sf::Color colorPropio);
};
#endif
