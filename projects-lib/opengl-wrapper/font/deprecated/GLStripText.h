#ifndef GLStripText___H
#define GLStripText___H

#include <map>
#include <vector>
#include <glew/glew.h>

#include <aribeiro/all_math.h>

#include <aribeiro/all_math.h>
#include <opengl-wrapper/Font.h>
#include <opengl-wrapper/GLTexture.h>
#include <opengl-wrapper/GLShaderFont.h>
#include <aribeiro/SSE2.h>


namespace openglWrapper {

    class StripInfoVertexAttrib{
    public:
        aRibeiro::vec4 mColor;
        aRibeiro::vec2 mCoord;
        aRibeiro::vec2 mPos;

        SSE2_CLASS_NEW_OPERATOR
    };

    class GLStripText {
        StripInfoVertexAttrib *mStripInfo;
        int mSize;
        
        GLTexture *texture;
        GLShaderFont *shader;

    private:

        // avoid copy
        GLStripText(const GLStripText& v);
        void operator=(const GLStripText&v);

    public:

        void setSize(int s);

        GLStripText(int buffSize, GLTexture *texture, GLShaderFont *shader);
        virtual ~GLStripText();
        StripInfoVertexAttrib& getVertex(int index);
        StripInfoVertexAttrib& getCharVertex(int CharIndex);

        void draw();

        void resetColor(aRibeiro::vec4 c);

        int getCharacterCount();

        void saturateFromBegin(float increment);
    };

}

#endif