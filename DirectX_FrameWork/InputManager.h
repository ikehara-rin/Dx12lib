// InputManager.h
#pragma once
#include <Windows.h>

class InputManager {
public:
    InputManager();
    void OnKeyDown(WPARAM key);
    void OnKeyUp(WPARAM key);
    bool IsKeyPressed(WPARAM key) const;
    void Reset(); // 1�t���[�����Ƃ̃��Z�b�g�p�i�K�v�ɉ����āj

private:
    bool keys[256]; // �L�[��Ԕz��
};
