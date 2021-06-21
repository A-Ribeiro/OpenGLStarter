
#ifndef ReferenceCounter__H
#define ReferenceCounter__H

#include <aribeiro/aribeiro.h>

#include <map>

//#include "../SharedPointer.h"

namespace GLEngine {

    struct ReferenceCounterElement{
        int count;
        bool isArray;
    };

    template <typename T>
    class ReferenceCounter {
        std::map<T,ReferenceCounterElement> map;
    public:
        T add(T c,bool isArray = false){

            ARIBEIRO_ABORT(c == NULL, "trying to register NULL on reference counter.\n");

            //if (map.find(c) == map.end())
                //map[c] = 0;
            map[c].count++;
            map[c].isArray = isArray;
            return c;
        }

        bool willDeleteOnRemove(T c) {
            if (map.find(c) != map.end()) {
                //map[c]--;
                if (map[c].count <= 1) {
                    return true;
                }
            }
            else {
                //erase data if the reference is not in the map
                return true;
            }
            return false;
        }

        void remove(T &c, bool isArray = false){

            typename std::map<T, ReferenceCounterElement>::iterator it = map.find(c);

            if (it != map.end()){
                it->second.count--;
                if (it->second.count <= 0){
                    //map.erase(c);
                    //SharedPointerDatabase::getInstance()->notifyDeletion(c);
                    if (it->second.isArray)
                        aRibeiro::setNullAndDeleteArray(c);
                    else
                        aRibeiro::setNullAndDelete(c);
                    map.erase(it);
                }
            } else {
                //erase data if the reference is not in the map
                //SharedPointerDatabase::getInstance()->notifyDeletion(c);
                if (isArray)
                    aRibeiro::setNullAndDeleteArray(c);
                else
                    aRibeiro::setNullAndDelete(c);
            }
        }
        
        void removeNoDelete(T c, bool isArray = false){
            
            ARIBEIRO_ABORT(!willDeleteOnRemove(c),"Not the last element to remove error.\n");
            
            typename std::map<T, ReferenceCounterElement>::iterator it = map.find(c);
            
            if (it != map.end()){
                it->second.count--;
                if (it->second.count <= 0){
                    //if (it->second.isArray)
                    //    aRibeiro::setNullAndDeleteArray(c);
                    //else
                    //    aRibeiro::setNullAndDelete(c);
                    map.erase(it);
                }
            } else {
                //if (isArray)
                //    aRibeiro::setNullAndDeleteArray(c);
                //else
                //    aRibeiro::setNullAndDelete(c);
            }
        }

        ~ReferenceCounter() {
            /*
            typename std::map<T,ReferenceCounterElement>::iterator it = map.begin();
            while (it != map.end()){
                T c = it->first;
                if (it->second.isArray)
                    aRibeiro::setNullAndDeleteArray(c);
                else
                    aRibeiro::setNullAndDelete(c);
                it++;
            }
            map.clear();
            */
            if (map.size() > 0) {
                fprintf(stderr, "Still there are elements inside the ReferenceCounter that doesnt have been released...\n");
            }


        }
    };

}

#endif
