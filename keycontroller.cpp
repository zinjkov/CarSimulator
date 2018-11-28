#include "keycontroller.hpp"
#include <QDebug>

KeyController &KeyController::instance()
{
    static KeyController *kc = new KeyController();
    return *kc;
}

void KeyController::KeyPressed(QKeyEvent *ke)
{
    // Transmit key press event to SDL
    SDL_Event sdlEvent;
    sdlEvent.type = SDL_KEYDOWN;
    sdlEvent.key.keysym.sym = convertQtKeyToSDL( Qt::Key(ke->key()) );
    sdlEvent.key.keysym.mod = convertQtKeyModifierToSDL(ke->modifiers());
    SDL_PushEvent(&sdlEvent);
}

void KeyController::KeyReleased(QKeyEvent *ke)
{
    // Transmit key release event to SDL
    SDL_Event sdlEvent;
    sdlEvent.type = SDL_KEYUP;
    sdlEvent.key.keysym.sym = convertQtKeyToSDL( Qt::Key(ke->key()) );
    sdlEvent.key.keysym.mod = convertQtKeyModifierToSDL(ke->modifiers());
    SDL_PushEvent(&sdlEvent);
}

KeyController::KeyController()
{
    init();
}

void KeyController::init()
{
    m_keyMap[Qt::Key_unknown]     = SDLK_UNKNOWN;
    m_keyMap[Qt::Key_Escape]      = SDLK_ESCAPE;
    m_keyMap[Qt::Key_Space]      = SDLK_SPACE;
    m_keyMap[Qt::Key_Tab]         = SDLK_TAB;
    m_keyMap[Qt::Key_Backspace]   = SDLK_BACKSPACE;
    m_keyMap[Qt::Key_Return]      = SDLK_RETURN;
    m_keyMap[Qt::Key_Enter]       = SDLK_KP_ENTER;
    m_keyMap[Qt::Key_Insert]      = SDLK_INSERT;
    m_keyMap[Qt::Key_Delete]      = SDLK_DELETE;
    m_keyMap[Qt::Key_Pause]       = SDLK_PAUSE;
    m_keyMap[Qt::Key_Print]       = SDLK_PRINTSCREEN;
    m_keyMap[Qt::Key_SysReq]      = SDLK_SYSREQ;
    m_keyMap[Qt::Key_Home]        = SDLK_HOME;
    m_keyMap[Qt::Key_End]         = SDLK_END;
    m_keyMap[Qt::Key_Left]        = SDLK_LEFT;
    m_keyMap[Qt::Key_Right]       = SDLK_RIGHT;
    m_keyMap[Qt::Key_Up]          = SDLK_UP;
    m_keyMap[Qt::Key_Down]        = SDLK_DOWN;
    m_keyMap[Qt::Key_PageUp]      = SDLK_PAGEUP;
    m_keyMap[Qt::Key_PageDown]    = SDLK_PAGEDOWN;
    m_keyMap[Qt::Key_Shift]       = SDLK_LSHIFT;
    m_keyMap[Qt::Key_Control]     = SDLK_LCTRL;
    m_keyMap[Qt::Key_Alt]         = SDLK_LALT;
    m_keyMap[Qt::Key_CapsLock]    = SDLK_CAPSLOCK;
    m_keyMap[Qt::Key_NumLock]     = SDLK_NUMLOCKCLEAR;
    m_keyMap[Qt::Key_ScrollLock]  = SDLK_SCROLLLOCK;
    m_keyMap[Qt::Key_F1]          = SDLK_F1;
    m_keyMap[Qt::Key_F2]          = SDLK_F2;
    m_keyMap[Qt::Key_F3]          = SDLK_F3;
    m_keyMap[Qt::Key_F4]          = SDLK_F4;
    m_keyMap[Qt::Key_F5]          = SDLK_F5;
    m_keyMap[Qt::Key_F6]          = SDLK_F6;
    m_keyMap[Qt::Key_F7]          = SDLK_F7;
    m_keyMap[Qt::Key_F8]          = SDLK_F8;
    m_keyMap[Qt::Key_F9]          = SDLK_F9;
    m_keyMap[Qt::Key_F10]         = SDLK_F10;
    m_keyMap[Qt::Key_F11]         = SDLK_F11;
    m_keyMap[Qt::Key_F12]         = SDLK_F12;
    m_keyMap[Qt::Key_F13]         = SDLK_F13;
    m_keyMap[Qt::Key_F14]         = SDLK_F14;
    m_keyMap[Qt::Key_F15]         = SDLK_F15;
    m_keyMap[Qt::Key_Menu]        = SDLK_MENU;
    m_keyMap[Qt::Key_Help]        = SDLK_HELP;

    // A-Z
    for(int key='A'; key<='Z'; key++)
        m_keyMap[Qt::Key(key)] = key + 32;

    // 0-9
    for(int key='0'; key<='9'; key++)
        m_keyMap[Qt::Key(key)] = key;
}

Uint16 KeyController::convertQtKeyModifierToSDL(Qt::KeyboardModifiers qtKeyModifiers)
{
    Uint16 sdlModifiers = KMOD_NONE;

    if(qtKeyModifiers.testFlag(Qt::ShiftModifier))
        sdlModifiers |= KMOD_LSHIFT | KMOD_RSHIFT;
    if(qtKeyModifiers.testFlag(Qt::ControlModifier))
        sdlModifiers |= KMOD_LCTRL | KMOD_RCTRL;
    if(qtKeyModifiers.testFlag(Qt::AltModifier))
        sdlModifiers |= KMOD_LALT | KMOD_RALT;

    return sdlModifiers;
}

SDL_Keycode KeyController::convertQtKeyToSDL(Qt::Key qtKey)
{
    SDL_Keycode sldKey = m_keyMap.value(Qt::Key(qtKey));

    if(sldKey == 0) {
        qDebug() << "Warning: Key %d not mapped" <<  qtKey;
    }
    return sldKey;
}
