#pragma once
#include <Windows.h>

// ���͂��Ǘ�����N���X
class Input
{
public:
    // �L�[�̏��
    bool m_keys[256] = {};

    // ������
    void Initialize();

    // �L�[���������Ƃ��ɌĂ�
    void KeyDown(WPARAM key);

    // �L�[�𗣂����Ƃ��ɌĂ�
    void KeyUp(WPARAM key);

    // �L�[��������Ă��邩�𔻒肷��
    bool IsKeyDown(UINT keyCode) const;
};
