#include "game.h"

int main()
{
    Game* game = new Game();
    game->train(1e5);

    delete game;
}