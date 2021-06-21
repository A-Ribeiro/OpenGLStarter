#ifndef SIZE__H
#define SIZE__H

#include "string.h"//memcmp

class iSize {
public:
    int x, y;

    iSize(const iSize& v) {
        (*this) = v;
    }

    void operator=(const iSize& v) {
        x = v.x;
        y = v.y;
    }

    bool operator==(const iSize& v) {
        return memcmp(this, &v, sizeof(iSize)) == 0;
    }

    bool operator!=(const iSize& v) {
        return memcmp(this, &v, sizeof(iSize)) != 0;
    }

    iSize() {
        x = y = 0;
    }

    iSize(int x, int y) {
        this->x = x;
        this->y = y;
    }
};


#endif
