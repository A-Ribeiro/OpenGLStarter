#include "AtlasRect.h"

namespace DataModel {
    
    AtlasRect::AtlasRect(){
        x=y=w=h=0;
    }
    
    AtlasRect::AtlasRect(int _x,int _y,int _w,int _h){
        x=_x;
        y=_y;
        w=_w;
        h=_h;
    }
    
    AtlasRect::AtlasRect(int _w,int _h){
        x=0;
        y=0;
        w=_w;
        h=_h;
    }
    
    void AtlasRect::setXY(int _x,int _y) {
        x=_x;
        y=_y;
    }
    
    int AtlasRect::maxXInclusive()const{
        return x+w-1;
    }
    
    int AtlasRect::maxYInclusive()const {
        return y+h-1;
    }
    
    bool AtlasRect::overlaps(const AtlasRect &other, int xspacing, int yspacing)const{
        if (w==0||h==0||other.w==0||other.h==0)
            return false;
        
        int xmax = x + w - 1;
        int ymax = y + h - 1;
        
        int otherxmax = other.x+other.w-1;
        int otherymax = other.y+other.h-1;
        
        if (otherxmax + xspacing < x || other.x > xmax + xspacing)
            return false;
        if (otherymax + yspacing < y || other.y > ymax + yspacing)
            return false;
        
        return true;
    }
    
    bool AtlasRect::inside(const AtlasRect &screen, int xspacing, int yspacing)const {
        if (w==0||h==0||screen.w==0||screen.h==0)
            return false;
        
        if (x >= xspacing && (x + w + xspacing) <= screen.w &&
            y >= yspacing && (y + h + yspacing) <= screen.h )
            return true;

        /*
        int xmax = x + w - 1;
        int ymax = y + h - 1;
        
        int screen_xmax = screen.x+screen.w-1;
        int screen_ymax = screen.y+screen.h-1;
        
        if (x >= screen.x + xspacing && xmax + xspacing <= screen_xmax &&
            y >= screen.y + yspacing && ymax + yspacing <= screen_ymax)
            return true;
        */

        return false;
    }
    
    void AtlasRect::write(aRibeiro::BinaryWriter *writer)const{
        writer->writeUInt16(x);
        writer->writeUInt16(y);
        writer->writeUInt16(w);
        writer->writeUInt16(h);
    }
    void AtlasRect::read(aRibeiro::BinaryReader *reader){
        x = reader->readUInt16();
        y = reader->readUInt16();
        w = reader->readUInt16();
        h = reader->readUInt16();
    }

}
