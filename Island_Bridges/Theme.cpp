#include "Theme.hpp"
#include <iostream>

static const char* CANDIDATES[] = {
    "C:\\\\Windows\\\\Fonts\\\\bahnschrift.ttf",
    "C:\\\\Windows\\\\Fonts\\\\segoeui.ttf",
    "C:\\\\Windows\\\\Fonts\\\\trebuc.ttf",
    "C:\\\\Windows\\\\Fonts\\\\verdana.ttf",
    "C:\\\\Windows\\\\Fonts\\\\gabriola.ttf",
    nullptr
};

bool loadGameFont(sf::Font& font) {
    for (int i=0; CANDIDATES[i]!=nullptr; ++i) {
        if (font.loadFromFile(CANDIDATES[i])) return true;
    }
    std::cerr << "[WARN] No se pudieron cargar fuentes del sistema. Usando intento por defecto (arial.ttf)\n";
    return font.loadFromFile("fuentes/arial.ttf");
}
