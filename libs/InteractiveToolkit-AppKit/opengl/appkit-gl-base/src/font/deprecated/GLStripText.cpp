
#ifdef _WIN32

#pragma warning(disable : 4996)
#pragma warning(disable : 4244)
#pragma warning(disable : 4309)
#pragma warning(disable : 4018)

#endif

#include <appkit-gl-base/font/deprecated/GLStripText.h>
#include <appkit-gl-base/platform/PlatformGL.h>

#include <stdlib.h>
#include <string.h>

#include <assert.h>


namespace AppKit
{

    namespace OpenGL
    {

        GLStripText::GLStripText(int buffSize, GLTexture *texture, GLShaderFont *shader)
        {
            mStripInfo = new StripInfoVertexAttrib[buffSize];
            mSize = buffSize;

            this->texture = texture;
            this->shader = shader;

            // mTextureID = textureID;
            // TextureManager::getSingleton()->incrementTextureUse(textureID);
        }

        GLStripText::~GLStripText()
        {
            delete[] mStripInfo;
            mStripInfo = nullptr;
            mSize = 0;
            // TextureManager::getSingleton()->releaseTexture(mTextureID);
        }

        StripInfoVertexAttrib &GLStripText::getVertex(int index)
        {
            assert(index >= 0 && index < mSize);
            return mStripInfo[index];
        }
        StripInfoVertexAttrib &GLStripText::getCharVertex(int CharIndex)
        {
            assert(CharIndex * 6 >= 0 && CharIndex * 6 < mSize);
            assert((CharIndex + 1) * 6 <= mSize);
            return mStripInfo[CharIndex * 6];
        }
        void GLStripText::draw()
        {

            if (mSize == 0 || mStripInfo == nullptr)
                return;

            texture->active(0);

            shader->enable();
            shader->setTexture(0);

            OPENGL_CMD(glEnableVertexAttribArray(shader->vPosition));
            OPENGL_CMD(glVertexAttribPointer(shader->vPosition, 2, GL_FLOAT, false, sizeof(StripInfoVertexAttrib), &mStripInfo[0].mPos));

            OPENGL_CMD(glEnableVertexAttribArray(shader->vUV));
            OPENGL_CMD(glVertexAttribPointer(shader->vUV, 2, GL_FLOAT, false, sizeof(StripInfoVertexAttrib), &mStripInfo[0].mCoord));

            OPENGL_CMD(glEnableVertexAttribArray(shader->vColor));
            OPENGL_CMD(glVertexAttribPointer(shader->vColor, 4, GL_FLOAT, false, sizeof(StripInfoVertexAttrib), &mStripInfo[0].mColor));

            OPENGL_CMD(glDrawArrays(GL_TRIANGLES, 0, mSize));

            OPENGL_CMD(glDisableVertexAttribArray(shader->vPosition));
            OPENGL_CMD(glDisableVertexAttribArray(shader->vUV));
            OPENGL_CMD(glDisableVertexAttribArray(shader->vColor));

            texture->deactive(0);
            // shader->disable();
        }

        void GLStripText::setSize(int s)
        {
            mSize = s;
        }

        void GLStripText::resetColor(const MathCore::vec4f &c)
        {
            for (int i = 0; i < mSize; i++)
            {
                mStripInfo[i].mColor = c;
            }
        }

        int GLStripText::getCharacterCount()
        {
            return mSize / 6;
        }

        void GLStripText::saturateFromBegin(float increment)
        {
            for (int i = 0; i < mSize; i += 6)
            {

                float oldAlpha = mStripInfo[i].mColor.a;

                for (int j = 0; j < 6; j++)
                {
                    mStripInfo[i + j].mColor.a += increment;
                    mStripInfo[i + j].mColor.a = MathCore::OP<float>::clamp(mStripInfo[i].mColor.a, 0.0f, 1.0f);
                }
                // if (distance(mStripInfo[i].mColor,vec4(1)) > 0.01f)
                //   return;
                // if (mStripInfo[i].mColor.a < 1.0f)
                if (oldAlpha != mStripInfo[i].mColor.a)
                    return;
            }
        }
    }
}
