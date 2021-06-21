#ifndef AppBase__H_
#define AppBase__H_

#include <aribeiro/aribeiro.h>
#include <opengl-wrapper/opengl-wrapper.h>

#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

namespace GLEngine {

    BEGIN_DECLARE_DELEGATE(MouseEvent, sf::Mouse::Button button, const aRibeiro::vec2 &pos) CALL_PATTERN(button, pos) END_DECLARE_DELEGATE;
    BEGIN_DECLARE_DELEGATE(KeyboardEvent, sf::Keyboard::Key code) CALL_PATTERN(code) END_DECLARE_DELEGATE;
    BEGIN_DECLARE_DELEGATE(CallEvent) CALL_PATTERN() END_DECLARE_DELEGATE;
    BEGIN_DECLARE_DELEGATE(UpdateEvent, aRibeiro::PlatformTime *time) CALL_PATTERN(time) END_DECLARE_DELEGATE;

    class AppBase {

        void OnWindowSizeChanged(aRibeiro::Property<sf::Vector2i> *prop);

    public:

        bool canExitApplication;
        aRibeiro::vec2 MousePosCenter;// Mouse FPS controller helper
        sf::Vector2i screenCenterWindowSpace;// used for FPS like mouse move

        //
        // Properties
        //
        aRibeiro::Property<sf::Vector2i> WindowSize;
        aRibeiro::Property<aRibeiro::vec2> MousePos;
        aRibeiro::Property<aRibeiro::vec2> MousePosRelatedToCenter;
        aRibeiro::Property<aRibeiro::vec2> MousePosRelatedToCenterNormalized;

        //
        // Events
        //
        MouseEvent OnMouseDown;
        MouseEvent OnMouseUp;
        CallEvent OnMouseWheelUp;
        CallEvent OnMouseWheelDown;
        KeyboardEvent OnKeyDown;
        KeyboardEvent OnKeyUp;
        CallEvent OnLostFocus;
        CallEvent OnGainFocus;

        UpdateEvent OnPreUpdate;
        UpdateEvent OnUpdate;
        UpdateEvent OnLateUpdate;

        UpdateEvent OnAfterGraphPrecompute;
        
        AppBase();
        virtual ~AppBase();
        virtual void draw();

        void moveMouseToScreenCenter();

        aRibeiro::vec2 WindowCoordToNormalizedCoord(const aRibeiro::vec2 &windowCoord);

        void exitApp();

        SSE2_CLASS_NEW_OPERATOR
    };

}

#endif
