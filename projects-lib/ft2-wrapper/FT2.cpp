#include "FT2.h"

#define __STDC_CONSTANT_MACROS
#include <iostream>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_STROKER_H

#include <iostream>

using namespace std;
#define FT_CHECK_ERROR(_fnc, message) \
    ARIBEIRO_ABORT( (_fnc) != 0, message );

namespace ft2Wrapper {
    
    FT2::FT2(){
        library = NULL;
        face = NULL;
        stroker = NULL;
    }
    FT2::~FT2(){
        clear();
    }
    
    void FT2::clear(){
        if (stroker != NULL)
            FT_Stroker_Done( stroker );
        if (face != NULL)
            FT_Done_Face(face);
        if (library != NULL)
            FT_Done_FreeType(library);
        stroker = NULL;
        face = NULL;
        library = NULL;
    }
    
    void FT2::readFromFile_PX(const std::string &filename,
                              FT_Int face_index,
                              float outlineThickness,
                              FT_UInt max_width, FT_UInt max_height) {
        clear();
        
        FT_CHECK_ERROR( FT_Init_FreeType( &library ) , "Erro do initialize FT2 lib" );
        FT_CHECK_ERROR( FT_New_Face( library,filename.c_str(),face_index,&face ) , "Erro do load font file FT2 lib" );
        FT_CHECK_ERROR( FT_Stroker_New( library, &stroker ),"error to create font stroker");
        
        cout << "FT2 (PX mode)"<< endl;
        cout << "File Opened: " << filename << endl;
        cout << "Faces: " << face->num_faces << endl;
        cout << "Selected face: " << face->face_index << endl;
        cout << "Family: " << face->family_name << endl;
        cout << "Style: " << face->style_name << endl;
        
        FT_CHECK_ERROR( FT_Set_Pixel_Sizes( face, max_width, max_height ), "error to set the font size");
        FT_Stroker_Set( stroker,
                       (FT_Fixed)( outlineThickness * 64.0f + 0.5f),
                       FT_STROKER_LINECAP_ROUND, FT_STROKER_LINEJOIN_ROUND, 0 );
    }
    
    
    ///    windows default DPI = 96
    ///    mac default DPI = 72
    ///    iphone4 - 960-by-640-pixel resolution at 326 ppi
    ///    iphone - 480 x 320 163 pixels per inch
    ///    ipod touch - 480 x 320 160 pixels per inch
    void FT2::readFromFile_DPI(const std::string &filename,
                               FT_Int face_index,
                               float outlineThickness,
                               FT_UInt DPI, float fontPT) {
        clear();
        
        FT_CHECK_ERROR( FT_Init_FreeType( &library ) , "Erro do initialize FT2 lib" );
        FT_CHECK_ERROR( FT_New_Face( library,filename.c_str(),face_index,&face ) , "Erro do load font file FT2 lib" );
        FT_CHECK_ERROR( FT_Stroker_New( library, &stroker ),"error to create font stroker");
        
        cout << "FT2 (DPI mode)"<< endl;
        cout << "File Opened: " << filename << endl;
        cout << "Faces: " << face->num_faces << endl;
        cout << "Selected face: " << face->face_index << endl;
        cout << "Family: " << face->family_name << endl;
        cout << "Style: " << face->style_name << endl;
        
        FT_UInt ptInt = (FT_UInt)( fontPT * 64.0f + 0.5f );
        FT_CHECK_ERROR( FT_Set_Char_Size(face, 0, ptInt, DPI, DPI ), "error to set the font size");
        
        FT_Stroker_Set( stroker,
                       (FT_Fixed)( outlineThickness * 64.0f + 0.5f),
                       FT_STROKER_LINECAP_ROUND, FT_STROKER_LINEJOIN_ROUND, 0 );
        
    }
    
    FT2Glyph* FT2::generateGlyph(FT_ULong charcode_utf32){
        FT_Int glyph_index = FT_Get_Char_Index( face, charcode_utf32 );
        if (glyph_index == 0)
            return NULL;
        
        FT2Glyph* result = new FT2Glyph();
        
        FT_CHECK_ERROR( FT_Load_Glyph( face, glyph_index, FT_LOAD_DEFAULT ), "Error to load the character");
        FT_CHECK_ERROR( FT_Render_Glyph( face->glyph, FT_RENDER_MODE_NORMAL ), "Error to render the character");
        
        result->charcode = charcode_utf32;
        result->advancex = ((float)face->glyph->advance.x) / 64.0f;
        result->advancey = ((float)face->glyph->advance.y) / 64.0f;
        
        result->normalRect = FT2Rect(face->glyph->bitmap_left,
                                     face->glyph->bitmap_top,
                                     face->glyph->bitmap.width,
                                     face->glyph->bitmap.rows);
        
        if (face->glyph->bitmap.width > 0 && face->glyph->bitmap.rows > 0)
            result->copyNormalBuffer(face->glyph->bitmap.buffer, false);
        
        FT_CHECK_ERROR( FT_Load_Glyph( face, glyph_index, FT_LOAD_NO_BITMAP | FT_LOAD_TARGET_NORMAL ), "Error to load the character");
        
        FT_Glyph glyphDescStroke = NULL;
        FT_CHECK_ERROR(FT_Get_Glyph( face->glyph, &glyphDescStroke ),"error to get stroke glyph");
        
        FT_CHECK_ERROR(FT_Glyph_Stroke( &glyphDescStroke, stroker, true ),"error to stroke");
        FT_CHECK_ERROR(FT_Glyph_To_Bitmap( &glyphDescStroke, FT_RENDER_MODE_NORMAL, 0, true),"error to get bitmap");
        
        FT_BitmapGlyph glyph_bitmap = (FT_BitmapGlyph)glyphDescStroke;
        
        result->strokeRect = FT2Rect(glyph_bitmap->left,
                                     glyph_bitmap->top,
                                     glyph_bitmap->bitmap.width,
                                     glyph_bitmap->bitmap.rows);
        
        if (glyph_bitmap->bitmap.width > 0 && glyph_bitmap->bitmap.rows > 0)
            result->copyStrokeBuffer(glyph_bitmap->bitmap.buffer, false);
        
        FT_Done_Glyph( glyphDescStroke );
        
        return result;
    }
    
    void FT2::releaseGlyph(FT2Glyph**glyph){
        delete *glyph;
        *glyph = NULL;
    }
    
}
