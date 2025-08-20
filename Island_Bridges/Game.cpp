#include "Game.hpp"
#include "UI.hpp"
#include <cmath>
#include <iostream>
#include <string>
#include <cstdio>
#include <cstdlib>

static float ISLA_H = 160.f;
static const float BRIDGE_H = 200.f;

struct WavesBackground {
    sf::Vector2u size;
    sf::VertexArray l1, l2, l3;
    WavesBackground(sf::Vector2u s) : size(s), l1(sf::TriangleStrip), l2(sf::TriangleStrip), l3(sf::TriangleStrip) {}
    void strip(sf::VertexArray& va,float baseY,float amp,float k,float ph,float t,sf::Color top,sf::Color bot){
        va.clear(); const int cols=220; float W=(float)size.x,H=(float)size.y,dx=W/cols;
        for(int i=0;i<=cols;++i){ float x=i*dx; float y=baseY+amp*std::sin(k*x+ph+t);
            va.append(sf::Vertex({x,y},top)); va.append(sf::Vertex({x,H},bot)); }
    }
    void update(float t){
        sf::Color c1(90,190,255,160), c2(60,170,240,140), c3(40,150,220,120), b(20,120,200,100);
        strip(l1,size.y*0.55f,18.f,0.010f,0.0f,  t*0.9f,c1,b);
        strip(l2,size.y*0.60f,28.f,0.008f,1.7f,  t*0.6f,c2,b);
        strip(l3,size.y*0.65f,38.f,0.0065f,3.2f, t*0.4f,c3,b);
    }
    void draw(sf::RenderWindow& w){ w.draw(l3); w.draw(l2); w.draw(l1); }
};

struct Firefly { sf::Vector2f p,v; float ph,amp; };
struct FireflySystem {
    Firefly* a; int n; sf::Vector2u S;
    FireflySystem(int N,sf::Vector2u s):n(N),S(s){ a=new Firefly[n];
        for(int i=0;i<n;++i){ a[i].p={(float)(std::rand()% (int)S.x),(float)(std::rand()% (int)S.y)*0.9f};
            float ang=(std::rand()%628)/100.f, sp=12.f+(std::rand()%30);
            a[i].v={std::cos(ang)*sp,std::sin(ang)*sp*0.3f}; a[i].ph=(std::rand()%628)/100.f; a[i].amp=0.6f+(std::rand()%40)/100.f;} }
    ~FireflySystem(){ delete[] a; }
    void update(float dt){ for(int i=0;i<n;++i){ a[i].ph+=dt*2.4f; a[i].p+=a[i].v*dt;
        if(a[i].p.x<0) a[i].p.x+=S.x; if(a[i].p.x>S.x) a[i].p.x-=S.x;
        if(a[i].p.y<0) a[i].p.y+=S.y*0.9f; if(a[i].p.y>S.y*0.9f) a[i].p.y-=S.y*0.9f; } }
    void draw(sf::RenderWindow& w){ for(int i=0;i<n;++i){ float pu=0.5f+0.5f*std::sin(a[i].ph);
        sf::CircleShape d(2.2f+1.5f*pu*a[i].amp); d.setOrigin(d.getRadius(),d.getRadius());
        d.setPosition(a[i].p); d.setFillColor(sf::Color(255,250,170,(sf::Uint8)(140+115*pu))); w.draw(d);} }
};

static inline void endpointsOnEdge(const sf::Vector2f& A, const sf::Vector2f& B, sf::Vector2f& Aout, sf::Vector2f& Bout){
    sf::Vector2f d = {B.x - A.x, B.y - A.y};
    float len = std::sqrt(d.x*d.x + d.y*d.y);
    if (len < 1.f) { Aout = A; Bout = B; return; }
    sf::Vector2f u = { d.x/len, d.y/len };
    float R = ISLA_H * 0.32f;
    Aout = { A.x + u.x*R, A.y + u.y*R };
    Bout = { B.x - u.x*R, B.y - u.y*R };
}

static void drawOwners(sf::RenderWindow& w, const Graph& grafo, const sf::Vector2f* posIslas,
                       const int* owners, const float* alphaArr, const float* buildT,
                       sf::Color tint, const sf::Font& font){
    static sf::Texture tex; static bool ok=false;
    if(!ok){ ok = tex.loadFromFile("imagenes/puente.png"); tex.setSmooth(false); }
    const auto& puentes = grafo.obtenerPuentes();
    for (int i=0;i<puentes.tamano();++i){
        if (!owners[i]) continue;
        int a=puentes[i].obtenerIslaA(), b=puentes[i].obtenerIslaB();
        sf::Vector2f A=posIslas[a], B=posIslas[b], Aedge, Bedge;
        endpointsOnEdge(A,B,Aedge,Bedge);
        sf::Vector2f d={Bedge.x-Aedge.x, Bedge.y-Aedge.y};
        float len=std::sqrt(d.x*d.x+d.y*d.y);
        float ang = std::atan2(d.y,d.x)*180.f/3.14159265f;
        float alpha = std::max(0.f,std::min(255.f, alphaArr ? alphaArr[i] : 255.f));
        sf::Color c=tint; c.a=(sf::Uint8)alpha;
        sf::Sprite s(tex);
        sf::Vector2u ts = tex.getSize();
        s.setOrigin(0.f, ts.y*0.5f);
        s.setScale(len/(float)ts.x, BRIDGE_H/(float)ts.y);
        s.setPosition(Aedge);
        s.setRotation(ang);
        s.setColor(c);
        w.draw(s);

    }
}

static void drawOpciones(sf::RenderWindow& w, const Graph& grafo, const sf::Vector2f* posIslas, const int* opciones, int numOpc, const sf::Font& font){
    static sf::Texture tex; static bool ok=false;
    if(!ok){ ok = tex.loadFromFile("imagenes/puente.png"); tex.setSmooth(false); }
    static sf::Clock clk; float s=clk.getElapsedTime().asSeconds();
    float pulse = 0.65f + 0.35f*std::fabs(std::sin(s*2.2f));
    sf::Uint8 a = (sf::Uint8)(130 + 90*pulse);
    const auto& puentes=grafo.obtenerPuentes();
    for(int k=0;k<numOpc;++k){
        int i=opciones[k];
        int aId=puentes[i].obtenerIslaA(), bId=puentes[i].obtenerIslaB();
        sf::Vector2f A=posIslas[aId], B=posIslas[bId], Aedge, Bedge;
        endpointsOnEdge(A,B,Aedge,Bedge);
        sf::Vector2f d={Bedge.x-Aedge.x, Bedge.y-Aedge.y};
        float len=std::sqrt(d.x*d.x+d.y*d.y);
        float ang=std::atan2(d.y,d.x)*180.f/3.14159265f;
        sf::Sprite s(tex);
        sf::Vector2u ts = tex.getSize();
        s.setOrigin(0.f, ts.y*0.5f);
        s.setScale(len/(float)ts.x, BRIDGE_H/(float)ts.y);
        s.setPosition(Aedge);
        s.setRotation(ang);
        s.setColor(sf::Color(255,255,255,a));
        w.draw(s);


    }
}

static void drawIslas(sf::RenderWindow& w, const Graph& grafo, const sf::Vector2f* posIslas, int islaActual, const sf::Font& font){
    static sf::Texture tex; static bool ok=false; if(!ok){ ok = tex.loadFromFile("imagenes/isla.png"); tex.setSmooth(false); }
    const auto& islas=grafo.obtenerIslas(); int n=islas.tamano();
    static sf::Clock clk; float t=clk.getElapsedTime().asSeconds();
    float pulso=1.0f + 0.10f*std::sin(t*6.0f);
    for(int i=0;i<n;++i){
        sf::Sprite sp(tex);
        sf::FloatRect r=sp.getLocalBounds();
        float sc = ISLA_H / r.height;
        if(i==islaActual) sp.setScale(sc*pulso, sc*pulso); else sp.setScale(sc, sc);
        sp.setOrigin(r.width*0.5f, r.height*0.5f);
        sp.setPosition(posIslas[i]);
        sp.setColor(i==islaActual ? sf::Color(255,255,255,255) : sf::Color(255,255,255,220));
        w.draw(sp);
        sf::Text name; name.setFont(font); name.setCharacterSize(16);
        name.setFillColor(sf::Color(230,230,230));
        name.setString(islas[i].obtenerNombre());
        name.setPosition(posIslas[i].x + ISLA_H*0.45f, posIslas[i].y - ISLA_H*0.35f);
        w.draw(name);
    }
}

Game::Game()
: ownersJ1(nullptr), ownersJ2(nullptr),
  dsuJ1(nullptr), dsuJ2(nullptr),
  aristasJ1(0), aristasJ2(0), costoJ1(0), costoJ2(0),
  costoOptimo(0),
  posIslas(nullptr),
  currentIslaJ1(0), currentIslaJ2(0)
{
    totalNiveles = 3;
    nivelActual  = 1;
    cargarNivel(nivelActual);
}

Game::~Game() {
    delete dsuJ1; dsuJ1 = nullptr;
    delete dsuJ2; dsuJ2 = nullptr;
    delete[] ownersJ1; ownersJ1 = nullptr;
    delete[] ownersJ2; ownersJ2 = nullptr;
    delete[] posIslas; posIslas = nullptr;
}

void Game::reiniciarEstructuras() {
    delete dsuJ1; dsuJ1 = nullptr;
    delete dsuJ2; dsuJ2 = nullptr;
    delete[] ownersJ1; ownersJ1 = nullptr;
    delete[] ownersJ2; ownersJ2 = nullptr;
    int n = grafo.obtenerIslas().tamano();
    int m = grafo.obtenerPuentes().tamano();
    dsuJ1 = new UnionFind(n);
    dsuJ2 = new UnionFind(n);
    ownersJ1 = new int[m];
    ownersJ2 = new int[m];
    for (int i = 0; i < m; ++i) { ownersJ1[i] = 0; ownersJ2[i] = 0; }
    aristasJ1 = aristasJ2 = 0;
    costoJ1   = costoJ2   = 0;
    currentIslaJ1 = currentIslaJ2 = 0;
    delete[] posIslas; posIslas = nullptr;
    posIslas = new sf::Vector2f[n];
}

bool Game::cargarNivel(int idx) {
    std::string ruta = "niveles/nivel" + std::to_string(idx) + ".txt";
    grafo.cargarDesdeArchivo(ruta);
    reiniciarEstructuras();
    costoOptimo = calcularCostoMST();
    inicializarPosiciones(grafo.obtenerIslas().tamano());
    debugGradosIniciales();
    return true;
}

float Game::distPuntoSegmento(const sf::Vector2f& p, const sf::Vector2f& a, const sf::Vector2f& b) const {
    sf::Vector2f ab = { b.x - a.x, b.y - a.y };
    sf::Vector2f ap = { p.x - a.x, p.y - a.y };
    float ab2 = ab.x*ab.x + ab.y*ab.y;
    float t = (ab2 > 0.f) ? (ap.x*ab.x + ap.y*ab.y) / ab2 : 0.f;
    if (t < 0.f) t = 0.f; else if (t > 1.f) t = 1.f;
    sf::Vector2f h = { a.x + t*ab.x, a.y + t*ab.y };
    float dx = p.x - h.x, dy = p.y - h.y;
    return std::sqrt(dx*dx + dy*dy);
}

bool Game::clickSobrePuente(int mx, int my, int& idxClic) const {
    const auto& puentes = grafo.obtenerPuentes();
    const float TOL = 10.f;
    sf::Vector2f p((float)mx,(float)my);
    int best = -1; float bestd = 1e9f;
    for (int i = 0; i < puentes.tamano(); ++i) {
        int a = puentes[i].obtenerIslaA();
        int b = puentes[i].obtenerIslaB();
        float d = distPuntoSegmento(p, posIslas[a], posIslas[b]);
        if (d < TOL && d < bestd) { bestd = d; best = i; }
    }
    idxClic = best;
    return best != -1;
}

bool Game::clickSobreIsla(int mx, int my, int& islaId) const {
    const int n = grafo.obtenerIslas().tamano();
    float R = ISLA_H * 0.48f;
    float R2 = R * R;
    sf::Vector2f p((float)mx, (float)my);
    for (int i = 0; i < n; ++i) {
        float dx = p.x - posIslas[i].x;
        float dy = p.y - posIslas[i].y;
        if (dx*dx + dy*dy <= R2) { islaId = i; return true; }
    }
    islaId = -1;
    return false;
}

void Game::inicializarPosiciones(int n) {
    sf::VideoMode dm = sf::VideoMode::getDesktopMode();
    const float sx = dm.width  / 1024.f;
    const float sy = dm.height / 720.f;
    auto put = [&](int i, float x, float y){ posIslas[i] = { x*sx, y*sy }; };
    if (nivelActual == 1 && n == 5) { put(0,800,360); put(1,650,560); put(2,320,520); put(3,260,220); put(4,580,160); return; }
    if (nivelActual == 2 && n == 10) {
        float t[10][2]={{820,380},{720,560},{540,630},{320,560},{210,380},{240,180},{460,120},{680,180},{900,240},{120,300}};
        for (int i=0;i<10;++i) put(i,t[i][0],t[i][1]); return;
    }
    if (nivelActual == 3 && n == 15) {
        float t[15][2]={{860,360},{760,580},{580,650},{380,620},{220,520},{160,360},{220,200},{380,120},{600,140},{780,220},{940,300},{480,300},{520,480},{360,440},{680,420}};
        for (int i=0;i<15;++i) put(i,t[i][0],t[i][1]); return;
    }
    const float R = 280.f; const sf::Vector2f C(512.f*sx, 360.f*sy);
    for (int i=0;i<n;++i){
        float ang = i * 6.2831853f / n;
        posIslas[i] = { C.x + R*sx*std::cos(ang), C.y + R*sy*std::sin(ang) };
    }
}

int Game::generarOpciones(int islaActual, bool esJ1, int* opc, int maxOpc) const {
    const auto& puentes = grafo.obtenerPuentes();
    const int m = puentes.tamano();
    UnionFind* dsu = esJ1 ? dsuJ1 : dsuJ2;
    int cnt = 0;
    for (int i = 0; i < m && cnt < maxOpc; ++i) {
        int a = puentes[i].obtenerIslaA();
        int b = puentes[i].obtenerIslaB();
        if (a == islaActual || b == islaActual) {
            int otro = (a == islaActual) ? b : a;
            if (dsu->encontrar(a) == dsu->encontrar(b)) continue;
            if (esJ1 ? (ownersJ1[i] != 0) : (ownersJ2[i] != 0)) continue;
            opc[cnt] = i;
            cnt++;
        }
    }
    return cnt;
}

void Game::run() {
    while (true) {
        const int n = grafo.obtenerIslas().tamano();
        sf::VideoMode dm = sf::VideoMode::getDesktopMode();
        sf::RenderWindow window(dm, ("Island Bridges — Nivel " + std::to_string(nivelActual)).c_str(), sf::Style::Fullscreen);
        window.setFramerateLimit(60);
        sf::Font font;
        font.loadFromFile("fuentes/PressStart2P-Regular.ttf");
        WavesBackground waves(window.getSize());
        FireflySystem flies(90, window.getSize());
        sf::Clock clk; float last = clk.getElapsedTime().asSeconds();
        sf::Texture texJ1, texJ2; texJ1.loadFromFile("imagenes/jugador1.png"); texJ2.loadFromFile("imagenes/jugador2.png");
        sf::Sprite sprJ1(texJ1), sprJ2(texJ2);
        float h = 220.f;
        sf::FloatRect b1 = sprJ1.getLocalBounds(), b2 = sprJ2.getLocalBounds();
        sprJ1.setScale(h/b1.height, h/b1.height); sprJ2.setScale(h/b2.height, h/b2.height);
        auto S = window.getSize();
        float gap = 12.f;
        sf::FloatRect g1 = sprJ1.getGlobalBounds(); sf::FloatRect g2 = sprJ2.getGlobalBounds();
        sprJ2.setPosition((float)S.x - 10.f - g2.width, 10.f);
        sprJ1.setPosition(sprJ2.getPosition().x - gap - g1.width, 10.f);
        const auto& puentes = grafo.obtenerPuentes();
        int m = puentes.tamano();
        float* alphaJ1 = new float[m]; float* alphaJ2 = new float[m]; float* buildT = new float[m];
        for (int i = 0; i < m; ++i) { alphaJ1[i] = 0.f; alphaJ2[i] = 0.f; buildT[i] = -1.f; }
        bool turnoJ1 = true;
        int* opciones = new int[m];
        auto regenOpc = [&]() {
            return generarOpciones(
                turnoJ1 ? currentIslaJ1 : currentIslaJ2,
                turnoJ1,
                opciones,
                m
            );
        };
        int numOpc = regenOpc();
        auto ambosListos = [&]() { return (aristasJ1 >= n-1 && aristasJ2 >= n-1); };
        while (window.isOpen() && !ambosListos()) {
            sf::Event e;
            while (window.pollEvent(e)) {
                if (e.type == sf::Event::Closed) { window.close(); }
                if (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::Escape) { window.close(); }
                if (e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Left) {
                    int islaClick;
                    if (!clickSobreIsla(e.mouseButton.x, e.mouseButton.y, islaClick)) continue;
                    int islaActual = turnoJ1 ? currentIslaJ1 : currentIslaJ2;
                    if (numOpc == 0) {

                        if (turnoJ1) currentIslaJ1 = islaClick;
                        else currentIslaJ2 = islaClick;
                        numOpc = regenOpc();
                        continue;
                    }

                    int kSel = -1;
                    for (int k = 0; k < numOpc; ++k) {
                        int idx = opciones[k];
                        int a = puentes[idx].obtenerIslaA();
                        int b = puentes[idx].obtenerIslaB();
                        int destino = (a == islaActual) ? b : a;
                        if (destino == islaClick) {
                            kSel = k;
                            break;
                        }
                    }
                    if (kSel == -1) continue;


                    int idx = opciones[kSel];
                    int a = puentes[idx].obtenerIslaA();
                    int b = puentes[idx].obtenerIslaB();
                    int c = puentes[idx].obtenerCosto();
                    UnionFind* dsu = turnoJ1 ? dsuJ1 : dsuJ2;
                    if (dsu->encontrar(a) != dsu->encontrar(b)) {
                        dsu->unir(a, b);
                        if (turnoJ1) {
                            ownersJ1[idx] = 1;
                            costoJ1 += c;
                            aristasJ1++;
                            alphaJ1[idx] = 0.f;
                            buildT[idx] = 0.f;
                            currentIslaJ1 = (a == islaActual) ? b : a;
                        } else {
                            ownersJ2[idx] = 2;
                            costoJ2 += c;
                            aristasJ2++;
                            alphaJ2[idx] = 0.f;
                            buildT[idx] = 0.f;
                            currentIslaJ2 = (a == islaActual) ? b : a;
                        }
                        turnoJ1 = !turnoJ1;
                        numOpc = regenOpc();
                    }
                }
            }
            float now = clk.getElapsedTime().asSeconds();
            float dt = now - last; last = now;
            waves.update(now);
            flies.update(dt);
            for (int i = 0; i < m; ++i) {
                if (buildT[i] >= 0.f) buildT[i] += dt;
                if (ownersJ1[i]) alphaJ1[i] = std::min(255.f, alphaJ1[i] + 420.f*dt);
                if (ownersJ2[i]) alphaJ2[i] = std::min(255.f, alphaJ2[i] + 420.f*dt);
            }
            sf::Text hud; hud.setFont(font);
            hud.setCharacterSize(22);
            hud.setFillColor(sf::Color(20,40,60));
            hud.setPosition(24.f, 36.f);
            char buf[256];
            std::snprintf(buf, sizeof(buf),
                "Turno: J%d | MST:%d  |  J1 a:%d c:%d  |  J2 a:%d c:%d",
                turnoJ1?1:2, costoOptimo, aristasJ1, costoJ1, aristasJ2, costoJ2);
            hud.setString(buf);
            window.clear(sf::Color(140,210,255));
            waves.draw(window);
            flies.draw(window);
            sf::RectangleShape ov(sf::Vector2f((float)window.getSize().x, (float)window.getSize().y));
            ov.setFillColor(sf::Color(255,255,255,28));
            window.draw(ov);
            if (turnoJ1) drawOwners(window, grafo, posIslas, ownersJ1, alphaJ1, buildT, sf::Color(0,200,0), font);
            else drawOwners(window, grafo, posIslas, ownersJ2, alphaJ2, buildT, sf::Color(220,40,40), font);
            drawOpciones(window, grafo, posIslas, opciones, numOpc, font);
            drawIslas(window, grafo, posIslas, turnoJ1 ? currentIslaJ1 : currentIslaJ2, font);
            static sf::Clock clkPirata;
            float tp = clkPirata.getElapsedTime().asSeconds();
            float pulse = 0.55f + 0.45f*(0.5f + 0.5f*std::sin(tp*6.0f));
            if (turnoJ1) {
                sprJ1.setColor(sf::Color(255,255,255,(sf::Uint8)(255*pulse)));
                sprJ2.setColor(sf::Color(255,255,255,120));
            } else {
                sprJ1.setColor(sf::Color(255,255,255,120));
                sprJ2.setColor(sf::Color(255,255,255,(sf::Uint8)(255*pulse)));
            }
            window.draw(sprJ1);
            window.draw(sprJ2);
            if (numOpc == 0) {
                sf::Text noOps; noOps.setFont(font);
                noOps.setCharacterSize(18);
                noOps.setFillColor(sf::Color(35,45,60));
                noOps.setString("Sin opciones. Presiona sobre otra isla.");
                noOps.setPosition(24.f, 66.f);
                window.draw(noOps);
            }
            window.display();
        }
        window.close();
        delete[] alphaJ1; delete[] alphaJ2; delete[] buildT; delete[] opciones;
        std::cout << "\n=== Fin de nivel " << nivelActual << " ===\n";
        std::cout << "Jugador 1 -> aristas: " << aristasJ1 << "  costo: " << costoJ1 << "\n";
        std::cout << "Jugador 2 -> aristas: " << aristasJ2 << "  costo: " << costoJ2 << "\n";
        std::cout << "MST (óptimo): " << costoOptimo << "\n";
        bool haySiguiente = (nivelActual < totalNiveles);
        bool seguir = mostrarResultados(haySiguiente);
        if (haySiguiente && seguir) { ++nivelActual; cargarNivel(nivelActual); continue; }
        break;
    }
}
int Game::calcularCostoMST() const {
    const auto& puentes = grafo.obtenerPuentes();
    const int n = grafo.obtenerIslas().tamano();
    const int m = puentes.tamano();
    struct E { int a,b,c; };
    E* edges = new E[m];
    for (int i = 0; i < m; ++i) {
        edges[i].a = puentes[i].obtenerIslaA();
        edges[i].b = puentes[i].obtenerIslaB();
        edges[i].c = puentes[i].obtenerCosto();
    }
    for (int i = 0; i < m; ++i) {
        int minIdx = i;
        for (int j = i + 1; j < m; ++j) if (edges[j].c < edges[minIdx].c) minIdx = j;
        if (minIdx != i) { E tmp = edges[i]; edges[i] = edges[minIdx]; edges[minIdx] = tmp; }
    }
    UnionFind dsu(n);
    int total = 0, used = 0;
    for (int i = 0; i < m; ++i) {
        int a = edges[i].a, b = edges[i].b, c = edges[i].c;
        if (dsu.encontrar(a) != dsu.encontrar(b)) { dsu.unir(a, b); total += c; if (++used == n - 1) break; }
    }
    delete[] edges;
    return (used == n - 1) ? total : -1;
}

static void centerText(sf::Text& text, float x, float y) {
    sf::FloatRect bounds = text.getLocalBounds();
    text.setPosition(x - bounds.width * 0.5f, y);
}

bool Game::mostrarResultados(bool haySiguiente) const {
    sf::VideoMode dm = sf::VideoMode::getDesktopMode();
    sf::RenderWindow window(dm, "Resultados - Island Bridges", sf::Style::Fullscreen);
    sf::Font font;
    font.loadFromFile("fuentes/PressStart2P-Regular.ttf");
    int diffJ1 = (costoOptimo >= 0) ? std::abs(costoJ1 - costoOptimo) : -1;
    int diffJ2 = (costoOptimo >= 0) ? std::abs(costoJ2 - costoOptimo) : -1;
    std::string ganador = "Empate";
    if (diffJ1 >= 0 && diffJ2 >= 0) {
        if (diffJ1 < diffJ2) ganador = "Jugador 1";
        else if (diffJ2 < diffJ1) ganador = "Jugador 2";
    }

    auto makeText = [&](const std::string& s, unsigned size, float x, float y) {
        sf::Text t; t.setFont(font); t.setString(s);
        t.setCharacterSize(size); t.setFillColor(sf::Color(230,230,230));
        t.setPosition(x, y); return t;
    };

    auto S = window.getSize();

    sf::Text title = makeText("Resumen final del nivel " + std::to_string(nivelActual), (unsigned)(S.y*0.06f), 0, 80.f);
    title.setFillColor(sf::Color(255, 255, 0));
    centerText(title, S.x * 0.5f, title.getPosition().y);

    sf::Text player1Title = makeText("Jugador 1", (unsigned)(S.y*0.045f), S.x * 0.25f, 200.f);
    centerText(player1Title, player1Title.getPosition().x, player1Title.getPosition().y);

    sf::Text player1Score = makeText("Costo: " + std::to_string(costoJ1), (unsigned)(S.y*0.04f), S.x * 0.25f, 250.f);
    centerText(player1Score, player1Score.getPosition().x, player1Score.getPosition().y);

    std::string sDiff1 = (diffJ1 >= 0) ? std::to_string(diffJ1) : "N/A";
    sf::Text player1Diff = makeText("Diferencia: " + sDiff1, (unsigned)(S.y*0.035f), S.x * 0.25f, 290.f);
    centerText(player1Diff, player1Diff.getPosition().x, player1Diff.getPosition().y);

    sf::Text player2Title = makeText("Jugador 2", (unsigned)(S.y*0.045f), S.x * 0.75f, 200.f);
    centerText(player2Title, player2Title.getPosition().x, player2Title.getPosition().y);

    sf::Text player2Score = makeText("Costo: " + std::to_string(costoJ2), (unsigned)(S.y*0.04f), S.x * 0.75f, 250.f);
    centerText(player2Score, player2Score.getPosition().x, player2Score.getPosition().y);

    std::string sDiff2 = (diffJ2 >= 0) ? std::to_string(diffJ2) : "N/A";
    sf::Text player2Diff = makeText("Diferencia: " + sDiff2, (unsigned)(S.y*0.035f), S.x * 0.75f, 290.f);
    centerText(player2Diff, player2Diff.getPosition().x, player2Diff.getPosition().y);

    sf::Text optTitle = makeText("Costo óptimo (MST):", (unsigned)(S.y*0.035f), S.x * 0.5f, 360.f);
    centerText(optTitle, optTitle.getPosition().x, optTitle.getPosition().y);

    sf::Text optScore = makeText(std::to_string(costoOptimo), (unsigned)(S.y*0.05f), S.x * 0.5f, 400.f);
    optScore.setFillColor(sf::Color(0, 255, 0));
    centerText(optScore, optScore.getPosition().x, optScore.getPosition().y);

    sf::Text win = makeText("Ganador: " + ganador, (unsigned)(S.y*0.045f), 0, 500.f);
    win.setFillColor(sf::Color(255, 100, 100));
    centerText(win, S.x * 0.5f, win.getPosition().y);

    sf::Text hint = makeText(haySiguiente ?
                              "Clic en SEGUIR para avanzar, o ESC para terminar." :
                              "Presiona ESC o cierra la ventana para terminar.",
                              (unsigned)(S.y*0.026f), 40.f, (float)S.y - 64.f);

    sf::RectangleShape btnSeguir;
    sf::Text            txtSeguir;
    bool hover = false;
    if (haySiguiente) {
        btnSeguir.setSize({220.f, 64.f});
        btnSeguir.setPosition({S.x * 0.5f - 110.f, 580.f});
        btnSeguir.setFillColor(sf::Color(60,60,60));
        btnSeguir.setOutlineThickness(2.f);
        btnSeguir.setOutlineColor(sf::Color(180,180,180));
        txtSeguir = makeText("SEGUIR", (unsigned)(S.y*0.034f), 0, 580.f + 16.f);
        centerText(txtSeguir, S.x * 0.5f, txtSeguir.getPosition().y);
    }

    while (window.isOpen()) {
        sf::Event e;
        while (window.pollEvent(e)) {
            if (e.type == sf::Event::Closed) { window.close(); return false; }
            if (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::Escape) { window.close(); return false; }
            if (haySiguiente) {
                if (e.type == sf::Event::MouseMoved) {
                    float mx = (float)e.mouseMove.x, my = (float)e.mouseMove.y;
                    hover = btnSeguir.getGlobalBounds().contains(mx, my);
                    btnSeguir.setFillColor(hover ? sf::Color(90,90,90) : sf::Color(60,60,60));
                }
                if (e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Left) {
                    float mx = (float)e.mouseButton.x, my = (float)e.mouseButton.y;
                    if (btnSeguir.getGlobalBounds().contains(mx, my)) { window.close(); return true; }
                }
            }
        }
        window.clear(sf::Color::Black);
        window.draw(title);
        window.draw(player1Title);
        window.draw(player1Score);
        window.draw(player1Diff);
        window.draw(player2Title);
        window.draw(player2Score);
        window.draw(player2Diff);
        window.draw(optTitle);
        window.draw(optScore);
        window.draw(win);
        window.draw(hint);
        if (haySiguiente) { window.draw(btnSeguir); window.draw(txtSeguir); }
        window.display();
    }
    return false;
}

void Game::debugGradosIniciales() const {
    const auto& islas   = grafo.obtenerIslas();
    const auto& puentes = grafo.obtenerPuentes();
    int n = islas.tamano();
    int m = puentes.tamano();
    int* grado = new int[n];
    for (int i=0;i<n;++i) grado[i]=0;
    for (int i=0;i<m;++i) {
        int a = puentes[i].obtenerIslaA();
        int b = puentes[i].obtenerIslaB();
        if (a>=0 && a<n) grado[a]++;
        if (b>=0 && b<n) grado[b]++;
    }
    std::cout << "\n[DEBUG] Nivel " << nivelActual << " — grados iniciales:\n";
    for (int i=0;i<n;++i) {
        std::cout << "  Isla " << islas[i].obtenerNombre() << " (id " << i << ") -> grado=" << grado[i];
        if (grado[i] < 3) std::cout << "  <3 (agregar conexiones)";
        std::cout << "\n";
    }
    delete[] grado;
}

void Game::debugOpcionesPorIsla(bool esJ1) const {
    const auto& islas   = grafo.obtenerIslas();
    const auto& puentes = grafo.obtenerPuentes();
    int n = islas.tamano();
    int m = puentes.tamano();
    const int* owners = esJ1 ? ownersJ1 : ownersJ2;
    UnionFind* dsu    = esJ1 ? dsuJ1    : dsuJ2;
    std::cout << "\n[DEBUG] Opciones válidas por isla (jugador " << (esJ1?1:2) << "):\n";
    for (int isla = 0; isla < n; ++isla) {
        int cnt = 0;
        for (int i = 0; i < m; ++i) {
            int a = puentes[i].obtenerIslaA();
            int b = puentes[i].obtenerIslaB();
            if (a != isla && b != isla) continue;
            if (owners[i] != 0) continue;
            if (dsu->encontrar(a) == dsu->encontrar(b)) continue;
            cnt++;
        }
        std::cout << "  " << islas[isla].obtenerNombre() << " (id " << isla << ") -> opciones=" << cnt << "\n";
    }
}

