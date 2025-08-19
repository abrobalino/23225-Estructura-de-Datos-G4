#include "Game.hpp"
#include "Screens.hpp"

int main() {
    if (!showMainMenu()) return 0;
    Game g;
    g.run();
    return 0;
}
