#pragma once

#include <appkit-gl-base/platform/PlatformGL.h>
#include <appkit-gl-base/font/GLFont2.h>
#include <appkit-gl-base/font/GLFont2PolygonCache.h>

#include <InteractiveToolkit/CollisionCore/CollisionCore.h>

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
            GLFont2VerticalAlign_baseline = 0,
            GLFont2VerticalAlign_top,
            GLFont2VerticalAlign_middle,
            GLFont2VerticalAlign_bottom,
        };

        enum GLFont2WrapMode
        {
            GLFont2WrapMode_NoWrap = 0,
            GLFont2WrapMode_Character,
            GLFont2WrapMode_Word,
        };

        enum GLFont2FirstLineHeightMode
        {
            GLFont2FirstLineHeightMode_UseLineHeight = 0,
            GLFont2FirstLineHeightMode_UseCharacterMaxHeight,
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

            // ITK_INLINE
            void GLFont2BitmapRef_to_VertexAttrib(const MathCore::vec3f &pos, const MathCore::vec4f &color, const GLFont2BitmapRef &bitmapRef, float scale = 1.0f);

        public:
            // deleted copy constructor and assign operator, to avoid copy...
            GLFont2Builder(const GLFont2Builder &v) = delete;
            GLFont2Builder &operator=(const GLFont2Builder &v) = delete;

            GLFont2 glFont2; ///< loaded font glyph set

            std::vector<GLFont2Builder_VertexAttrib> vertexAttrib; ///< last generated OpenGL vertex buffer data
            std::vector<uint32_t> triangles; ///< indices used when generates from polygonCache

            // state variables
            MathCore::vec4f faceColor;              ///< current state of the face color
            MathCore::vec4f strokeColor;            ///< current state of the stroke color
            bool drawFace;                          ///< should draw face
            bool drawStroke;                        ///< should draw stroke
            MathCore::vec3f strokeOffset;           ///< current state stroke offset
            GLFont2HorizontalAlign horizontalAlign; ///< current state of the horizontal alignment
            GLFont2VerticalAlign verticalAlign;     ///< current state of the vertical alignment
            float lineHeight;                       ///< current state of the line height
            float size;                             ///< current state of the font size
            GLFont2WrapMode wrapMode;
            GLFont2FirstLineHeightMode firstLineHeightMode;
            char32_t wordSeparatorChar;

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
            /// \param force_srgb use sRGB as input
            ///
            void load(const std::string &filename, bool force_srgb);

            /// \brief Compute the information about render limits
            ///
            /// \author Alessandro Ribeiro
            /// \param str The string to compute bounds
            ///
            CollisionCore::AABB<MathCore::vec3f> u32RichComputeBox(const char32_t *str, float max_width = -1);

            /// \brief Compute the information about render limits
            ///
            /// \author Alessandro Ribeiro
            /// \param str The string to compute bounds
            ///
            CollisionCore::AABB<MathCore::vec3f> richComputeBox(const char *str, float max_width = -1);

            /// \brief Build OpenGL vertex information to render the text parameter
            ///
            /// \author Alessandro Ribeiro
            /// \param utf32_str The string to be rendered
            /// \param use_srgb Use sRGB color space for the face and stroke colors
            /// \param max_width The maximum width of the text to be rendered, if -1 it will not limit the width
            /// \param useThisPolygons Use this polygon cache instead of rendering texture based glyphs
            /// \return #GLFont2Builder this instance information
            ///
            GLFont2Builder *u32richBuild(const char32_t *utf32_str, bool use_srgb, float max_width = -1, std::shared_ptr<GLFont2PolygonCache> useThisPolygons = nullptr);

            /// \brief Build OpenGL vertex information to render the text parameter
            ///
            /// \author Alessandro Ribeiro
            /// \param utf8_str The string to be rendered
            /// \param use_srgb Use sRGB color space for the face and stroke colors
            /// \param max_width The maximum width of the text to be rendered, if -1 it will not limit the width
            /// \param useThisPolygons Use this polygon cache instead of rendering texture based glyphs
            /// \return #GLFont2Builder this instance information
            ///
            GLFont2Builder *richBuild(const char *utf8_str, bool use_srgb, float max_width = -1, std::shared_ptr<GLFont2PolygonCache> useThisPolygons = nullptr);

            /// \brief Build triangles vertex information to render the glyphs
            ///
            /// \author Alessandro Ribeiro
            /// \param size The size of the font to be rendered
            /// \param max_distance_tolerance The maximum distance tolerance for the triangulation
            /// \return #GLFont2PolygonCache with the triangulated glyphs
            ///
            std::shared_ptr<GLFont2PolygonCache> createPolygonCache(float size, float max_distance_tolerance, Platform::ThreadPool *threadPool = nullptr) const;

            int u32richCountLines(const char32_t *utf32_str, float max_width = -1);
            int richCountLines(const char *utf8_str, float max_width = -1);

            bool isConstructedFromPolygonCache() const;
        };

    }

}