// InputManager.cpp
#include "InputManager.h"

InputManager::InputManager() {
    Reset();
}

void InputManager::OnKeyDown(WPARAM key) {
    if (key < 256)
        keys[key] = true;
}

void InputManager::OnKeyUp(WPARAM key) {
    if (key < 256)
        keys[key] = false;
}

bool InputManager::IsKeyPressed(WPARAM key) const {
    if (key < 256)
        return keys[key];
    return false;
}

void InputManager::Reset() {
    for (int i = 0; i < 256; ++i)
        keys[i] = false;
}
