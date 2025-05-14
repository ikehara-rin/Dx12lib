#include "Food.h"
#include <chrono>
#include <cstdlib>
#include <ctime>
#include "SpriteRenderer.h"  // DrawCell() �𗘗p

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

void Food::Draw(ID3D12GraphicsCommandList* commandList, ID3D12Resource* texture, float cellSize) {
    // DrawCell(commandList, texture, x, y, cellSize) ���g�p
    DrawCell(commandList, texture, static_cast<float>(x), static_cast<float>(y), cellSize);
}