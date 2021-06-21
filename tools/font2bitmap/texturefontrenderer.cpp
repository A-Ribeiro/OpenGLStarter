
#include "texturefontrenderer.h"
#include "ft2_font_generator.h"
//#include <windows.h>
//#include <GL/gl.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#pragma warning(disable:4996)
#pragma warning(disable:4244)
#pragma warning(disable:4309)
#pragma warning(disable:4018)
#endif

#include <aribeiro/PNGHelper.h>
using namespace aRibeiro;

void bufferWrite(char*dstRGBA, char*originLum,
                 int dstx, int dsty,
                 int w,int h, int limdstW, int limorgW ){
    for (int y=0;y<h;y++){
        for (int x=0;x<w;x++){
            //aqui pode escrever branco com alpha, ao invez de tom de cinza + alpha
            ((unsigned char*)dstRGBA)[(x+dstx+(y+dsty)*limdstW)*4+0] = 255;//originLum[(x+y*limorgW)];
            ((unsigned char*)dstRGBA)[(x+dstx+(y+dsty)*limdstW)*4+1] = 255;//originLum[(x+y*limorgW)];
            ((unsigned char*)dstRGBA)[(x+dstx+(y+dsty)*limdstW)*4+2] = 255;//originLum[(x+y*limorgW)];
            dstRGBA[(x+dstx+(y+dsty)*limdstW)*4+3] = originLum[(x+y*limorgW)];
        }
    }
}

void fillBuffer_RGBA(void* targetBuffer, int w, int h, int spaces, PixelFont *pixelFont,
                     std::map<UTF32, TextureFontRenderer::GlyphInfo> * newGlyphs){
    std::map<UTF32, TextureFontRenderer::GlyphInfo>& newGlyphsRef = *newGlyphs;
    memset(targetBuffer,0,w*h*4);
    int x = spaces;
    int y = spaces;

    //increment the line
    int maxy = 0;

    const std::vector<UTF32>& charset = pixelFont->getCharset();
    for (size_t i=0;i<charset.size();i++){
        UTF32 c = charset[i];
        PixelFont::GlyphInfo *info;
        void* data;
        pixelFont->getGlyph(c,&info,&data);



        int newx = x + info->w + spaces;
        if (newx > w){
            newx = spaces;

            y += maxy + spaces;
            maxy = 0;

            TextureFontRenderer::GlyphInfo glyph;

            glyph.texX = newx;
            glyph.texY = y;
            glyph.x = info->x;
            glyph.y = info->y;
            glyph.w = info->w;
            glyph.h = info->h;
            glyph.advX = info->advX;
            glyph.advY = info->advY;

            newGlyphsRef[c] = glyph;

            //write font to texture
            bufferWrite((char*)targetBuffer, (char*)data,
                        newx,y,
                        info->w,info->h, w, info->w );

            //increment newx
            newx += info->w + spaces;

        }else{

            TextureFontRenderer::GlyphInfo glyph;

            glyph.texX = x;
            glyph.texY = y;
            glyph.x = info->x;
            glyph.y = info->y;
            glyph.w = info->w;
            glyph.h = info->h;
            glyph.advX = info->advX;
            glyph.advY = info->advY;

            newGlyphsRef[c] = glyph;

            //write font to texture
            bufferWrite((char*)targetBuffer, (char*)data,
                        x,y,
                        info->w,info->h, w, info->w );
        }
        if (int(info->h) > maxy)
            maxy = info->h;
        x = newx;
    }
}

//verifica se os caracteres cabem nesse w/h
bool checkSizeForThisResolution(int w,int h, int spaces, PixelFont *pixelFont){
    int x = spaces;
    int y = spaces;

    //increment the line
    int maxy = 0;

    const std::vector<UTF32>& charset = pixelFont->getCharset();
    for (size_t i=0;i<charset.size();i++){
        UTF32 c = charset[i];
        PixelFont::GlyphInfo *info;
        void* data;
        pixelFont->getGlyph(c,&info,&data);
        int newx = x + info->w + spaces;
        if (i==0 && newx > w){
            return false;
        }else if (newx > w){
            newx = spaces;
            y += maxy + spaces;
            maxy = 0;
            newx += info->w + spaces;
            if (newx > w){
                return false;
            }
        }
        if (int(info->h) > maxy)
            maxy = info->h;
        x = newx;
    }
    if (y+maxy+spaces > h)
        return false;
    return true;
}

#include <stdio.h>

void computeTextureMinSize(int spaces, int iw,int ih,PixelFont *pixelFont, int *wr, int *hr){
    int w = iw;
    int h = ih;
    //space between fonts
    //int spaces = 1;

    //faz incrementar um eixo, depois outro quando aumentar o tamanho da area da textura

    int double_turn = 0;
    printf("Double turn OFF!!!\n");
    printf("w: %d h: %d\n",w,h);
    while (!checkSizeForThisResolution(w,h,spaces,pixelFont)){
        if (double_turn == 0)
            w = w << 1;
        else
            h = h << 1;
        double_turn = (double_turn+1)%2;
        printf("w: %d h: %d\n",w,h);
    }
    *wr = w;
    *hr = h;

    //test with double_turn on
    double_turn = 1;
    w = iw;
    h = ih;
    printf("Double turn ON!!!\n");
    printf("w: %d h: %d\n",w,h);
    while (!checkSizeForThisResolution(w,h,spaces,pixelFont)){
        if (double_turn == 0)
            w = w << 1;
        else
            h = h << 1;
        double_turn = (double_turn+1)%2;
        printf("w: %d h: %d\n",w,h);
    }

    printf("Check double turn result...\n");
    if ((*wr)*(*hr) > w*h){
        *wr = w;
        *hr = h;
    }

    printf("Best resolution: %dx%d !!!\n",*wr,*hr);

}

void invertBuffer(int w,int h,char*buffer){
    char*aux = new char[w*h*4];
    for (int i=0;i<h/2;i++){
        memcpy(aux,&buffer[i*w*4],w*4);
        memcpy(&buffer[i*w*4],&buffer[(h-1-i)*w*4],w*4);
        memcpy(&buffer[(h-1-i)*w*4],aux,w*4);
    }
    delete[] aux;
}

TextureFontRenderer::TextureFontRenderer(PixelFont *pixelFont, int w_h, int space_between_chars){
    int finalW,finalH;
    //check the w/h param
    computeTextureMinSize(space_between_chars,w_h,w_h,pixelFont, &finalW,&finalH);
    //this->mWH = finalW;
    mW = finalW;
    mH = finalH;
    printf("Final texture size: %dx%d\n",mW,mH);
    fflush(stdout);
    //alloc the buffer
    textureBuffer = (char*)malloc(mW*mH*4);//RGBA - 8bpp

    glyph_w = pixelFont->width();
    glyph_h = pixelFont->height();


    //generate texture
    fillBuffer_RGBA(textureBuffer, mW, mH, space_between_chars, pixelFont, &glyphs);
    invertBuffer(mW,mH,textureBuffer);
}

TextureFontRenderer::~TextureFontRenderer(){
    if (textureBuffer)
        free(textureBuffer);
}

TextureFontRenderer::TextureFontRenderer(const TextureFontRenderer& param){
    //memcpy(this,&param,sizeof(TextureFontRenderer));

    mW = param.mW;
    mH = param.mH;
    glyphs = param.glyphs;
    glyph_w = param.glyph_w;
    glyph_h = param.glyph_h;

    if (this->textureBuffer){
        textureBuffer = (char*)malloc(mW*mH*4);
        memcpy(textureBuffer,param.textureBuffer,sizeof(char)*mW*mH*4);
    }
}

void TextureFontRenderer::operator=(const TextureFontRenderer& param){
    if (textureBuffer)
        free(textureBuffer);
    //memcpy(this,&param,sizeof(TextureFontRenderer));

    mW = param.mW;
    mH = param.mH;
    glyphs = param.glyphs;
    glyph_w = param.glyph_w;
    glyph_h = param.glyph_h;

    if (this->textureBuffer){
        textureBuffer = (char*)malloc(mW*mH*4);
        memcpy(textureBuffer,param.textureBuffer,sizeof(char)*mW*mH*4);
    }
}



//#include "pngwriter.h"


TextureFontRenderer::GlyphInfo* TextureFontRenderer::getGlyph(UTF32 c){
    std::map<UTF32, GlyphInfo>::iterator it = glyphs.find(c);
    if (it != glyphs.end()){
        return &it->second;
    }
    return NULL;
}


void TextureFontRenderer::exportSFont(const char* file_meta, const char* file_png, const char* file_png_gray){

    if (file_png != NULL)
        PNGHelper::writePNG(file_png, mW, mH, 4, textureBuffer, true);
    
    if (file_png_gray != NULL)
    {
        char* luminance_tex = new char[mW*mH];
        for (int y = 0; y < mH; y++)
            for (int x = 0; x < mW; x++) {
                luminance_tex[x + y * mW] = textureBuffer[(x + y * mW) * 4 + 3];//get alpha channel and copy it to the luminance texture
            }

        PNGHelper::writePNG(file_png_gray, mW, mH, 1, luminance_tex);

        delete[] luminance_tex;
    }

    //PNGWriter::getSingleton()->writeRGBA8888(textureBuffer,mW,mH,file_png);

    FILE* out = fopen(file_meta,"w");
    if (out){
        int space_width = glyph_w;
        //get space glyph
        GlyphInfo* space = getGlyph(L' ');
        if (space)
            space_width = space->advX;
        //write header
        fprintf(out,"FONT_TOOL_FILE %u %u %u %u %u %u\n",
               mW ,//<TexW>
               mH ,//<TexH>
               space_width ,//<Space_size_x>
               glyph_h,//<new_line_size_y>
               1,//<pixel_format=1(8888)>
               (unsigned int)glyphs.size()//<GlyphsCount>
               );
        //iterate over all glyphs
        std::map<UTF32, TextureFontRenderer::GlyphInfo>::iterator it;
        for (it = glyphs.begin();it!=glyphs.end();it++){
            UTF32 c = it->first;
            TextureFontRenderer::GlyphInfo& glyph = it->second;
            fprintf(out,"%u %d %u %u %u %u %u\n",
                   (unsigned int)c,//<unicode char code>
                   glyph.y + 6 ,//<ascent_y>
                   glyph.texX - 3,//<tex_left_x>
                   glyph.texY - 3,//<tex_top_y>
                   glyph.w + 6,//<glyph_w>
                   glyph.h + 6,//<glyph_h>
                   glyph.advX //<increment_x>
                   );

        }

        fclose(out);
    }

}


void TextureFontRenderer::exportASilvaBinTable(const char* tablefileBin, const char* file_png, const char* file_png_gray){

    if (file_png != NULL)
    PNGHelper::writePNG(file_png, mW, mH, 4, textureBuffer, true);

    if (file_png_gray != NULL)
    {
        char* luminance_tex = new char[mW*mH];
        for (int y = 0; y < mH; y++)
            for (int x = 0; x < mW; x++)
                luminance_tex[x + y * mW] = textureBuffer[(x + y * mW) * 4 + 3];//get alpha channel and copy it to the luminance texture

        PNGHelper::writePNG(file_png_gray, mW, mH, 1, luminance_tex, true);

        delete[] luminance_tex;
    }

    //PNGWriter::getSingleton()->writeRGBA8888(textureBuffer,mW,mH,file_png);

    FILE* out = fopen(tablefileBin,"wb");
    if (out){
        int space_width = glyph_w;
        //get space glyph
        GlyphInfo* space = getGlyph(L' ');
        if (space)
            space_width = space->advX;

        //file ID
        fwrite(".asilva.",sizeof(int),2,out);//lib ID
        fwrite("glyphtab",sizeof(int),2,out);//file type id
        //fwrite("lum.font",sizeof(int),2,out);//file type id

        //headers
        unsigned short sw = mW;
        unsigned short sh = mH;
        unsigned short sspace = space_width;
        unsigned short sglyphH = glyph_h;

        fwrite(&sw,sizeof(short),1,out);//texW
        fwrite(&sh,sizeof(short),1,out);//texH
        fwrite(&sspace,sizeof(short),1,out);
        fwrite(&sglyphH,sizeof(short),1,out);//new line height


        //glyph table
        unsigned short count = (unsigned short)glyphs.size();
        fwrite(&count,sizeof(short),1,out);//numberOfGlyphs

        //iterate over all glyphs
        std::map<UTF32, TextureFontRenderer::GlyphInfo>::iterator it;
        for (it = glyphs.begin();it!=glyphs.end();it++){
            UTF32 c = it->first;
            TextureFontRenderer::GlyphInfo& glyph = it->second;
            fwrite(&c,sizeof(UTF32),1,out);
            fwrite(&glyph,sizeof(TextureFontRenderer::GlyphInfo),1,out);
        }

        fclose(out);
    }
}



void TextureFontRenderer::exportGLPack(const char* outfileBin){
    FILE* out = fopen(outfileBin,"wb");
    if (out){
        int space_width = glyph_w;
        //get space glyph
        GlyphInfo* space = getGlyph(L' ');
        if (space)
            space_width = space->advX;

        //file ID
        fwrite(".asilva.",sizeof(int),2,out);//lib ID
        //fwrite("glyphtab",sizeof(int),2,out);//file type id
        fwrite("lum.font",sizeof(int),2,out);//file type id

        //headers
        unsigned short sw = mW;
        unsigned short sh = mH;
        unsigned short sspace = space_width;
        unsigned short sglyphH = glyph_h;

        fwrite(&sw,sizeof(short),1,out);//texW
        fwrite(&sh,sizeof(short),1,out);//texH
        fwrite(&sspace,sizeof(short),1,out);
        fwrite(&sglyphH,sizeof(short),1,out);//new line height


        //glyph table
        unsigned short count = (unsigned short)glyphs.size();
        fwrite(&count,sizeof(unsigned short),1,out);//numberOfGlyphs

        //iterate over all glyphs
        std::map<UTF32, TextureFontRenderer::GlyphInfo>::iterator it;
        for (it = glyphs.begin();it!=glyphs.end();it++){
            UTF32 c = it->first;
            TextureFontRenderer::GlyphInfo& glyph = it->second;
            fwrite(&c,sizeof(UTF32),1,out);
            fwrite(&glyph,sizeof(TextureFontRenderer::GlyphInfo),1,out);
        }
        //write the luminance texture
        char* luminance_tex = new char[mW*mH];

        for(int y=0;y<mH;y++)
        for(int x=0;x<mW;x++)
            luminance_tex[x+y*mW] = textureBuffer[(x+y*mW)*4+3];//get alpha channel and copy it to the luminance texture


        unsigned char rle_count = 0;
        int state = 0;
#define NORMAL 0
#define BLACK 1
#define WHITE 2
        for (int i=0;i<mW*mH;i++){
            if (luminance_tex[i]=='\x00'){
                if (state != BLACK){
                    if (state != NORMAL){
                        //flush
                        fwrite(&rle_count,sizeof(char),1,out);
                    }
                    fwrite(&luminance_tex[i],sizeof(char),1,out);
                    state = BLACK;
                    rle_count = 1;
                }else{
                    //state = black
                    if (rle_count == 255){
                        //flush
                        fwrite(&rle_count,sizeof(char),1,out);
                        char black = '\x00';
                        fwrite(&black,sizeof(char),1,out);
                        rle_count = 0;
                    }
                    rle_count++;
                }
            }else if (luminance_tex[i]=='\xff'){
                if (state != WHITE){
                    if (state != NORMAL){
                        //flush
                        fwrite(&rle_count,sizeof(char),1,out);
                    }
                    fwrite(&luminance_tex[i],sizeof(char),1,out);
                    state = WHITE;
                    rle_count = 1;
                }else{
                    //state = white
                    if (rle_count == 255){
                        //flush
                        fwrite(&rle_count,sizeof(char),1,out);
                        char white = '\xff';
                        fwrite(&white,sizeof(char),1,out);
                        rle_count = 0;
                    }
                    rle_count++;
                }
            }else{
                if (state != NORMAL){
                    //flush
                    fwrite(&rle_count,sizeof(char),1,out);
                    state = NORMAL;
                }
                //normal case
                fwrite(&luminance_tex[i],sizeof(char),1,out);
            }
        }
        if (state != NORMAL){
            //flush
            fwrite(&rle_count,sizeof(char),1,out);
            state = NORMAL;
        }

        //fwrite(luminance_tex,sizeof(char),mW*mH,out);
        delete[]luminance_tex;

        fclose(out);
    }
}
