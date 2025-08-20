#ifndef THEME_HPP
#define THEME_HPP

#include <SFML/Graphics.hpp>

bool loadGameFont(sf::Font& font);

namespace Theme {
    static const sf::Color bg      = sf::Color(11,15,26);
    static const sf::Color node    = sf::Color(30,30,30);
    static const sf::Color nodeHi  = sf::Color(0,120,255);
    static const sf::Color lineOwn = sf::Color(0,255,0);
    static const sf::Color lineOpt = sf::Color(0,200,255);
    static const sf::Color text    = sf::Color(230,230,240);
    static const sf::Color subtxt  = sf::Color(180,200,220);
    static const sf::Color hint    = sf::Color(150,160,170);
    static const sf::Color price   = sf::Color(255,255,0);
}

#endif
