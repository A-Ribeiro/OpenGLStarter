
#ifndef object_h
#define object_h

//#include "../../common/common.h"
//#include "../colision/AABB.h"//AABB
//#include "../../../math/all.h" //vec2
#include "string.h"
#include "memory.h"

#include "../../../SpaceInvader.h"


enum ObjectType{
  OT_Nothing = 0,
  OT_Enemy,
  OT_EnemyShipShoot,
  OT_SpaceShip,
  OT_SpaceShipShoot,
  OT_Xplosion
};
const char* objectTypeToStr(ObjectType);

enum ObjectState{
  OS_Nothing = -1,
  OS_GotoOutOfScreen_0 = 0,
  OS_GotoOutOfScreen_1,
  OS_GotoOutOfScreen_2,
  OS_GotoOutOfScreen_3,
  OS_GgotoOutOfScreen_4,
  OS_GotoOutOfScreen_MAX,
  OS_GotoFormationOrigin,
  OS_GotoSquadOrigin,
};
const char* objectStateToStr(ObjectState o);

class Object {
public:
    Object(){}

    Object(
        const ObjectType type,
        const ObjectState action,
        const vec2 origin,
        const vec2 toMove,
        const vec2 position,
        const AABB aabb,
        const bool active,
        const vec2 dimension,
        const vec2 velocity,
        const int counter) {
        this->type = type;
        this->action = action;
        this->formation.origin = origin;
        this->formation.toMove = toMove;
        this->position = position;
        this->aabb = aabb;
        this->active = active;
        this->dimension = dimension;
            this->velocity = velocity;
        this->counter = counter;
    }

    struct _form {
        vec2 origin;//pos de entrada
        vec2 toMove;//pos na formacao de ataque
    }formation;
    vec2 position;

    AABB aabb;//Axis-Aligned Bound Box
    
    vec2 dimension;
    vec2 velocity;

    ObjectType type;
    ObjectState action;
    bool active;
    int counter;//usado somente para saber quanto tempo a explos�o tem que ficar aparecendo

    SSE2_CLASS_NEW_OPERATOR
};


#include <set>
using namespace std;

typedef set<Object*> ObjectList_list;
typedef ObjectList_list::iterator ObjectListIterator_list;

class ObjectList{
  private:
    ObjectList_list objects;
    ObjectList_list objectsToRemove;
  public:
    int size();
    void clear();
    void insert(const Object &obj);
    void remove(Object *obj);
    void clearRemoveList();
    void doRemove();
    int getNumberOfRemoves();
    bool hasColision(Object* obj, vec2 *colisionPoint);
    //retorna false quando for para parar o traverse
    void traverseObjectList(bool (*callProcedure)(void* param, Object *object), void* param1);
    Object* getFirst();
};

void printObjectState(const Object &obj);

#endif
