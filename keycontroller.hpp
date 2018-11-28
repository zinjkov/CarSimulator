#ifndef KEYCONTROLLER_HPP
#define KEYCONTROLLER_HPP
#include <QMap>
#include <QKeyEvent>
#include <SDL/SDL_joystick.h>
#include <SDL/SDL_gamecontroller.h>
#include <SDL/SDL_keycode.h>
#include <SDL/SDL_mouse.h>
#include <SDL/SDL_events.h>
class KeyController
{
public:
    static KeyController& instance();
    void KeyPressed(QKeyEvent *ke);
    void KeyReleased(QKeyEvent *ke);
private:
    KeyController();
    void init();
    Uint16 convertQtKeyModifierToSDL(Qt::KeyboardModifiers qtKeyModifiers);
    SDL_Keycode convertQtKeyToSDL(Qt::Key qtKey);
    QMap<Qt::Key, SDL_Keycode> m_keyMap;
};

#endif // KEYCONTROLLER_HPP
