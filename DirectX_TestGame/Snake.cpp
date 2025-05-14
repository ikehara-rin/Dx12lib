#include "Snake.h"

Snake::Snake(int fieldWidth, int fieldHeight)
    : fieldWidth(fieldWidth), fieldHeight(fieldHeight), currentDirection(Direction::Right) {
    segments.push_back({ fieldWidth / 2, fieldHeight / 2 });
}

void Snake::ChangeDirection(Direction newDir) {
    // ‹t•ûŒü‹ÖŽ~
    if ((currentDirection == Direction::Up && newDir != Direction::Down) ||
        (currentDirection == Direction::Down && newDir != Direction::Up) ||
        (currentDirection == Direction::Left && newDir != Direction::Right) ||
        (currentDirection == Direction::Right && newDir != Direction::Left)) {
        currentDirection = newDir;
    }
}

void Snake::Update() {
    auto head = segments.front();
    switch (currentDirection) {
    case Direction::Up:    head.y -= 1; break;
    case Direction::Down:  head.y += 1; break;
    case Direction::Left:  head.x -= 1; break;
    case Direction::Right: head.x += 1; break;
    }

    segments.push_front(head);
    segments.pop_back();
}

const std::deque<DirectX::XMINT2>& Snake::GetSegments() const {
    return segments;
}

int Snake::GetHeadX() const {
    return body.front().first;
}

int Snake::GetHeadY() const {
    return body.front().second;
}

bool Snake::CheckSelfCollision() const {
    auto head = body.front();
    for (size_t i = 1; i < body.size(); ++i) {
        if (body[i] == head) {
            return true;
        }
    }
    return false;
}
