#include "InterfaceGuiGroup.h"

/*
#define EXCEPT(message)                     \
{                                           \
  fprintf(stderr, "Error in file: [%s line %d] ->> %s\n",  \
          __FILE__, __LINE__, message);     \
  exit(-1); \
}
*/

#define EXCEPT(message)\
    ARIBEIRO_ABORT(true,"%s\n",message);

#define DEBUG_PRINT(msg)


//-----------------------------------------------------------------------------------------------
InterfaceGuiGroup::InterfaceGuiGroup(ActionProcessor *actionPerformer) {
    this->actionPerformer = actionPerformer;
}
void InterfaceGuiGroup::setMousePos(const int ID, const vec2 &pos) {}
void InterfaceGuiGroup::resizeWindow(const int ID, const int w, const int h) {}
void InterfaceGuiGroup::render(const int ID, const unsigned int time_ms) {}
//bool-> pra fazer se o evento foi consumido
bool InterfaceGuiGroup::keyDown(const int ID, const int k) { return false; }
bool InterfaceGuiGroup::keyUp(const int ID, const int k) { return false; }
bool InterfaceGuiGroup::mouseDown(const int ID, const int btn, const vec2 &pos) { return false; }
bool InterfaceGuiGroup::mouseUp(const int ID, const int btn, const vec2 &pos) { return false; }
InterfaceGuiGroup::~InterfaceGuiGroup() {}
void InterfaceGuiGroup::setFocus(const int ID) {
    Event e;
    e.type = SET_FOCUS;
    if (!performAction(ID, e)) {
        EXCEPT("[InterfaceGuiGroup]: SetFocus error");
    }
}
bool InterfaceGuiGroup::performAction(const int ID, Event &e)const {
    return actionPerformer->performAction(ID, e);
}
//-----------------------------------------------------------------------------------------------

  //retorna true se puder dar continuidade ao evento
bool ActionProcessor::performAction(const int ID, Event &e) {
#ifdef USING_DEBUG
    char tmp[1024];
    sprintf(tmp, "[ActionProcessor] Event received from %d", ID);
    DEBUG_PRINT(tmp);
#endif
    return true;
}

ActionProcessor::~ActionProcessor() {}

