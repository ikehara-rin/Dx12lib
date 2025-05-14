#pragma once
#include <DirectXMath.h>
#include <random>
using namespace DirectX;

class Food {
public:
    Food(int fieldWidth, int fieldHeight);

    void Respawn();
    void Draw(ID3D12GraphicsCommandList* commandList, ID3D12Resource* texture, float cellSize);
    int GetX() const { return x; }
    int GetY() const { return y; }

private:
    int fieldWidth;
    int fieldHeight;
    int x, y;
    std::mt19937 rng;
};
