#include "FT2Glyph.h"

namespace ft2Wrapper {
    
    //private copy constructores, to avoid copy...
    FT2Glyph::FT2Glyph(const FT2Glyph& v){}
    void FT2Glyph::operator=(const FT2Glyph& v){}

    void FT2Glyph::copyNormalBuffer(uint8_t *src, bool invert) {
        
        ARIBEIRO_ABORT(
            (normalRect.w == 0 || normalRect.h == 0), 
            "error: normalRect not set\n");
        //if (normalRect.w==0||normalRect.h==0){
        //    fprintf(stderr,"error: normalRect not set\n");
        //    exit(-1);
        //}
        
        if (invert)
            normalRect.y = normalRect.h - normalRect.y - 1;
        
        if (bitmapGrayNormal != NULL)
            delete[] bitmapGrayNormal;
        
        bitmapGrayNormal = new uint8_t[normalRect.w*normalRect.h];
        
        if (invert){
            for (int i=0;i<normalRect.h;i++)
                memcpy(bitmapGrayNormal + i * normalRect.w,
                       src + (normalRect.h-1-i) * normalRect.w,
                       normalRect.w * sizeof(uint8_t));
        } else
            memcpy(bitmapGrayNormal, src, normalRect.w*normalRect.h);
        
        //generate RGBA
        if (bitmapRGBANormal != NULL)
            delete[] bitmapRGBANormal;
        bitmapRGBANormal = new uint8_t[normalRect.w*normalRect.h*4];
        for(int y=0;y<normalRect.h;y++){
            for(int x=0;x<normalRect.w;x++){
                bitmapRGBANormal[(x+y*normalRect.w)*4+0] = 255;
                bitmapRGBANormal[(x+y*normalRect.w)*4+1] = 255;
                bitmapRGBANormal[(x+y*normalRect.w)*4+2] = 255;
                bitmapRGBANormal[(x+y*normalRect.w)*4+3] = bitmapGrayNormal[x + y * normalRect.w];
            }
        }
    }
    
    void FT2Glyph::copyStrokeBuffer(uint8_t *src, bool invert) {
        
        ARIBEIRO_ABORT(
            (strokeRect.w == 0 || strokeRect.h == 0),
            "error: strokeRect not set\n");
        //if (strokeRect.w==0||strokeRect.h==0){
        //    fprintf(stderr,"error: strokeRect not set\n");
        //    exit(-1);
        //}
        
        if (invert)
            strokeRect.y = strokeRect.h - strokeRect.y - 1;
        
        if (bitmapGrayStroke != NULL)
            delete[] bitmapGrayStroke;
        
        bitmapGrayStroke = new uint8_t[strokeRect.w * strokeRect.h];
        
        if (invert){
            for (int i=0;i<strokeRect.h;i++)
                memcpy(bitmapGrayStroke + i * strokeRect.w,
                       src + (strokeRect.h-1-i) * strokeRect.w,
                       strokeRect.w * sizeof(uint8_t));
        } else
            memcpy(bitmapGrayStroke, src, strokeRect.w * strokeRect.h);
        
        //generate RGBA
        if (bitmapRGBAStroke != NULL)
            delete[] bitmapRGBAStroke;
        bitmapRGBAStroke = new uint8_t[strokeRect.w * strokeRect.h*4];
        for(int y=0;y<strokeRect.h;y++){
            for(int x=0;x<strokeRect.w;x++){
                bitmapRGBAStroke[(x+y*strokeRect.w)*4+0] = 255;
                bitmapRGBAStroke[(x+y*strokeRect.w)*4+1] = 255;
                bitmapRGBAStroke[(x+y*strokeRect.w)*4+2] = 255;
                bitmapRGBAStroke[(x+y*strokeRect.w)*4+3] = bitmapGrayStroke[x + y * strokeRect.w];
            }
        }
    }

    void FT2Glyph::saveNormalGrayPNG(const std::string& filename) {
        aRibeiro::PNGHelper::writePNG(filename.c_str(), normalRect.w, normalRect.h, 1, (char*)bitmapGrayNormal);
    }

    void FT2Glyph::saveStrokeGrayPNG(const std::string& filename) {
        aRibeiro::PNGHelper::writePNG(filename.c_str(), strokeRect.w, strokeRect.h, 1, (char*)bitmapGrayStroke);
    }

    void FT2Glyph::saveNormalRGBAPNG(const std::string& filename) {
        aRibeiro::PNGHelper::writePNG(filename.c_str(), normalRect.w, normalRect.h, 4, (char*)bitmapRGBANormal);
    }

    void FT2Glyph::saveStrokeRGBAPNG(const std::string& filename) {
        aRibeiro::PNGHelper::writePNG(filename.c_str(), strokeRect.w, strokeRect.h, 4, (char*)bitmapRGBAStroke);
    }
    
    FT2Glyph::FT2Glyph(){
        bitmapGrayNormal = NULL;
        bitmapGrayStroke = NULL;
        bitmapRGBANormal = NULL;
        bitmapRGBAStroke = NULL;
    }
    
    FT2Glyph::~FT2Glyph(){
        if (bitmapGrayNormal != NULL)
            delete[] bitmapGrayNormal;
        if (bitmapGrayStroke != NULL)
            delete[] bitmapGrayStroke;
        if (bitmapRGBANormal != NULL)
            delete[] bitmapRGBANormal;
        if (bitmapRGBAStroke != NULL)
            delete[] bitmapRGBAStroke;
        
        bitmapGrayNormal = NULL;
        bitmapGrayStroke = NULL;
        bitmapRGBANormal = NULL;
        bitmapRGBAStroke = NULL;
    }
    
}
