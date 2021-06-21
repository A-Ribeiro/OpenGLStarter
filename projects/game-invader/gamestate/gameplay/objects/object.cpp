
#include "object.h"
//------------------------------------------------------------------------------
const char* objectTypeToStr(ObjectType o){
  switch(o){
    case OT_Nothing: return "OT_Nothing";
    case OT_Enemy: return "OT_Enemy";
    case OT_EnemyShipShoot: return "OT_EnemyShipShoot";
    case OT_SpaceShip: return "OT_SpaceShip";
    case OT_SpaceShipShoot: return "OT_SpaceShipShoot";
    default: break;
  }
  return "OT_Unknown";
}
//------------------------------------------------------------------------------
const char* objectStateToStr(ObjectState o){
  switch(o){
    case OS_GotoOutOfScreen_0: return "OS_GotoOutOfScreen_0";
    case OS_GotoOutOfScreen_1: return "OS_GotoOutOfScreen_1";
    case OS_GotoOutOfScreen_2: return "OS_GotoOutOfScreen_2";
    case OS_GotoOutOfScreen_3: return "OS_GotoOutOfScreen_3";
    case OS_GgotoOutOfScreen_4: return "OS_GgotoOutOfScreen_4";
    case OS_GotoOutOfScreen_MAX: return "OS_GotoOutOfScreen_MAX";
    case OS_GotoFormationOrigin: return "OS_GotoFormationOrigin";
    case OS_GotoSquadOrigin: return "OS_GotoSquadOrigin";
    default: break;
  }
  return "OT_Unknown";
}
//------------------------------------------------------------------------------
int ObjectList::size(){
  return objects.size();
}
//------------------------------------------------------------------------------
void ObjectList::clear(){
  ObjectListIterator_list iterator;
  for( iterator = objects.begin(); iterator != objects.end(); iterator++ ){
    delete *iterator;
  }
  objects.clear();
}
//------------------------------------------------------------------------------
void ObjectList::insert(const Object &obj){
  objects.insert(new Object(obj));
}
//------------------------------------------------------------------------------
void ObjectList::remove(Object *obj){
  if (objectsToRemove.count(obj) == 0)
    objectsToRemove.insert(obj);
}
//------------------------------------------------------------------------------
void ObjectList::clearRemoveList(){
  objectsToRemove.clear();
}
//------------------------------------------------------------------------------
void ObjectList::doRemove(){
  ObjectListIterator_list iterator;
  for( iterator = objectsToRemove.begin(); iterator != objectsToRemove.end(); iterator++ ){
    if (objects.count(*iterator) != 0 ){
      objects.erase(*iterator);
      delete (*iterator);
    }
  }
  objectsToRemove.clear();
}
//------------------------------------------------------------------------------
int ObjectList::getNumberOfRemoves(){
  return objectsToRemove.size();
}
//------------------------------------------------------------------------------
bool ObjectList::hasColision(Object* obj, vec2 *colisionPoint){
  ObjectListIterator_list iterator;
  for( iterator = objects.begin(); iterator != objects.end(); iterator++ ){
    if (!(*iterator)->active) continue;
    if (AABB::aabbOverlapsAABB((*iterator)->aabb, obj->aabb)) {
      *colisionPoint = (*iterator)->position;
      remove(*iterator);
      return true;
    }
  }
  return false;
}
//------------------------------------------------------------------------------
void ObjectList::traverseObjectList(bool (*callProcedure)(void* param, Object *object), void* param1){
  ObjectListIterator_list iterator;
  for( iterator = objects.begin(); iterator != objects.end(); iterator++ ){
    if (!callProcedure(param1,*iterator)) return;
  }
}
//------------------------------------------------------------------------------
Object* ObjectList::getFirst(){
  if (objects.size()>0)
    return *objects.begin();
  return NULL;
}
//------------------------------------------------------------------------------
void printObjectState(const Object &obj){
  if (!obj.active) return;
  printf("        OBJECT(");
  printf("type = %s, ",objectTypeToStr(obj.type));
  printf("action = %s\n",objectStateToStr(obj.action));
  printf("   formation.origin = (%g, %g), ",obj.formation.origin.x,obj.formation.origin.y);
  printf("formation.toMove = (%g, %g)\n",obj.formation.toMove.x,obj.formation.toMove.y);
  printf("   position = (%g, %g)\n",obj.position.x,obj.position.y);
  printf("   aabb = IE(%g, %g) SD(%g, %g)\n ",obj.aabb.min_box.x,obj.aabb.min_box.y,
                                                    obj.aabb.max_box.x,obj.aabb.max_box.y);
  printf("  active = %c, ",(obj.active)?'T':'F');
  printf("dimension = (%g, %g), ",obj.dimension.x,obj.dimension.y);
  printf("velocity = (%g, %g)\n",obj.velocity.x,obj.velocity.y);
}
//------------------------------------------------------------------------------
