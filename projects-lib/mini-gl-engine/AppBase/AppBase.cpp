#include "AppBase.h"

#include <mini-gl-engine/Engine.h>
#include <aribeiro/aribeiro.h>
using namespace aRibeiro;

namespace GLEngine {

    AppBase::AppBase() {
        canExitApplication = false;
        WindowSize.OnChange.add(this, &AppBase::OnWindowSizeChanged);

        sf::RenderWindow* window = Engine::Instance()->window;
        WindowSize = sf::Vector2i(window->getSize().x,window->getSize().y);
    }

    AppBase::~AppBase() {
    }

    void AppBase::OnWindowSizeChanged(aRibeiro::Property<sf::Vector2i> *prop) {

        //printf("WindowSize: %i %i\n",prop->value.x, prop->value.y);
        
        screenCenterWindowSpace.x = prop->value.x / 2;
        screenCenterWindowSpace.y = prop->value.y / 2;

        //printf("screenCenterWindowSpace: %i %i\n",screenCenterWindowSpace.x, screenCenterWindowSpace.y);
        
        MousePosCenter.x = screenCenterWindowSpace.x;
        MousePosCenter.y = prop->value.y - 1 - screenCenterWindowSpace.y;
    }

    void AppBase::moveMouseToScreenCenter(){
        sf::RenderWindow* window = Engine::Instance()->window;
        sf::Mouse::setPosition(screenCenterWindowSpace, *window);
    }

    aRibeiro::vec2 AppBase::WindowCoordToNormalizedCoord(const aRibeiro::vec2 &_2dCoord) {
        aRibeiro::vec2 result = _2dCoord - MousePosCenter;

        if (screenCenterWindowSpace.x > screenCenterWindowSpace.y)
            result /= (float)screenCenterWindowSpace.y;
        else
            result /= (float)screenCenterWindowSpace.x;
        
        return result;
    }

    void AppBase::exitApp() {
        canExitApplication = true;
    }

    void AppBase::draw() {
    }

}
