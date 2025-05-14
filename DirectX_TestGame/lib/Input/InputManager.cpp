#include "InputManager.h"

void Input::Update() {
    previousKeys = currentKeys;

    currentKeys.clear();
    for (int key = 0x01; key <= 0xFE; ++key) {
        SHORT state = GetAsyncKeyState(key);
        currentKeys[key] = (state & 0x8000) != 0;
    }
}

bool Input::IsKeyPressed(int keyCode) const {
    return currentKeys.at(keyCode) && !previousKeys.at(keyCode);
}

bool Input::IsKeyHeld(int keyCode) const {
    return currentKeys.at(keyCode);
}

bool Input::IsKeyReleased(int keyCode) const {
    return !currentKeys.at(keyCode) && previousKeys.at(keyCode);
}
