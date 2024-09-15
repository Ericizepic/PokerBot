#include "game.h"

int main()
{
 Game* game = new Game();
 game->train(100);
    std::vector<std::string> hist = {"c", "c", "|", "c", "c", "|", "c", "c", "|", "c", "r50", "r50", "r50",  "r50",  "r50"};
    // std::vector<std::string> hist = {"c", "c", "|", "c", "c", "|", "c", "c", "|", "c", "r50", "f"};
    // game->deck = new Deck();
    // game->get_utility(hist);
}