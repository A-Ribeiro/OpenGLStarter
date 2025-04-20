#pragma once

#include <appkit-gl-base/platform/PlatformGL.h>
#include <appkit-gl-base/font/GLFont2.h>

namespace AppKit
{

    namespace OpenGL
    {

        struct GLFont2Builder_VertexAttrib
        {
            MathCore::vec3f pos;
            MathCore::vec2f uv;
            MathCore::vec4f color;

            GLFont2Builder_VertexAttrib();
            GLFont2Builder_VertexAttrib(
                const MathCore::vec3f &pos,
                const MathCore::vec2f &uv,
                const MathCore::vec4f &color);
        };

        enum GLFont2HorizontalAlign
        {
            GLFont2HorizontalAlign_left = 0,
            GLFont2HorizontalAlign_center,
            GLFont2HorizontalAlign_right,
        };

        enum GLFont2VerticalAlign
        {
            GLFont2VerticalAlign_none = 0,
            GLFont2VerticalAlign_top,
            GLFont2VerticalAlign_middle,
            GLFont2VerticalAlign_bottom,
        };

        /// \brief Construct an OpenGL compatible vertex information about the font to be rendered.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// #include <appkit-gl-base/opengl-wrapper.h>
        /// using namespace aRibeiro;
        /// using namespace AppKit::OpenGL;
        ///
        /// ...
        ///
        /// StringUtil stringUtil;
        ///
        /// GLFont2Builder *fontBuilder_40 = new GLFont2Builder();
        /// fontBuilder_40->load("resources/Roboto-Regular-40.basof2");
        ///
        /// GLFont2VBO fontVBO_40;
        /// GLShaderFont2 shaderFont2;
        ///
        /// ...
        ///
        /// // Draw code
        /// stringUtil.printf("Elapsed:\n%f", time.deltaTime);
        ///
        /// // need to setup current state of font rendering
        /// fontBuilder_40->strokeColor = vec4(0.25, 0.25, 0.25, 1);
        /// fontBuilder_40->horizontalAlign = GLFont2HorizontalAlign_center;
        /// fontBuilder_40->verticalAlign = GLFont2VerticalAlign_middle;
        ///
        /// // build vertex data and upload to the VBO
        /// fontBuilder_40->build(stringUtil.char_ptr());
        /// fontVBO_40.uploadData(fontBuilder_40);
        ///
        /// shaderFont2.enable();
        /// shaderFont2.setTexture(0);
        /// fontBuilder_40->glFont2.texture.active(0);
        ///
        /// // VBO draw method
        /// shaderFont2.setMatrix(projection * GEN<MathCore::mat4f>::translateHomogeneous(0, -100.0f, 0) );
        /// fontVBO_40.draw(GLShaderFont2::vPosition, GLShaderFont2::vUV, GLShaderFont2::vColor);
        ///
        /// // Direct draw method
        /// shaderFont2.setMatrix(projection * GEN<MathCore::mat4f>::translateHomogeneous(0, -200.0f, 0));
        /// GLFont2DirectDraw::draw(fontBuilder_40, GLShaderFont2::vPosition, GLShaderFont2::vUV, GLShaderFont2::vColor);
        ///
        /// GLTexture::deactive(0);
        /// ...
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        ///
        class GLFont2Builder
        {
            // private copy constructores, to avoid copy...
            GLFont2Builder(const GLFont2Builder &v);
            GLFont2Builder& operator=(const GLFont2Builder &v);

            // std::vector<char> char_buffer;
            // std::vector<wchar_t> wchar_buffer;

            // ITK_INLINE
            void GLFont2BitmapRef_to_VertexAttrib(const MathCore::vec3f &pos, const MathCore::vec4f &color, const GLFont2BitmapRef &bitmapRef);

            void countNewLines_1stlineHeight_1stlineLength(const char32_t *str, int size, int *count, float *_1stLineMaxHeight, float *_1stLinelength);

            float computeStringLengthUntilNewLine(const char32_t *str, int size, int offset);

        public:
            GLFont2 glFont2; ///< loaded font glyph set

            std::vector<GLFont2Builder_VertexAttrib> vertexAttrib; ///< last generated OpenGL vertex buffer data

            // state variables
            MathCore::vec4f faceColor;              ///< current state of the face color
            MathCore::vec4f strokeColor;            ///< current state of the stroke color
            bool drawFace;                          ///< should draw face
            bool drawStroke;                        ///< should draw stroke
            MathCore::vec3f strokeOffset;           ///< current state stroke offset
            GLFont2HorizontalAlign horizontalAlign; ///< current state of the horizontal alignment
            GLFont2VerticalAlign verticalAlign;     ///< current state of the vertical alignment

            GLFont2Builder();

            /// \brief Load basof2 font file format
            ///
            /// The basof2 is a custom font format exporter created in this framework.<br />
            /// It means: Binary ASilva OpenGL Font, version 2.0
            ///
            /// This method should be called only once.
            ///
            /// \author Alessandro Ribeiro
            /// \param filename file to load
            ///
            void load(const std::string &filename);

            /// \brief Compute the information about render limits
            ///
            /// \author Alessandro Ribeiro
            /// \param str The string to compute bounds
            ///
            void u32ComputeBox(const char32_t *str, float *xmin, float *xmax, float *ymin, float *ymax);

            /// \brief Compute the information about render limits
            ///
            /// \author Alessandro Ribeiro
            /// \param str The string to compute bounds
            ///
            void computeBox(const char *str, float *xmin, float *xmax, float *ymin, float *ymax);

            /// \brief Build OpenGL vertex information to render the text parameter
            ///
            /// \author Alessandro Ribeiro
            /// \param str The string to be rendered
            /// \return #GLFont2Builder this instance information
            ///
            GLFont2Builder *u32build(const char32_t *str);

            /// \brief Build OpenGL vertex information to render the text parameter
            ///
            /// \author Alessandro Ribeiro
            /// \param str The string to be rendered
            /// \return #GLFont2Builder this instance information
            ///
            GLFont2Builder *build(const char *str);
        };

    }

}