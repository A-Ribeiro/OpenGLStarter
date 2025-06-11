#pragma once

#include "GLFont2.h"
#include <InteractiveToolkit/AlgorithmCore/Polygon/Polygon2D.h>
#include <InteractiveToolkit/AlgorithmCore/Polygon/TriangulatorEarClipping.h>
#include <InteractiveToolkit/Platform/ThreadPool.h>

namespace AppKit
{
    namespace OpenGL
    {
        struct GLFont2TriangulatedGlyph
        {
            MathCore::vec3f advancex;
            std::vector<MathCore::vec3f> vertices;
            std::vector<uint32_t> triangles; ///< Indices to the vertices that form triangles

            GLFont2TriangulatedGlyph() = default;
            GLFont2TriangulatedGlyph(const GLFont2TriangulatedGlyph &other) = default;
            GLFont2TriangulatedGlyph(GLFont2TriangulatedGlyph &&other) noexcept
            {
                advancex = other.advancex;
                vertices = std::move(other.vertices);
                triangles = std::move(other.triangles);
                // Reset the other object to a default state
                other.advancex = MathCore::vec3f(0.0f);
                other.vertices.clear();
                other.triangles.clear();
            }
            GLFont2TriangulatedGlyph &operator=(const GLFont2TriangulatedGlyph &other) = default;
            GLFont2TriangulatedGlyph &operator=(GLFont2TriangulatedGlyph &&other) noexcept
            {
                advancex = other.advancex;
                vertices = std::move(other.vertices);
                triangles = std::move(other.triangles);
                // Reset the other object to a default state
                other.advancex = MathCore::vec3f(0.0f);
                other.vertices.clear();
                other.triangles.clear();
                return *this;
            }
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
            float size;

            GLFont2PolygonCache() = default;
            GLFont2PolygonCache(const GLFont2PolygonCache &other) = default;
            GLFont2PolygonCache(GLFont2PolygonCache &&other) noexcept
            {
                size = other.size;
                triangulated_glyphs = std::move(other.triangulated_glyphs);
                // Reset the other object to a default state
                other.triangulated_glyphs.clear();
                other.size = 0.0f; // Reset size to a default state
            }
            GLFont2PolygonCache &operator=(const GLFont2PolygonCache &other) = default;
            GLFont2PolygonCache &operator=(GLFont2PolygonCache &&other) noexcept
            {
                size = other.size;
                triangulated_glyphs = std::move(other.triangulated_glyphs);
                // Reset the other object to a default state
                other.triangulated_glyphs.clear();
                other.size = 0.0f; // Reset size to a default state
                return *this;
            }

            void setFromGLFont2(const GLFont2 &glFont2, float size, float max_distance_tolerance, Platform::ThreadPool *threadPool = nullptr);

            const GLFont2TriangulatedGlyph *getGlyph(uint32_t charcode) const;
        };

    } // namespace OpenGL

} // namespace AppKit