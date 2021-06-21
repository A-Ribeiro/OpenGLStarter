#ifndef AppBase__H
#define AppBase__H

#include <aribeiro/aribeiro.h>
using namespace aRibeiro;

#include "Size.h"

#include <SFML/Window.hpp>

BEGIN_DECLARE_DELEGATE(MouseEvent, sf::Mouse::Button button, const vec2 &pos) CALL_PATTERN(button, pos) END_DECLARE_DELEGATE;
BEGIN_DECLARE_DELEGATE(KeyboardEvent, sf::Keyboard::Key code) CALL_PATTERN(code) END_DECLARE_DELEGATE;
BEGIN_DECLARE_DELEGATE(CallEvent) CALL_PATTERN() END_DECLARE_DELEGATE;

class AppBase {

    void OnWindowSizeChanged(Property<iSize> *prop);

public:
    bool canExitApplication;

    //
    // Properties
    //
    Property<iSize> WindowSize;
    Property<vec2> MousePos;

    //
    // Events
    //
    MouseEvent OnMouseDown;
    MouseEvent OnMouseUp;
    CallEvent OnMouseWheelUp;
    CallEvent OnMouseWheelDown;
    KeyboardEvent OnKeyDown;
    KeyboardEvent OnKeyUp;
    
    AppBase(int w, int h);
    virtual ~AppBase();
    virtual void draw();
    
    SSE2_CLASS_NEW_OPERATOR
};

#endif
