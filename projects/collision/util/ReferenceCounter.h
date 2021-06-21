
#ifndef ReferenceCounter__H
#define ReferenceCounter__H

//#include "../SharedPointer.h"

#include <map>

template <typename T>
class ReferenceCounter {
    std::map<T,int> map;
public:
    T add(T c){
        //if (map.find(c) == map.end())
            //map[c] = 0;
        map[c]++;
        return c;
    }

    bool willDeleteOnRemove(T c) {
        if (map.find(c) != map.end()) {
            //map[c]--;
            if (map[c] <= 1) {
                return true;
            }
        }
        else {
            //erase data if the reference is not in the map
            return true;
        }
        return false;
    }

    void remove(T c){

        typename std::map<T, int>::iterator it = map.find(c);

        if (it != map.end()){
            it->second--;
            if (it->second <= 0){
                //map.erase(c);
                //SharedPointerDatabase::getInstance()->notifyDeletion(c);
                setNullAndDelete(c);
                map.erase(it);
            }
        } else {
            //erase data if the reference is not in the map
            //SharedPointerDatabase::getInstance()->notifyDeletion(c);
            setNullAndDelete(c);
        }
    }

    ~ReferenceCounter() {
        typename std::map<T,int>::iterator it = map.begin();
        while (it != map.end()){
            T c = it->first;
            setNullAndDelete(c);
            it++;
        }
        map.clear();
    }
};



template <typename T>
class ReferenceCounterArray {
    std::map<T, int> map;
public:
    T add(T c) {
        //if (map.find(c) == map.end())
            //map[c] = 0;
        map[c]++;
        return c;
    }

    bool willDeleteOnRemove(T c) {
        if (map.find(c) != map.end()) {
            //map[c]--;
            if (map[c] <= 1) {
                return true;
            }
        }
        else {
            //erase data if the reference is not in the map
            return true;
        }
        return false;
    }

    void remove(T c) {

        typename std::map<T, int>::iterator it = map.find(c);

        if (it != map.end()) {
            it->second--;
            if (it->second <= 0) {
                //map.erase(c);
                //SharedPointerDatabase::getInstance()->notifyDeletion(c);
                setNullAndDeleteArray(c);
                map.erase(it);
            }
        }
        else {
            //erase data if the reference is not in the map
            //SharedPointerDatabase::getInstance()->notifyDeletion(c);
            setNullAndDeleteArray(c);
        }
    }

    ~ReferenceCounterArray() {
        typename std::map<T, int>::iterator it = map.begin();
        while (it != map.end()) {
            T c = it->first;
            setNullAndDeleteArray(c);
            it++;
        }
        map.clear();
    }
};


#endif
