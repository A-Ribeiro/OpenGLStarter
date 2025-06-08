#pragma once

#include "GLFont2.h"
#include <InteractiveToolkit/AlgorithmCore/Polygon/Polygon2D.h>
#include <InteractiveToolkit/AlgorithmCore/Polygon/TriangulatorEarClipping.h>

namespace AppKit
{
    namespace OpenGL
    {
        struct GLFont2TriangulatedGlyph
        {
            MathCore::vec3f advancex;
            std::vector<MathCore::vec3f> vertices;
            std::vector<uint32_t> triangles; ///< Indices to the vertices that form triangles
        };

        /// \brief A cache for polygon glyphs
        ///
        /// This class is used to cache polygon glyphs for a font.
        ///
        /// \author Alessandro Ribeiro
        ///
        class GLFont2PolygonCache
        {
        public:
            std::unordered_map<uint32_t, GLFont2TriangulatedGlyph> triangulated_glyphs;

            GLFont2PolygonCache() = default;
            GLFont2PolygonCache(const GLFont2PolygonCache &other) = default;
            GLFont2PolygonCache(GLFont2PolygonCache &&other) noexcept = default;
            GLFont2PolygonCache &operator=(const GLFont2PolygonCache &other) = default;
            GLFont2PolygonCache &operator=(GLFont2PolygonCache &&other) noexcept = default;

            void setFromGLFont2(const GLFont2 &glFont2, float size, float max_distance_tolerance);

            const GLFont2TriangulatedGlyph *getGlyph(uint32_t charcode) const;
        };

    } // namespace OpenGL

} // namespace AppKit