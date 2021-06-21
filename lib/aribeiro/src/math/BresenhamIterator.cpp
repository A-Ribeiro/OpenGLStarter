
#include "BresenhamIterator.h"

namespace aRibeiro{

BresenhamIterator::BresenhamIterator(const int x,const int y,const int xd,const int yd){
    deltaCol = xd-x;
    deltaRow = yd-y;
    nextCol = x;
    nextRow = y;
    endCol = xd;
    endRow = yd;
    if (deltaRow <  0){
        stepRow=-1;
        deltaRow = -deltaRow*2;
    }else
        stepRow=1;

    if (deltaCol <  0){
        stepCol=-1;
        deltaCol = -deltaCol*2;
    }else
        stepCol=1;

    if (deltaCol >deltaRow){
        fraction = deltaRow*2-deltaCol;
        iterateCol = true;
    }else{
        fraction = deltaCol*2-deltaRow;
        iterateCol = false;
    }
}

bool BresenhamIterator::next(){
    if(iterateCol){
        if (nextCol != endCol){
            if (fraction >=0){
                nextRow =nextRow +stepRow;
                fraction =fraction -deltaCol;
            }
            nextCol=nextCol+stepCol;
            fraction=fraction +deltaRow;
            return true;
        }
    }else{
        if (nextRow !=endRow){
            if (fraction >=0){
                nextCol=nextCol+stepCol;
                fraction=fraction -deltaRow;
            }
            nextRow =nextRow +stepRow;
            fraction=fraction +deltaCol;
            return true;
        }
    }
    return false;
}

bool BresenhamIterator::next(int *x,int *y){
    bool retorno = next();
    *x = nextCol;
    *y = nextRow;
    return retorno;
}

void BresenhamIterator::getXY(int *x,int *y){
    *x = nextCol;
    *y = nextRow;
}

}
