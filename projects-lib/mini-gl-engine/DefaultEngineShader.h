#ifndef __default_engine_shader__h_
#define __default_engine_shader__h_

#include <aribeiro/aribeiro.h>
#include <opengl-wrapper/opengl-wrapper.h>
#include <mini-gl-engine/Transform.h>
#include <data-model/data-model.h>
#include <mini-gl-engine/ComponentCamera.h>


namespace GLEngine {

    class DefaultEngineShader: public openglWrapper::GLShader{
    public:
        uint32_t format;
        
        /*
        static const int aPos = 0;
        static const int aNormal = 1;
        static const int aTangent = 2;
        static const int aBinormal = 3;
        static const int aUV0 = 4;
        static const int aUV1 = 5;
        static const int aUV2 = 6;
        static const int aUV3 = 7;
        static const int aUV4 = 8;
        static const int aUV5 = 9;
        static const int aUV6 = 10;
        static const int aUV7 = 11;
        static const int aColor0 = 12;
        static const int aColor1 = 13;
        static const int aColor2 = 14;
        static const int aColor3 = 15;
        static const int aColor4 = 16;
        static const int aColor5 = 17;
        static const int aColor6 = 18;
        static const int aColor7 = 19;
        */

        DefaultEngineShader() {
            format = 0;
            //format = model::CONTAINS_POS | model::CONTAINS_UV0;
        }

        virtual int queryAttribLocation(const char* aname) {
            ARIBEIRO_ABORT(!format,"shader format cannot be 0\n.");
            
            int count = 0;
            
            if (format & model::CONTAINS_POS){
                if (strcmp(aname,"aPosition") == 0)
                    return count;
                count++;
            }
            if (format & model::CONTAINS_NORMAL){
                if (strcmp(aname,"aNormal") == 0)
                    return count;
                count++;
            }
            if (format & model::CONTAINS_TANGENT){
                if (strcmp(aname,"aTangent") == 0)
                    return count;
                count++;
            }
            if (format & model::CONTAINS_BINORMAL){
                if (strcmp(aname,"aBinormal") == 0)
                    return count;
                count++;
            }
            char name[16];
            
            for (int i=0;i<8;i++){
                if (format & (model::CONTAINS_UV0 << i)){
                    sprintf(name,"aUV%i",i);
                    if (strcmp(aname,name) == 0)
                        return count;
                    count++;
                }
                if (format & (model::CONTAINS_COLOR0 << i)){
                    sprintf(name,"aColor%i",i);
                    if (strcmp(aname,name) == 0)
                        return count;
                    count++;
                }
            }

            if (format & model::CONTAINS_VERTEX_WEIGHT_ANY) {
                if (strcmp(aname, "aSkinIndex") == 0)
                    return count;
                count++;
                if (strcmp(aname, "aSkinWeight") == 0)
                    return count;
                count++;
            }

            return -1;
        }

        int getUserAttribLocationStart() {
            int count = 0;
            
            //check max index
            if (format & model::CONTAINS_POS)
                count++;
            if (format & model::CONTAINS_NORMAL)
                count++;
            if (format & model::CONTAINS_TANGENT)
                count++;
            if (format & model::CONTAINS_BINORMAL)
                count++;
            for (int i=0;i<8;i++){
                if (format & (model::CONTAINS_UV0 << i))
                    count++;
                if (format & (model::CONTAINS_COLOR0 << i))
                    count++;
            }
            if (format & model::CONTAINS_VERTEX_WEIGHT_ANY) {
                count++;
                count++;
            }

            return count;
        }
        
        virtual void setupAttribLocation() {
            
            ARIBEIRO_ABORT(!format,"shader format cannot be 0\n.");
            
            int count = 0;
            
            //check max index
            if (format & model::CONTAINS_POS)
                count++;
            if (format & model::CONTAINS_NORMAL)
                count++;
            if (format & model::CONTAINS_TANGENT)
                count++;
            if (format & model::CONTAINS_BINORMAL)
                count++;
            for (int i=0;i<8;i++){
                if (format & (model::CONTAINS_UV0 << i))
                    count++;
                if (format & (model::CONTAINS_COLOR0 << i))
                    count++;
            }

            if (format & model::CONTAINS_VERTEX_WEIGHT_ANY) {
                count++;
                count++;
            }
            
            GLint max;
            OPENGL_CMD(glGetIntegerv(GL_MAX_VERTEX_ATTRIBS,&max));
            ARIBEIRO_ABORT((count >= max), "Max vertex attrib reached (max=%i).\n", max);
            
            count = 0;
            
            if (format & model::CONTAINS_POS)
                bindAttribLocation(count++, "aPosition");
            if (format & model::CONTAINS_NORMAL)
                bindAttribLocation(count++, "aNormal");
            if (format & model::CONTAINS_TANGENT)
                bindAttribLocation(count++, "aTangent");
            if (format & model::CONTAINS_BINORMAL)
                bindAttribLocation(count++, "aBinormal");
            
            char name[16];
            
            for (int i=0;i<8;i++){
                if (format & (model::CONTAINS_UV0 << i)){
                    sprintf(name,"aUV%i",i);
                    bindAttribLocation(count++, name);
                }
                if (format & (model::CONTAINS_COLOR0 << i)){
                    sprintf(name,"aColor%i",i);
                    bindAttribLocation(count++, name);
                }
            }

            if (format & model::CONTAINS_VERTEX_WEIGHT_ANY) {
                bindAttribLocation(count++, "aSkinIndex");
                bindAttribLocation(count++, "aSkinWeight");
            }

        }

    };
}

#endif
