
#include "MipMapGenerator.h"

namespace openglWrapper {
    
    MipMapImage::MipMapImage(int width, int height, int channels) {
        this->width = width;
        this->height = height;
        this->channels = channels;
        buffer = new uint8_t[width*height*channels];
    }
    
    void MipMapImage::copyBuffer(uint8_t* _buffer) {
        memcpy(buffer, _buffer, sizeof(uint8_t)*width*height*channels);
    }
    
    uint8_t MipMapImage::getPixel(int x, int y, int component)const{
        if (channels == 4 && component != 3) {
            return uint8_t( (
                (uint32_t)buffer[(x + y * width)*channels + component] * 
                (uint32_t)buffer[(x + y * width)*channels + 3]
                ) / (uint32_t)255 );
        }
        return buffer[(x+y*width)*channels + component];
    }
    
    void MipMapImage::setPixel(int x, int y, int component, uint8_t v) {
        if (x >= width)
            x = width-1;
        if (y >= height)
            y = height-1;
        buffer[(x+y*width)*channels + component] = v;
    }
    
    void MipMapImage::copyBufferResample(const MipMapImage& src) {
        ARIBEIRO_ABORT((width > 1 && (width != (src.width >> 1))) ||
                        (height > 1 && (height != (src.height >> 1))), "Mip Map Processing error...\n" );
        
        for(int y=0;y < height; y++ ){
            for(int x=0;x < width; x++ ){
                for (int ch = 0;ch<channels;ch++){

                    int v = (int)src.getPixel(x*2+0,y*2+0,ch) +
                            (int)src.getPixel(x*2+1,y*2+0,ch) +
                            (int)src.getPixel(x*2+0,y*2+1,ch) +
                            (int)src.getPixel(x*2+1,y*2+1,ch);
                    v /= 4;
                    
                    setPixel(x,y,ch,(uint8_t)v);
                }
            }
        }
    }
    
    MipMapImage::~MipMapImage() {
        delete[] buffer;
    }


    MipMapGenerator::MipMapGenerator(uint8_t* buffer,
                    int width,
                    int height,
                    int channels){
        
        bool power_of_two_w = width && !(width & (width - 1));
        bool power_of_two_h = height && !(height & (height - 1));
        
        ARIBEIRO_ABORT((!power_of_two_w || !power_of_two_h),"MipMap Generator need texture to be power of two.\n");
        
        //allocating memory for all mips
        
        while (width != 1 && height != 1) {
            images.push_back(new MipMapImage(width,height,channels));
            width >>= 1;
            height >>= 1;
            if (width == 0) width = 1;
            if (height == 0) height = 1;
        }

        //1x1
        images.push_back(new MipMapImage(width, height, channels));
        
        images[0]->copyBuffer( buffer );
        
        for(size_t i=1;i<images.size();i++)
            images[i]->copyBufferResample( *images[i-1] );
        
    }
    
    MipMapGenerator::~MipMapGenerator() {
        for(size_t i=0;i<images.size();i++){
            delete images[i];
        }
        images.clear();
    }
    
}


