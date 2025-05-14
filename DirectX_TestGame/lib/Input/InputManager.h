#pragma once
#include <Windows.h>
#include <unordered_map>

class Input {
public:
    void Update();  // 入力状態の更新（毎フレーム呼び出し）

    bool IsKeyPressed(int keyCode) const;     // 押された瞬間
    bool IsKeyHeld(int keyCode) const;        // 押され続けている
    bool IsKeyReleased(int keyCode) const;    // 離された瞬間

private:
    std::unordered_map<int, bool> currentKeys;
    std::unordered_map<int, bool> previousKeys;
};
