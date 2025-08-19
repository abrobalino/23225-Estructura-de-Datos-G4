#include "Screens.hpp"
#include <SFML/Graphics.hpp>
#include <cmath>
#include <string>
#include <iostream>


static void centerText(sf::Text& t, float x, float y) {
    sf::FloatRect b = t.getLocalBounds();
    t.setOrigin(b.left + b.width/2.f, b.top + b.height/2.f);
    t.setPosition(x, y);
}

static bool loadPixelFont(sf::Font& f) {
    return f.loadFromFile("fuentes/PressStart2P-Regular.ttf");
}

struct SimpleButton {
    sf::RectangleShape box;
    sf::Text label;
    bool hover = false;

    void init(const sf::Font& font, const std::string& txt, sf::Vector2f pos, sf::Vector2f size) {
        box.setSize(size);
        box.setOrigin(size.x/2.f, size.y/2.f);
        box.setPosition(pos);
        box.setFillColor(sf::Color(124, 94, 25));
        box.setOutlineThickness(3.f);
        box.setOutlineColor(sf::Color(201, 164, 63));
        label.setFont(font);
        label.setString(txt);
        label.setCharacterSize(28);
        label.setFillColor(sf::Color(255, 240, 200));
        centerText(label, pos.x, pos.y - 2.f);
    }

    void updateHover(float mx, float my) {
        hover = box.getGlobalBounds().contains(mx, my);
        box.setFillColor(hover ? sf::Color(168, 130, 42) : sf::Color(124, 94, 25));
        label.setScale(hover ? 1.05f : 1.f, hover ? 1.05f : 1.f);
    }

    bool clicked(float mx, float my) const {
        return box.getGlobalBounds().contains(mx, my);
    }

    void draw(sf::RenderWindow& w) {
        w.draw(box);
        w.draw(label);
    }
};

struct WavesBackground {
    sf::Vector2u size;
    sf::VertexArray layer1, layer2, layer3;
    WavesBackground(sf::Vector2u s)
    : size(s), layer1(sf::TriangleStrip), layer2(sf::TriangleStrip), layer3(sf::TriangleStrip) {}

    void buildStrip(sf::VertexArray& strip, float baseY, float amp, float k, float phase, float t, sf::Color top, sf::Color bottom) {
        strip.clear();
        const float W = (float)size.x;
        const float H = (float)size.y;
        const int cols = 220;
        const float dx = W / cols;
        for (int i=0;i<=cols;++i) {
            float x = i * dx;
            float y = baseY + amp * std::sin(k*x + phase + t);
            sf::Vertex vTop(sf::Vector2f(x, y), top);
            sf::Vertex vBot(sf::Vector2f(x, H), bottom);
            strip.append(vTop);
            strip.append(vBot);
        }
    }

    void update(float t) {
        sf::Color c1(90, 190, 255, 160);
        sf::Color c2(60, 170, 240, 140);
        sf::Color c3(40, 150, 220, 120);
        sf::Color b(20, 120, 200, 100);
        buildStrip(layer1, size.y*0.55f, 18.f, 0.010f, 0.0f,   t*0.9f,  c1, b);
        buildStrip(layer2, size.y*0.60f, 28.f, 0.008f, 1.7f,   t*0.6f,  c2, b);
        buildStrip(layer3, size.y*0.65f, 38.f, 0.0065f, 3.2f,  t*0.4f,  c3, b);
    }

    void draw(sf::RenderWindow& w) {
        w.draw(layer3);
        w.draw(layer2);
        w.draw(layer1);
    }
};

struct Firefly {
    sf::Vector2f pos;
    sf::Vector2f vel;
    float phase;
    float amp;
};

struct FireflySystem {
    Firefly* ff;
    int count;
    sf::Vector2u size;

    FireflySystem(int n, sf::Vector2u s) : count(n), size(s) {
        ff = new Firefly[count];
        for (int i=0;i<count;++i) {
            ff[i].pos = { (float)(std::rand()% (int)s.x), (float)(std::rand()% (int)s.y) * 0.9f };
            float ang = (std::rand()%628) / 100.0f;
            float sp  = 12.f + (std::rand()%30);
            ff[i].vel = { std::cos(ang)*sp, std::sin(ang)*sp*0.3f };
            ff[i].phase = (std::rand()%628)/100.0f;
            ff[i].amp   = 0.6f + (std::rand()%40)/100.0f;
        }
    }
    ~FireflySystem(){ delete[] ff; }

    void update(float dt) {
        for (int i=0;i<count;++i) {
            ff[i].phase += dt * 2.4f;
            ff[i].pos   += ff[i].vel * dt;
            if (ff[i].pos.x < 0) ff[i].pos.x += size.x;
            if (ff[i].pos.x > size.x) ff[i].pos.x -= size.x;
            if (ff[i].pos.y < 0) ff[i].pos.y += size.y*0.9f;
            if (ff[i].pos.y > size.y*0.9f) ff[i].pos.y -= size.y*0.9f;
        }
    }

    void draw(sf::RenderWindow& w) {
        for (int i=0;i<count;++i) {
            float pulse = 0.5f + 0.5f*std::sin(ff[i].phase);
            sf::CircleShape dot(2.2f + 1.5f*pulse*ff[i].amp);
            dot.setOrigin(dot.getRadius(), dot.getRadius());
            dot.setPosition(ff[i].pos);
            dot.setFillColor(sf::Color(255, 250, 170, (sf::Uint8)(140 + 115*pulse)));
            w.draw(dot);
        }
    }
};

static float bounceScale(float t) {
    const float omega = 6.0f, amp = 0.12f, decay = 0.9f;
    return 1.f + std::sin(omega*t) * amp * std::exp(-decay*t);
}

bool showMainMenu() {
    sf::VideoMode dm = sf::VideoMode::getDesktopMode();
    sf::RenderWindow window(dm, "Island Bridges — Menu", sf::Style::Fullscreen);
    window.setFramerateLimit(60);
    sf::Font font;
    if (!loadPixelFont(font)) std::cerr << "[WARN] No se pudo cargar fuentes/PressStart2P-Regular.ttf\n";
    auto S = window.getSize();
    float CX = S.x * 0.5f;
    float CY = S.y * 0.5f;
    WavesBackground waves(S);
    FireflySystem   flies(90, S);
    sf::Text title; title.setFont(font);
    title.setString("Island Bridges");
    title.setCharacterSize((unsigned)(S.y * 0.085f));
    title.setFillColor(sf::Color(15, 25, 35));
    centerText(title, CX+2.f, CY - S.y*0.225f + 2.f);
    sf::Text titleTop = title;
    titleTop.setFillColor(sf::Color(250,250,255));
    centerText(titleTop, CX, CY - S.y*0.225f);
    SimpleButton btnStart, btnInstr;
    float bw = std::min(380.f, S.x*0.30f);
    btnStart.init(font, "Empezar",       {CX, CY + 20.f},  {bw, 68.f});
    btnInstr.init(font, "Instrucciones", {CX, CY + 110.f}, {bw, 68.f});
    sf::RectangleShape overlay;
    overlay.setSize(sf::Vector2f((float)S.x, (float)S.y));
    overlay.setFillColor(sf::Color(255,255,255,30));
    sf::Clock clk;
    float last = clk.getElapsedTime().asSeconds();
    while (window.isOpen()) {
        sf::Event e;
        while (window.pollEvent(e)) {
            if (e.type == sf::Event::Closed) { window.close(); return false; }
            if (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::Escape) { window.close(); return false; }
            if (e.type == sf::Event::MouseMoved) {
                float mx = (float)e.mouseMove.x, my = (float)e.mouseMove.y;
                btnStart.updateHover(mx,my);
                btnInstr.updateHover(mx,my);
            }
            if (e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Left) {
                float mx = (float)e.mouseButton.x, my = (float)e.mouseButton.y;
                if (btnStart.clicked(mx,my)) { window.close(); return true; }
                if (btnInstr.clicked(mx,my)) { showInstructions(); }
            }
        }
        float now = clk.getElapsedTime().asSeconds();
        float dt  = now - last; last = now;
        waves.update(now);
        flies.update(dt);
        float s = bounceScale(now);
        title.setScale(s, s);
        titleTop.setScale(s, s);
        centerText(title,    CX+2.f, CY - S.y*0.225f + 2.f);
        centerText(titleTop, CX,     CY - S.y*0.225f);
        window.clear(sf::Color(140, 210, 255));
        waves.draw(window);
        flies.draw(window);
        window.draw(overlay);
        window.draw(title);
        window.draw(titleTop);
        sf::Text sub; sub.setFont(font); sub.setCharacterSize((unsigned)(S.y*0.028f));
        sub.setFillColor(sf::Color(20,40,60));
        sub.setString("Poly Bridge pero con Islas");
        centerText(sub, CX, CY - S.y*0.08f);
        window.draw(sub);
        btnStart.draw(window);
        btnInstr.draw(window);
        sf::Text hint; hint.setFont(font); hint.setCharacterSize((unsigned)(S.y*0.022f));
        hint.setFillColor(sf::Color(35,45,60));
        hint.setString("ESC para salir");
        centerText(hint, CX, S.y - 40.f);
        window.draw(hint);
        window.display();
    }
    return false;
}

void showInstructions() {
    sf::VideoMode dm = sf::VideoMode::getDesktopMode();
    sf::RenderWindow window(dm, "Island Bridges — Instrucciones", sf::Style::Fullscreen);
    window.setFramerateLimit(60);
    sf::Font font;
    if (!loadPixelFont(font)) std::cerr << "[WARN] No se pudo cargar fuentes/PressStart2P-Regular.ttf\n";
    auto S = window.getSize();
    WavesBackground waves(S);
    FireflySystem   flies(80, S);
    sf::Text title; title.setFont(font);
    title.setString("Como jugar al simulador de \n ser un Ing. Civil del mar:");
    title.setCharacterSize((unsigned)(S.y*0.06f));
    title.setFillColor(sf::Color(15,25,35));
    title.setPosition(40.f+2.f, 30.f+2.f);
    sf::Text titleTop = title; titleTop.setFillColor(sf::Color(250,250,255));
    titleTop.setPosition(40.f, 30.f);
    const char* lines[] = {
        " ",
        " ",
        "• Eres un pirata que se encuentra en la Isla A.",
        " ",
        "• La isla en la que te encuentras estara parpadeando.",
        " ",
        "• Haz click a la isla que deseas ir.",
        " ",
        "• Conecta hasta unir todas las islas.",
        " ",
        "• No puedes volver por tu propio puente.",
        " ",
        "• Sigue tu intuicion asi que Beuna Suerte.",
        " ",
        "• Se el mejor pirata",
        nullptr
    };
    SimpleButton btnBack; btnBack.init(font, "Atras", {110.f, (float)S.y - 70.f}, {200.f, 58.f});
    sf::Clock clk;
    float last = clk.getElapsedTime().asSeconds();
    while (window.isOpen()) {
        sf::Event e;
        while (window.pollEvent(e)) {
            if (e.type == sf::Event::Closed) { window.close(); return; }
            if (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::Escape) { window.close(); return; }
            if (e.type == sf::Event::MouseMoved) {
                float mx = (float)e.mouseMove.x, my = (float)e.mouseMove.y;
                btnBack.updateHover(mx,my);
            }
            if (e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Left) {
                float mx = (float)e.mouseButton.x, my = (float)e.mouseButton.y;
                if (btnBack.clicked(mx,my)) { window.close(); return; }
            }
        }
        float now = clk.getElapsedTime().asSeconds();
        float dt  = now - last; last = now;
        waves.update(now);
        flies.update(dt);
        window.clear(sf::Color(140,210,255));
        waves.draw(window);
        flies.draw(window);
        sf::RectangleShape panel(sf::Vector2f((float)S.x, (float)S.y));
        panel.setFillColor(sf::Color(255,255,255,40));
        window.draw(panel);
        window.draw(title);
        window.draw(titleTop);
        sf::Text t; t.setFont(font); t.setCharacterSize((unsigned)(S.y*0.035f));
        t.setFillColor(sf::Color(20,40,60));
        float yy = 120.f;
        for (int i=0; lines[i]!=nullptr; ++i) {
            t.setString(lines[i]);
            t.setPosition(60.f, yy);
            window.draw(t);
            yy += std::max(34.f, S.y*0.045f);
        }
        btnBack.draw(window);
        window.display();
    }
}
