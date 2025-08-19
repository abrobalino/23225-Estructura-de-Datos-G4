#include "UI.hpp"
#include <cmath>
#include <string>

namespace {
float dist2(const sf::Vector2f& a, const sf::Vector2f& b){
    float dx=a.x-b.x, dy=a.y-b.y; return dx*dx+dy*dy;
}
}

int UI::generarOpciones2(const Graph& grafo,
                         const int* ownersJugador,
                         UnionFind& dsuJugador,
                         const sf::Vector2f* posIslas,
                         int islaActual,
                         int outOpc[2],
                         int outDest[2],
                         bool outBuildable[2])
{
    const auto& puentes = grafo.obtenerPuentes();
    const int m = puentes.tamano();
    int* idxs = new int[m];
    float* ds = new float[m];
    int* dest = new int[m];
    int cnt = 0;
    for(int i=0;i<m;++i){
        int a=puentes[i].obtenerIslaA(), b=puentes[i].obtenerIslaB();
        if(a!=islaActual && b!=islaActual) continue;
        if(ownersJugador[i]!=0) continue;
        if(dsuJugador.encontrar(a)==dsuJugador.encontrar(b)) continue;
        int otro=(a==islaActual)?b:a;
        idxs[cnt]=i; dest[cnt]=otro; ds[cnt]=dist2(posIslas[islaActual],posIslas[otro]); ++cnt;
    }
    int take = (cnt<4)?cnt:4;
    for(int t=0;t<take;++t){
        int best=t; for(int j=t+1;j<cnt;++j) if(ds[j]<ds[best]) best=j;
        int it=idxs[t]; idxs[t]=idxs[best]; idxs[best]=it;
        float ft=ds[t]; ds[t]=ds[best]; ds[best]=ft;
        int dt=dest[t]; dest[t]=dest[best]; dest[best]=dt;
        outOpc[t]=idxs[t]; outDest[t]=dest[t]; outBuildable[t]=true;
    }
    delete[] idxs; delete[] ds; delete[] dest;
    return take;
}

void UI::dibujarTurno(sf::RenderWindow& window,
                      const Graph& grafo,
                      const int* ownersJugador,
                      const sf::Vector2f* posIslas,
                      const int* opciones, int numOpc,
                      int islaActual,
                      const sf::Font& font,
                      sf::Color colorPropio)
{
    const auto& islas   = grafo.obtenerIslas();
    const auto& puentes = grafo.obtenerPuentes();
    const int n = islas.tamano();
    const int m = puentes.tamano();

    for(int i=0;i<m;++i){
        if(ownersJugador[i]){
            int a=puentes[i].obtenerIslaA(), b=puentes[i].obtenerIslaB();
            sf::Vertex own[] = { sf::Vertex(posIslas[a], colorPropio), sf::Vertex(posIslas[b], colorPropio) };
            window.draw(own,2,sf::Lines);
            sf::Text t; t.setFont(font); t.setCharacterSize(16);
            t.setFillColor(sf::Color(255,255,0));
            t.setString(std::to_string(puentes[i].obtenerCosto()));
            sf::Vector2f mid=(posIslas[a]+posIslas[b])*0.5f; t.setPosition(mid.x+6.f,mid.y+6.f);
            window.draw(t);
        }
    }

    for(int k=0;k<numOpc;++k){
        int i=opciones[k];
        int a=puentes[i].obtenerIslaA(), b=puentes[i].obtenerIslaB();
        sf::Vertex opv[] = { sf::Vertex(posIslas[a], sf::Color(0,200,255)), sf::Vertex(posIslas[b], sf::Color(0,200,255)) };
        window.draw(opv,2,sf::Lines);
        sf::Text t; t.setFont(font); t.setCharacterSize(16);
        t.setFillColor(sf::Color(255,255,0));
        t.setString(std::to_string(puentes[i].obtenerCosto()));
        sf::Vector2f mid=(posIslas[a]+posIslas[b])*0.5f; t.setPosition(mid.x+6.f,mid.y+6.f);
        window.draw(t);
    }

    static sf::Clock clk; float s=std::fmod(clk.getElapsedTime().asSeconds(),1.f);
    bool on=(s<0.5f);
    for(int i=0;i<n;++i){
        sf::CircleShape circ(22.f); circ.setOrigin(22.f,22.f); circ.setPosition(posIslas[i]);
        circ.setFillColor((i==islaActual&&on)?sf::Color(0,120,255):sf::Color(30,30,30));
        circ.setOutlineThickness(2.f); circ.setOutlineColor(sf::Color(200,200,200)); window.draw(circ);
        sf::Text name; name.setFont(font); name.setCharacterSize(16); name.setFillColor(sf::Color(230,230,230));
        name.setString(islas[i].obtenerNombre()); name.setPosition(posIslas[i].x+18.f,posIslas[i].y-10.f); window.draw(name);
    }
}
