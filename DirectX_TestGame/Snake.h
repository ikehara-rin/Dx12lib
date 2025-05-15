#pragma once
#include <deque>
#include <vector>
#include <DirectXMath.h>
#include "Direction.h"

class SpriteRenderer;
class Food;

class Snake {
public:
    Snake(int fieldWidth, int fieldHeight,float cellSize);
    void Update();
    void Grow();
    /*void ChangeDirection(Direction newDir);*/
    const std::deque<DirectX::XMINT2>& GetSegments() const;
    bool CheckCollision(const int x,const int y) const;
    bool CheckSelfCollision() const;
    /*void Draw(SpriteRenderer* renderer, ID3D12GraphicsCommandList* commandList);*/
    int GetHeadX() const;
    int GetHeadY() const;
    void SetDirection(int dx, int dy);

private:
    struct Segment {
        int x, y;
    };
    std::deque<DirectX::XMINT2> segments;
    Direction currentDirection;
    int fieldWidth;
    int fieldHeight;

    std::vector<Segment> body;
    int dirX, dirY;
    int gridWidth, gridHeight;
    float cellSize;

};
