#include "Input.h"

void Input::Initialize()
{
    // ‘SƒL[‚Ìó‘Ô‚ğ‰Šú‰»
    ZeroMemory(m_keys, sizeof(m_keys));
}

void Input::KeyDown(WPARAM key)
{
    if (key < 256)
    {
        m_keys[key] = true;
    }
}

void Input::KeyUp(WPARAM key)
{
    if (key < 256)
    {
        m_keys[key] = false;
    }
}

bool Input::IsKeyDown(UINT keyCode) const
{
    if (keyCode < 256)
    {
        return m_keys[keyCode];
    }
    return false;
}
