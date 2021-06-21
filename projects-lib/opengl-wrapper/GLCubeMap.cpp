#include <glew/glew.h>

#include "GLCubeMap.h"

//#include <mini-gl-engine/mini-gl-engine.h>

#include <opengl-wrapper/PlatformGL.h>

#include <opengl-wrapper/MipMapGenerator.h>

namespace openglWrapper {

    void GLCubeMap::copySquareAndUpload(int x, int y, int w, int h,
                                        int channels,
                                        char* src, int src_stridex ,
                                        char* dst,
                                        GLuint target) {

        for(int _y = 0; _y< h;_y++){
            memcpy( &dst[_y*w*channels],
                   &src[(y+_y)*src_stridex + x * channels],
                   w*channels
                   );
        }

        if (channels == 1) {
            uploadBufferAlpha8(target, dst, w, h);
        } else if (channels == 3) {
            uploadBufferRGB_888(target, dst, w, h);
        } else if(channels == 4) {
            uploadBufferRGBA_8888(target, dst, w, h);
        }

    }

    GLCubeMap::GLCubeMap(bool _sRGB, int _maxResolution) {

        sRGB = _sRGB;

        maxResolution = _maxResolution;
        #if ARIBEIRO_RPI
            // tested on Raspbery Pi 3
            if (maxResolution > 512)
                maxResolution = 512;
        #else
            // tested on Intel HD3000
            if (maxResolution > 1024)
                maxResolution = 1024;
        #endif

        width = 0;
        height = 0;

        printf("Creating a cubemap with the resolution: %i\n",maxResolution);

        ARIBEIRO_ABORT(!GLEW_ARB_texture_cube_map, "Error, this hardware does not support ARB_texture_cube_map...\n");

        OPENGL_CMD(glGenTextures(1, &mCube));
        glBindTexture(GL_TEXTURE_CUBE_MAP, mCube);

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        //glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }

    void GLCubeMap::active(int id) {

        if (id != 0)
            ARIBEIRO_ABORT(!GLEW_ARB_multitexture,"ARB_multitexture not supported.");

        OPENGL_CMD(glActiveTexture(GL_TEXTURE0 + id));
#if !defined(ARIBEIRO_RPI)
        //OPENGL_CMD(glEnable(GL_TEXTURE_2D));
#endif
        OPENGL_CMD(glBindTexture(GL_TEXTURE_CUBE_MAP, mCube));
    }

    void GLCubeMap::deactive(int id) {
        OPENGL_CMD(glActiveTexture(GL_TEXTURE0 + id));
#if !defined(ARIBEIRO_RPI)
        //OPENGL_CMD(glDisable(GL_TEXTURE_2D));
#endif
        OPENGL_CMD(glBindTexture(GL_TEXTURE_CUBE_MAP, 0));
    }

    GLCubeMap::~GLCubeMap(){
        if (mCube != 0 && glIsTexture(mCube))
            OPENGL_CMD(glDeleteTextures(1, &mCube));
        mCube = 0;
    }


    void GLCubeMap::uploadBufferAlpha8(GLuint cube_target, const void* buffer_, int w_, int h_) {

        ARIBEIRO_ABORT((width != 0 && height != 0) && (width != w_) && (height != h_), "ERROR: trying to load different size cubemap texture set\n.");

        width = w_;
        height = h_;

        const void* buffer = buffer_;
        int w = w_;
        int h = h_;

        bool power_of_two_w = w && !(w & (w - 1));
        bool power_of_two_h = h && !(h & (h - 1));

        if (!power_of_two_w || !power_of_two_h)
            ARIBEIRO_ABORT(!GLEW_ARB_texture_non_power_of_two,"ARB_texture_non_power_of_two not supported.");

        MipMapGenerator *generator = NULL;

        //resample buffer
        //if (w > maxResolution || h > maxResolution)
        {
            aRibeiro::PlatformTime timer;

            printf("Generating mipmap on CPU...\n");
            timer.update();
            generator = new MipMapGenerator((uint8_t*)buffer_, w_, h_,1);
            for(size_t i=0;i<generator->images.size();i++){
                MipMapImage* image = generator->images[i];
                if (image->width <= maxResolution && image->height <= maxResolution ){
                    buffer = (const void *)image->buffer;
                    w = image->width;
                    h = image->height;
                    break;
                }
            }
            timer.update();
            printf("Time: %f\n", timer.deltaTime);

            aRibeiro::vec3 estimation = aRibeiro::vec3(
                generator->images[generator->images.size() - 1]->getPixel(0, 0, 0)
            ) / 255.0f;

            //texture to Gamma...
            estimation.x = pow(estimation.x, 2.2f);
            estimation.y = pow(estimation.y, 2.2f);
            estimation.z = pow(estimation.z, 2.2f);

            estimation /= 6.0f;//6 sides

            colorEstimation += estimation;
        }

        //active(0);

        OPENGL_CMD(glBindTexture(GL_TEXTURE_CUBE_MAP, mCube));

        int originalPackAlignment;
        int originalUnpackAlignment;
        OPENGL_CMD(glGetIntegerv(GL_PACK_ALIGNMENT, &originalPackAlignment));
        OPENGL_CMD(glGetIntegerv(GL_UNPACK_ALIGNMENT, &originalUnpackAlignment));

        //set pack data alignment to 1 byte read from memory and 1 byte to write to gpu memory...
        OPENGL_CMD(glPixelStorei(GL_PACK_ALIGNMENT, 1));
        OPENGL_CMD(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));

        //if (w != width || h != height) {
            OPENGL_CMD(glTexImage2D(cube_target, 0, GL_ALPHA, w, h, 0, GL_ALPHA, GL_UNSIGNED_BYTE, buffer));
            //width = w;
            //height = h;
        //}
        //else
            //OPENGL_CMD(glTexSubImage2D(cube_target, 0, 0, 0, w, h, GL_ALPHA, GL_UNSIGNED_BYTE, buffer));

        //back the pack alignment for texture operations
        OPENGL_CMD(glPixelStorei(GL_PACK_ALIGNMENT, originalPackAlignment));
        OPENGL_CMD(glPixelStorei(GL_UNPACK_ALIGNMENT, originalUnpackAlignment));

        OPENGL_CMD(glBindTexture(GL_TEXTURE_CUBE_MAP, 0));

        //deactive(0);

        aRibeiro::setNullAndDelete(generator);
    }

    void GLCubeMap::uploadBufferRGB_888(GLuint cube_target, const void* buffer_, int w_, int h_) {

        ARIBEIRO_ABORT((width != 0 && height != 0) && (width != w_) && (height != h_), "ERROR: trying to load different size cubemap texture set\n.");

        width = w_;
        height = h_;

        const void* buffer = buffer_;
        int w = w_;
        int h = h_;

        bool power_of_two_w = w && !(w & (w - 1));
        bool power_of_two_h = h && !(h & (h - 1));

        if (!power_of_two_w || !power_of_two_h)
            ARIBEIRO_ABORT(!GLEW_ARB_texture_non_power_of_two,"ARB_texture_non_power_of_two not supported.");

        MipMapGenerator *generator = NULL;

        //resample buffer
        //if (w > maxResolution || h > maxResolution)
        {
            aRibeiro::PlatformTime timer;

            printf("Generating mipmap on CPU...\n");
            timer.update();
            generator = new MipMapGenerator((uint8_t*)buffer_, w_, h_,3);
            for(size_t i=0;i<generator->images.size();i++){
                MipMapImage* image = generator->images[i];
                if (image->width <= maxResolution && image->height <= maxResolution ){
                    buffer = (const void *)image->buffer;
                    w = image->width;
                    h = image->height;
                    break;
                }
            }
            timer.update();
            printf("Time: %f\n", timer.deltaTime);

            aRibeiro::vec3 estimation = aRibeiro::vec3(
                generator->images[generator->images.size() - 1]->getPixel(0, 0, 0),
                generator->images[generator->images.size() - 1]->getPixel(0, 0, 1),
                generator->images[generator->images.size() - 1]->getPixel(0, 0, 2)
            ) / 255.0f;

            //texture to Gamma...
            estimation.x = pow(estimation.x, 2.2f);
            estimation.y = pow(estimation.y, 2.2f);
            estimation.z = pow(estimation.z, 2.2f);

            estimation /= 6.0f;//6 sides

            colorEstimation += estimation;
        }

        int originalPackAlignment;
        int originalUnpackAlignment;
        OPENGL_CMD(glGetIntegerv(GL_PACK_ALIGNMENT, &originalPackAlignment));
        OPENGL_CMD(glGetIntegerv(GL_UNPACK_ALIGNMENT, &originalUnpackAlignment));

        OPENGL_CMD(glBindTexture(GL_TEXTURE_CUBE_MAP, mCube));

        //set pack data alignment to 1 byte read from memory and 1 byte to write to gpu memory...
        OPENGL_CMD(glPixelStorei(GL_PACK_ALIGNMENT, 1));
        OPENGL_CMD(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));

        //if (w != width || h != height) {
        if (sRGB) {
            ARIBEIRO_ABORT(!GLEW_EXT_texture_sRGB, "Graphics driver does not support texture sRGB\n.");
            OPENGL_CMD(glTexImage2D(cube_target, 0, GL_SRGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, buffer));
        }
        else
            OPENGL_CMD(glTexImage2D(cube_target, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, buffer));
            //width = w;
            //height = h;
        //}
        //else
            //OPENGL_CMD(glTexSubImage2D(cube_target, 0, 0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, buffer));

        //back the pack alignment for texture operations
        OPENGL_CMD(glPixelStorei(GL_PACK_ALIGNMENT, originalPackAlignment));
        OPENGL_CMD(glPixelStorei(GL_UNPACK_ALIGNMENT, originalUnpackAlignment));

        OPENGL_CMD(glBindTexture(GL_TEXTURE_CUBE_MAP, 0));

        aRibeiro::setNullAndDelete(generator);
    }

    void GLCubeMap::uploadBufferRGBA_8888(GLuint cube_target, const void* buffer_, int w_, int h_) {

        ARIBEIRO_ABORT((width != 0 && height != 0) && (width != w_) && (height != h_), "ERROR: trying to load different size cubemap texture set\n.");

        width = w_;
        height = h_;

        const void* buffer = buffer_;
        int w = w_;
        int h = h_;

        bool power_of_two_w = w && !(w & (w - 1));
        bool power_of_two_h = h && !(h & (h - 1));

        if (!power_of_two_w || !power_of_two_h)
            ARIBEIRO_ABORT(!GLEW_ARB_texture_non_power_of_two,"ARB_texture_non_power_of_two not supported.");

        MipMapGenerator *generator = NULL;

        //resample buffer
        //if (w > maxResolution || h > maxResolution)
        {
            aRibeiro::PlatformTime timer;

            printf("Generating mipmap on CPU...\n");
            timer.update();
            generator = new MipMapGenerator((uint8_t*)buffer_, w_, h_,4);
            for(size_t i=0;i<generator->images.size();i++){
                MipMapImage* image = generator->images[i];
                if (image->width <= maxResolution && image->height <= maxResolution ){
                    buffer = (const void *)image->buffer;
                    w = image->width;
                    h = image->height;
                    break;
                }
            }
            timer.update();
            printf("Time: %f\n", timer.deltaTime);

            float alpha = (float)generator->images[generator->images.size() - 1]->getPixel(0, 0, 3) / 255.0f;

            aRibeiro::vec3 estimation = aRibeiro::vec3(
                generator->images[generator->images.size() - 1]->getPixel(0, 0, 0),
                generator->images[generator->images.size() - 1]->getPixel(0, 0, 1),
                generator->images[generator->images.size() - 1]->getPixel(0, 0, 2)
            ) / 255.0f;

            //texture to Gamma...
            estimation.x = pow(estimation.x, 2.2f);
            estimation.y = pow(estimation.y, 2.2f);
            estimation.z = pow(estimation.z, 2.2f);

            estimation /= 6.0f;//6 sides

            estimation *= alpha;

            colorEstimation += estimation;
        }

        OPENGL_CMD(glBindTexture(GL_TEXTURE_CUBE_MAP, mCube));

        //set pack data alignment to 1 byte read from memory and 1 byte to write to gpu memory...
        //OPENGL_CMD(glPixelStorei(GL_PACK_ALIGNMENT, 1));
        //OPENGL_CMD(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));

        //if (w != width || h != height) {
        if (sRGB) {
            ARIBEIRO_ABORT(!GLEW_EXT_texture_sRGB, "Graphics driver does not support texture sRGB\n.");
            OPENGL_CMD(glTexImage2D(cube_target, 0, GL_SRGB_ALPHA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer));
        }
        else
            OPENGL_CMD(glTexImage2D(cube_target, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer));
            //width = w;
            //height = h;
        //}
        //else
            //OPENGL_CMD(glTexSubImage2D(cube_target, 0, 0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, buffer));

        //back the pack alignment for texture operations
        //OPENGL_CMD(glPixelStorei(GL_PACK_ALIGNMENT, originalPackAlignment));
        //OPENGL_CMD(glPixelStorei(GL_UNPACK_ALIGNMENT, originalUnpackAlignment));

        OPENGL_CMD(glBindTexture(GL_TEXTURE_CUBE_MAP, 0));

        aRibeiro::setNullAndDelete(generator);
    }

    void GLCubeMap::setSizeAndFormat(int w, int h, GLuint format) {

        ARIBEIRO_ABORT(
            !(format == GL_RGB || format == GL_RGBA || format == GL_ALPHA),
            "Error... type not tested with a cubemap\n."
        );

        if (width == w && height == h)
            return;

        width = w;
        height = h;

        GLuint internalFormat = format;
        if (sRGB) {
            ARIBEIRO_ABORT(!GLEW_EXT_texture_sRGB, "Graphics driver does not support texture sRGB\n.");
            if (format == GL_RGB)
                internalFormat = GL_SRGB;
            if (format == GL_RGBA)
                internalFormat = GL_SRGB_ALPHA;
        }

        OPENGL_CMD(glBindTexture(GL_TEXTURE_CUBE_MAP, mCube));

        OPENGL_CMD(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, internalFormat, w, h, 0, format, GL_UNSIGNED_BYTE, 0));
        OPENGL_CMD(glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, internalFormat, w, h, 0, format, GL_UNSIGNED_BYTE, 0));
        OPENGL_CMD(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, internalFormat, w, h, 0, format, GL_UNSIGNED_BYTE, 0));
        OPENGL_CMD(glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, internalFormat, w, h, 0, format, GL_UNSIGNED_BYTE, 0));
        OPENGL_CMD(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, internalFormat, w, h, 0, format, GL_UNSIGNED_BYTE, 0));
        OPENGL_CMD(glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, internalFormat, w, h, 0, format, GL_UNSIGNED_BYTE, 0));

        OPENGL_CMD(glBindTexture(GL_TEXTURE_CUBE_MAP, 0));
    }

    void GLCubeMap::generateMipMap() {
        active(0);
        OPENGL_CMD(glGenerateMipmap(GL_TEXTURE_CUBE_MAP));
        OPENGL_CMD(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
        //OPENGL_CMD(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        deactive(0);
    }

    void GLCubeMap::loadFromFile_SingleSide(GLuint cube_target, const char* filename, bool invertY) {
        int w, h, channels, depth;

        char* buffer = NULL;

        bool isPNG = aRibeiro::PNGHelper::isPNGFilename(filename);
        bool isJPG = aRibeiro::JPGHelper::isJPGFilename(filename);

        void (*closeFnc)(char *&) = NULL;

        if (isPNG) {
            buffer = aRibeiro::PNGHelper::readPNG(filename, &w, &h, &channels, &depth, invertY);
            closeFnc = &aRibeiro::PNGHelper::closePNG;
        } else if (isJPG){
            buffer = aRibeiro::JPGHelper::readJPG(filename, &w, &h, &channels, &depth, invertY);
            closeFnc = &aRibeiro::JPGHelper::closeJPG;
        }

        ARIBEIRO_ABORT(
                       buffer == NULL,
                       "error to load: %s\n", filename);

        GLint max_tex_size;
        OPENGL_CMD(glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE, &max_tex_size));

        ARIBEIRO_ABORT ((w > max_tex_size || h > max_tex_size),
                        "Max cube texture size (%i) overloading. File: %s\n",max_tex_size, filename);

        if (channels == 1 && depth == 8) {
            uploadBufferAlpha8(cube_target, buffer, w, h);
            closeFnc(buffer);
            return;
        } else if (channels == 3 && depth == 8) {
            uploadBufferRGB_888(cube_target, buffer, w, h);
            closeFnc(buffer);
            return;
        } else if(channels == 4 && depth == 8) {
            uploadBufferRGBA_8888(cube_target, buffer, w, h);
            closeFnc(buffer);
            return;
        }

        closeFnc(buffer);
        ARIBEIRO_ABORT(
                       true,
                       "invalid image format: %d channels %d depth. Error to load: %s\n", channels, depth, filename);
        return ;
    }

    void GLCubeMap::loadFromFile(const std::string &_negz,const std::string &_posz,
                                 const std::string &_negx,const std::string &_posx,
                                 const std::string &_negy,const std::string &_posy,
                                 bool LeftHanded ) {

        colorEstimation = aRibeiro::vec3(0);

        if (LeftHanded){
            loadFromFile_SingleSide(GL_TEXTURE_CUBE_MAP_POSITIVE_X, _posx.c_str(), false);
            loadFromFile_SingleSide(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, _negx.c_str(), false);
        } else {
            loadFromFile_SingleSide(GL_TEXTURE_CUBE_MAP_POSITIVE_X, _negx.c_str(), false);
            loadFromFile_SingleSide(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, _posx.c_str(), false);
        }

        loadFromFile_SingleSide(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, _posy.c_str(), false);
        loadFromFile_SingleSide(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, _negy.c_str(), false);

        //Left Handed load
        if (LeftHanded){
            loadFromFile_SingleSide(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, _posz.c_str(), false);
            loadFromFile_SingleSide(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, _negz.c_str(), false);
        } else {
            loadFromFile_SingleSide(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, _negz.c_str(), false);
            loadFromFile_SingleSide(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, _posz.c_str(), false);
        }

        //Gamma to texture
        colorEstimation = colorEstimation / (colorEstimation + aRibeiro::vec3(1));
        colorEstimation.x = pow(colorEstimation.x, 1.0f / 2.2f);
        colorEstimation.y = pow(colorEstimation.y, 1.0f / 2.2f);
        colorEstimation.z = pow(colorEstimation.z, 1.0f / 2.2f);


        //generate mipmap
        ARIBEIRO_ABORT(!GLEW_SGIS_generate_mipmap,"SGIS_generate_mipmap not supported.");

        active(0);
        OPENGL_CMD(glGenerateMipmap(GL_TEXTURE_CUBE_MAP));
        OPENGL_CMD(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
        OPENGL_CMD(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        deactive(0);
    }

    void GLCubeMap::loadFromSingleFile(std::string single_file,
                                       bool LeftHanded) {
        int w, h, channels, depth;

        bool isPNG = aRibeiro::PNGHelper::isPNGFilename(single_file.c_str());
        bool isJPG = aRibeiro::JPGHelper::isJPGFilename(single_file.c_str());

        char* buffer = NULL;

        if ( isPNG )
            buffer = aRibeiro::PNGHelper::readPNG(single_file.c_str(), &w, &h, &channels, &depth, false);
        else if (isJPG)
            buffer = aRibeiro::JPGHelper::readJPG(single_file.c_str(), &w, &h, &channels, &depth, false);

        ARIBEIRO_ABORT(
                       buffer == NULL,
                       "error to load: %s\n", single_file.c_str());

        ARIBEIRO_ABORT( (depth != 8), "Bit depth (%i) not supported.\n",depth);

        bool power_of_two_w = w && !(w & (w - 1));
        bool power_of_two_h = h && !(h & (h - 1));

        if (!power_of_two_w || !power_of_two_h)
        {
            ARIBEIRO_ABORT(true,"non Power Of Two cubemap: %s\n", single_file.c_str());
        }
            //ARIBEIRO_ABORT(!GLEW_ARB_texture_non_power_of_two,"ARB_texture_non_power_of_two not supported.");

        GLuint target_left,target_right,target_top,target_bottom,target_front,target_back;

        if (LeftHanded){
            target_left = GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
            target_right = GL_TEXTURE_CUBE_MAP_POSITIVE_X;
        } else {
            target_left = GL_TEXTURE_CUBE_MAP_POSITIVE_X;
            target_right = GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
        }

        target_top =GL_TEXTURE_CUBE_MAP_POSITIVE_Y;
        target_bottom =GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;

        if (LeftHanded){
            target_front = GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
            target_back = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
        } else {
            target_front = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
            target_back = GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
        }


        //load each buffer...
        int internal_w = w / 4;
        int internal_h = h / 3;

        char* internal_buffer = new char[internal_w*internal_h*channels];

        // left
        copySquareAndUpload( 0, internal_h, internal_w, internal_h,
                            channels,
                            buffer, w*channels,
                            internal_buffer,
                            target_left);

        // right
        copySquareAndUpload( internal_w*2, internal_h, internal_w, internal_h,
                            channels,
                            buffer, w*channels,
                            internal_buffer,
                            target_right);

        // front
        copySquareAndUpload( internal_w, internal_h, internal_w, internal_h,
                            channels,
                            buffer, w*channels,
                            internal_buffer,
                            target_front);

        // back
        copySquareAndUpload( internal_w*3, internal_h, internal_w, internal_h,
                            channels,
                            buffer, w*channels,
                            internal_buffer,
                            target_back);

        // top
        copySquareAndUpload( internal_w, 0, internal_w, internal_h,
                            channels,
                            buffer, w*channels,
                            internal_buffer,
                            target_top);

        // down
        copySquareAndUpload( internal_w, internal_h*2, internal_w, internal_h,
                            channels,
                            buffer, w*channels,
                            internal_buffer,
                            target_bottom);

        delete[] internal_buffer;

        if ( isPNG )
            aRibeiro::PNGHelper::closePNG(buffer);
        else if (isJPG)
            aRibeiro::JPGHelper::closeJPG(buffer);

        //generate mipmap
        ARIBEIRO_ABORT(!GLEW_SGIS_generate_mipmap,"SGIS_generate_mipmap not supported.");

        active(0);
        OPENGL_CMD(glGenerateMipmap(GL_TEXTURE_CUBE_MAP));
        OPENGL_CMD(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
        //OPENGL_CMD(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        deactive(0);
    }



}


