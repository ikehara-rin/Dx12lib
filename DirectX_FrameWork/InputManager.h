// InputManager.h
#pragma once
#include <Windows.h>

class InputManager {
public:
    InputManager();
    void OnKeyDown(WPARAM key);
    void OnKeyUp(WPARAM key);
    bool IsKeyPressed(WPARAM key) const;
    void Reset(); // 1フレームごとのリセット用（必要に応じて）

private:
    bool keys[256]; // キー状態配列
};
