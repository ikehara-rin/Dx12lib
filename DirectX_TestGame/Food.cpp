#include "Food.h"
#include <chrono>
#include <cstdlib>
#include <ctime>
#include "SpriteRenderer.h"  // DrawCell() ‚ð—˜—p

Food::Food(int fieldWidth, int fieldHeight)
    : fieldWidth(fieldWidth), fieldHeight(fieldHeight) 
{
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    Respawn();
}

void Food::Respawn() {
    x = std::rand() % fieldWidth;
    y = std::rand() % fieldHeight;
}

void Food::SetPosition(int x, int y) {
    this->x = x;
    this->y = y;
}