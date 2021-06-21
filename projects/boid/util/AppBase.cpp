#include "AppBase.h"

#include <aribeiro/aribeiro.h>
using namespace aRibeiro;

#include "Size.h"

#include "GLRenderState.h"

AppBase::AppBase(int w,int h) {
    GLRenderState *renderState = GLRenderState::getInstance();
    renderState->initialize(w,h);


    canExitApplication = false;
    WindowSize.OnChange.add(this, &AppBase::OnWindowSizeChanged);

    WindowSize = iSize(w, h);
}

AppBase::~AppBase() {
}

void AppBase::OnWindowSizeChanged(Property<iSize> *prop) {
    GLRenderState *renderState = GLRenderState::getInstance();
    renderState->Viewport = iRect(prop->value.x, prop->value.y);
}

void AppBase::draw() {
    //GLRenderState *renderState = GLRenderState::getInstance();
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

