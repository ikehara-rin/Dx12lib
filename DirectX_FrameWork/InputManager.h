#pragma once
#include <Windows.h>
#include <unordered_map>

class Input {
public:
    void Update();  // ���͏�Ԃ̍X�V�i���t���[���Ăяo���j

    bool IsKeyPressed(int keyCode) const;     // �����ꂽ�u��
    bool IsKeyHeld(int keyCode) const;        // �����ꑱ���Ă���
    bool IsKeyReleased(int keyCode) const;    // �����ꂽ�u��

private:
    std::unordered_map<int, bool> currentKeys;
    std::unordered_map<int, bool> previousKeys;
};
