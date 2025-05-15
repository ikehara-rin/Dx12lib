#include "Snake.h"
#include "SpriteRenderer.h"
#include "Food.h"

Snake::Snake(int gridWidth, int gridHeight, float cellSize)
    : gridWidth(gridWidth), gridHeight(gridHeight), currentDirection(Direction::Right) ,dirX(1),cellSize(cellSize), dirY(0)
{
    body.push_back({ gridWidth / 2, gridHeight / 2 });
}

//void Snake::ChangeDirection(Direction newDir) {
//    // ‹t•ûŒü‹ÖŽ~
//    if ((currentDirection == Direction::Up && newDir != Direction::Down) ||
//        (currentDirection == Direction::Down && newDir != Direction::Up) ||
//        (currentDirection == Direction::Left && newDir != Direction::Right) ||
//        (currentDirection == Direction::Right && newDir != Direction::Left)) {
//        currentDirection = newDir;
//    }
//}

void Snake::Update() {
    /*auto head = segments.front();
    switch (currentDirection) {
    case Direction::Up:    head.y -= 1; break;
    case Direction::Down:  head.y += 1; break;
    case Direction::Left:  head.x -= 1; break;
    case Direction::Right: head.x += 1; break;
    }

    segments.push_front(head);
    segments.pop_back();*/

    Segment newHead = { body.front().x + dirX, body.front().y + dirY };
    body.insert(body.begin(), newHead);
    body.pop_back();
}

void Snake::Grow() {
    Segment tail = body.back();
    body.push_back(tail);
}

bool Snake::CheckCollision(const int x, const int y) const {
    return body.front().x == x && body.front().y == y;
}

const std::deque<DirectX::XMINT2>& Snake::GetSegments() const {
    return segments;
}

int Snake::GetHeadX() const {
    return body.front().x;
}

int Snake::GetHeadY() const {
    return body.front().y;
}

bool Snake::CheckSelfCollision() const {
    /*auto head = body.front();
    for (size_t i = 1; i < body.size(); ++i) {
        if (body[i] == head) {
            return true;
        }
    }
    return false;*/
    const Segment& head = body.front();
    for (size_t i = 1; i < body.size(); ++i) {
        if (body[i].x == head.x && body[i].y == head.y)
            return true;
    }
    return false;
}

void Snake::SetDirection(int dx, int dy) {
    if ((dx != 0 && dx != -dirX) || (dy != 0 && dy != -dirY)) {
        dirX = dx;
        dirY = dy;
    }
}