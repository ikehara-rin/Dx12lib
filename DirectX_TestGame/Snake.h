#pragma once
#include <deque>
#include <DirectXMath.h>
#include "Direction.h"

class Snake {
public:
    Snake(int fieldWidth, int fieldHeight);
    void Update();
    void ChangeDirection(Direction newDir);
    const std::deque<DirectX::XMINT2>& GetSegments() const;
    bool CheckSelfCollision() const;
    int GetHeadX() const;
    int GetHeadY() const;

private:
    std::deque<DirectX::XMINT2> segments;
    Direction currentDirection;
    int fieldWidth;
    int fieldHeight;
};
