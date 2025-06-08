
#include <appkit-gl-engine/Components/deprecated/ComponentColorMesh.h>
#include <InteractiveToolkit/MathCore/MathCore.h>

namespace AppKit
{
    namespace GLEngine
    {

        namespace Components
        {

            const ComponentType ComponentColorMesh::Type = "ComponentColorMesh";

            void setTriangle(std::vector<uint32_t> *indices,
                             std::vector<MathCore::vec3f> *vertices,
                             std::vector<MathCore::vec3f> *uv,
                             std::vector<MathCore::vec3f> *normals,
                             std::vector<MathCore::vec3f> *tangents,
                             std::vector<MathCore::vec3f> *binormals)
            {
                vertices->clear();
                vertices->push_back(MathCore::vec3f(-0.5f, 0.0f, 0.0f));
                vertices->push_back(MathCore::vec3f(0.5f, 0.0f, 0.0f));
                vertices->push_back(MathCore::vec3f(0.0f, 1.0f, 0.0f));

                if (uv != nullptr)
                {
                    uv->clear();
                    uv->push_back(MathCore::vec3f(MathCore::vec2f(0.0f, 0.0f), 0));
                    uv->push_back(MathCore::vec3f(MathCore::vec2f(1.0f, 0.0f), 0));
                    uv->push_back(MathCore::vec3f(MathCore::vec2f(0.5f, 0.5f), 0));
                }

                if (normals != nullptr)
                {
                    normals->clear();
                    normals->push_back(MathCore::vec3f(0.0f, 0.0f, 1.0f));
                    normals->push_back(MathCore::vec3f(0.0f, 0.0f, 1.0f));
                    normals->push_back(MathCore::vec3f(0.0f, 0.0f, 1.0f));
                }

                if (tangents != nullptr)
                {
                    tangents->clear();
                    tangents->push_back(MathCore::vec3f(1.0f, 0.0f, 0.0f));
                    tangents->push_back(MathCore::vec3f(1.0f, 0.0f, 0.0f));
                    tangents->push_back(MathCore::vec3f(1.0f, 0.0f, 0.0f));
                }

                if (binormals != nullptr)
                {
                    binormals->clear();
                    binormals->push_back(MathCore::vec3f(0.0f, 1.0f, 0.0f));
                    binormals->push_back(MathCore::vec3f(0.0f, 1.0f, 0.0f));
                    binormals->push_back(MathCore::vec3f(0.0f, 1.0f, 0.0f));
                }

                indices->clear();
                for (int i = 0; i < vertices->size(); i += 3)
                {
                    indices->push_back(i);
                    indices->push_back(i + 2);
                    indices->push_back(i + 1);
                }
            }

            void setPlane(bool _xy,
                          bool _xz,
                          float _width, float _height,
                          std::vector<uint32_t> *indices,
                          std::vector<MathCore::vec3f> *vertices,
                          std::vector<MathCore::vec3f> *uv,
                          std::vector<MathCore::vec3f> *normals,
                          std::vector<MathCore::vec3f> *tangents,
                          std::vector<MathCore::vec3f> *binormals)
            {

                MathCore::vec3f dimension = MathCore::vec3f(_width, 0, _height);
                MathCore::vec3f halfDim = dimension * 0.5f;

                if (_xy)
                {
                    vertices->clear();
                    vertices->push_back(MathCore::vec3f(halfDim.x, halfDim.z, 0.0f));
                    vertices->push_back(MathCore::vec3f(halfDim.x, -halfDim.z, 0.0f));
                    vertices->push_back(MathCore::vec3f(-halfDim.x, -halfDim.z, 0.0f));
                    vertices->push_back(MathCore::vec3f(-halfDim.x, halfDim.z, 0.0f));
                }

                if (_xz)
                {
                    vertices->clear();
                    vertices->push_back(MathCore::vec3f(halfDim.x, 0, halfDim.z));
                    vertices->push_back(MathCore::vec3f(halfDim.x, 0, -halfDim.z));
                    vertices->push_back(MathCore::vec3f(-halfDim.x, 0, -halfDim.z));
                    vertices->push_back(MathCore::vec3f(-halfDim.x, 0, halfDim.z));
                }

                indices->clear();
                for (int i = 0; i < vertices->size(); i += 4)
                {
                    indices->push_back(i);
                    indices->push_back(i + 2);
                    indices->push_back(i + 1);

                    indices->push_back(i);
                    indices->push_back(i + 3);
                    indices->push_back(i + 2);
                }

                if (uv != nullptr)
                {
                    uv->clear();
                    uv->push_back(MathCore::vec3f(MathCore::vec2f(1.0f, 0.0f), 0));
                    uv->push_back(MathCore::vec3f(MathCore::vec2f(1.0f, 1.0f), 0));
                    uv->push_back(MathCore::vec3f(MathCore::vec2f(0.0f, 1.0f), 0));
                    uv->push_back(MathCore::vec3f(MathCore::vec2f(0.0f, 0.0f), 0));
                }

                if (normals != nullptr)
                {
                    normals->clear();
                    normals->push_back(MathCore::vec3f(0.0f, 1.0f, 0.0f));
                    normals->push_back(MathCore::vec3f(0.0f, 1.0f, 0.0f));
                    normals->push_back(MathCore::vec3f(0.0f, 1.0f, 0.0f));
                }

                if (tangents != nullptr)
                {
                    tangents->clear();
                    tangents->push_back(MathCore::vec3f(1.0f, 0.0f, 0.0f));
                    tangents->push_back(MathCore::vec3f(1.0f, 0.0f, 0.0f));
                    tangents->push_back(MathCore::vec3f(1.0f, 0.0f, 0.0f));
                }

                if (binormals != nullptr)
                {
                    binormals->clear();
                    binormals->push_back(MathCore::vec3f(0.0f, 0.0f, 1.0f));
                    binormals->push_back(MathCore::vec3f(0.0f, 0.0f, 1.0f));
                    binormals->push_back(MathCore::vec3f(0.0f, 0.0f, 1.0f));
                }
            }

            void setBox(const MathCore::vec3f &dimension,
                        std::vector<uint32_t> *indices,
                        std::vector<MathCore::vec3f> *vertices,
                        std::vector<MathCore::vec3f> *uv,
                        std::vector<MathCore::vec3f> *normals,
                        std::vector<MathCore::vec3f> *tangents,
                        std::vector<MathCore::vec3f> *binormals)
            {
                MathCore::vec3f halfDim = dimension * 0.5f;
                //
                // TOP
                //
                vertices->clear();
                vertices->push_back(MathCore::vec3f(halfDim.x, halfDim.y, halfDim.z));
                vertices->push_back(MathCore::vec3f(halfDim.x, halfDim.y, -halfDim.z));
                vertices->push_back(MathCore::vec3f(-halfDim.x, halfDim.y, -halfDim.z));
                vertices->push_back(MathCore::vec3f(-halfDim.x, halfDim.y, halfDim.z));
                //
                // BOTTOM
                //
                vertices->push_back(MathCore::vec3f(halfDim.x, -halfDim.y, halfDim.z));
                vertices->push_back(MathCore::vec3f(-halfDim.x, -halfDim.y, halfDim.z));
                vertices->push_back(MathCore::vec3f(-halfDim.x, -halfDim.y, -halfDim.z));
                vertices->push_back(MathCore::vec3f(halfDim.x, -halfDim.y, -halfDim.z));
                //
                // RIGHT
                //
                vertices->push_back(MathCore::vec3f(halfDim.x, halfDim.y, halfDim.z));
                vertices->push_back(MathCore::vec3f(halfDim.x, -halfDim.y, halfDim.z));
                vertices->push_back(MathCore::vec3f(halfDim.x, -halfDim.y, -halfDim.z));
                vertices->push_back(MathCore::vec3f(halfDim.x, halfDim.y, -halfDim.z));
                //
                // LEFT
                //
                vertices->push_back(MathCore::vec3f(-halfDim.x, halfDim.y, halfDim.z));
                vertices->push_back(MathCore::vec3f(-halfDim.x, halfDim.y, -halfDim.z));
                vertices->push_back(MathCore::vec3f(-halfDim.x, -halfDim.y, -halfDim.z));
                vertices->push_back(MathCore::vec3f(-halfDim.x, -halfDim.y, halfDim.z));
                //
                // FRONT
                //
                vertices->push_back(MathCore::vec3f(-halfDim.x, halfDim.y, halfDim.z));
                vertices->push_back(MathCore::vec3f(-halfDim.x, -halfDim.y, halfDim.z));
                vertices->push_back(MathCore::vec3f(halfDim.x, -halfDim.y, halfDim.z));
                vertices->push_back(MathCore::vec3f(halfDim.x, halfDim.y, halfDim.z));
                //
                // BACK
                //
                vertices->push_back(MathCore::vec3f(-halfDim.x, halfDim.y, -halfDim.z));
                vertices->push_back(MathCore::vec3f(halfDim.x, halfDim.y, -halfDim.z));
                vertices->push_back(MathCore::vec3f(halfDim.x, -halfDim.y, -halfDim.z));
                vertices->push_back(MathCore::vec3f(-halfDim.x, -halfDim.y, -halfDim.z));

                if (normals != nullptr)
                {
                    //
                    // TOP
                    //
                    normals->clear();
                    normals->push_back(MathCore::vec3f(0, 1, 0));
                    normals->push_back(MathCore::vec3f(0, 1, 0));
                    normals->push_back(MathCore::vec3f(0, 1, 0));
                    normals->push_back(MathCore::vec3f(0, 1, 0));
                    //
                    // BOTTOM
                    //
                    normals->push_back(MathCore::vec3f(0, -1, 0));
                    normals->push_back(MathCore::vec3f(0, -1, 0));
                    normals->push_back(MathCore::vec3f(0, -1, 0));
                    normals->push_back(MathCore::vec3f(0, -1, 0));
                    //
                    // RIGHT
                    //
                    normals->push_back(MathCore::vec3f(1, 0, 0));
                    normals->push_back(MathCore::vec3f(1, 0, 0));
                    normals->push_back(MathCore::vec3f(1, 0, 0));
                    normals->push_back(MathCore::vec3f(1, 0, 0));
                    //
                    // LEFT
                    //
                    normals->push_back(MathCore::vec3f(-1, 0, 0));
                    normals->push_back(MathCore::vec3f(-1, 0, 0));
                    normals->push_back(MathCore::vec3f(-1, 0, 0));
                    normals->push_back(MathCore::vec3f(-1, 0, 0));
                    //
                    // FRONT
                    //
                    normals->push_back(MathCore::vec3f(0, 0, 1));
                    normals->push_back(MathCore::vec3f(0, 0, 1));
                    normals->push_back(MathCore::vec3f(0, 0, 1));
                    normals->push_back(MathCore::vec3f(0, 0, 1));
                    //
                    // BACK
                    //
                    normals->push_back(MathCore::vec3f(0, 0, -1));
                    normals->push_back(MathCore::vec3f(0, 0, -1));
                    normals->push_back(MathCore::vec3f(0, 0, -1));
                    normals->push_back(MathCore::vec3f(0, 0, -1));
                }

                if (uv != nullptr)
                {
                    //
                    // TOP
                    //
                    uv->clear();

                    uv->push_back(MathCore::vec3f(0, 1, 0));
                    uv->push_back(MathCore::vec3f(1, 1, 0));
                    uv->push_back(MathCore::vec3f(1, 0, 0));
                    uv->push_back(MathCore::vec3f(0, 0, 0));
                    //
                    // BOTTOM
                    //
                    uv->push_back(MathCore::vec3f(0, 1, 0));
                    uv->push_back(MathCore::vec3f(1, 1, 0));
                    uv->push_back(MathCore::vec3f(1, 0, 0));
                    uv->push_back(MathCore::vec3f(0, 0, 0));
                    //
                    // RIGHT
                    //
                    uv->push_back(MathCore::vec3f(0, 1, 0));
                    uv->push_back(MathCore::vec3f(1, 1, 0));
                    uv->push_back(MathCore::vec3f(1, 0, 0));
                    uv->push_back(MathCore::vec3f(0, 0, 0));
                    //
                    // LEFT
                    //
                    uv->push_back(MathCore::vec3f(0, 1, 0));
                    uv->push_back(MathCore::vec3f(1, 1, 0));
                    uv->push_back(MathCore::vec3f(1, 0, 0));
                    uv->push_back(MathCore::vec3f(0, 0, 0));
                    //
                    // FRONT
                    //
                    uv->push_back(MathCore::vec3f(0, 1, 0));
                    uv->push_back(MathCore::vec3f(1, 1, 0));
                    uv->push_back(MathCore::vec3f(1, 0, 0));
                    uv->push_back(MathCore::vec3f(0, 0, 0));
                    //
                    // BACK
                    //
                    uv->push_back(MathCore::vec3f(0, 1, 0));
                    uv->push_back(MathCore::vec3f(1, 1, 0));
                    uv->push_back(MathCore::vec3f(1, 0, 0));
                    uv->push_back(MathCore::vec3f(0, 0, 0));
                }

                if (tangents != nullptr)
                {
                    //
                    // TOP
                    //

                    tangents->clear();

                    tangents->push_back(MathCore::vec3f(-1, 0, 0));
                    tangents->push_back(MathCore::vec3f(-1, 0, 0));
                    tangents->push_back(MathCore::vec3f(-1, 0, 0));
                    tangents->push_back(MathCore::vec3f(-1, 0, 0));

                    //
                    // BOTTOM
                    //
                    tangents->push_back(MathCore::vec3f(1, 0, 0));
                    tangents->push_back(MathCore::vec3f(1, 0, 0));
                    tangents->push_back(MathCore::vec3f(1, 0, 0));
                    tangents->push_back(MathCore::vec3f(1, 0, 0));

                    //
                    // RIGHT
                    //
                    tangents->push_back(MathCore::vec3f(0, 0, 1));
                    tangents->push_back(MathCore::vec3f(0, 0, 1));
                    tangents->push_back(MathCore::vec3f(0, 0, 1));
                    tangents->push_back(MathCore::vec3f(0, 0, 1));
                    //
                    // LEFT
                    //
                    tangents->push_back(MathCore::vec3f(0, 0, -1));
                    tangents->push_back(MathCore::vec3f(0, 0, -1));
                    tangents->push_back(MathCore::vec3f(0, 0, -1));
                    tangents->push_back(MathCore::vec3f(0, 0, -1));
                    //
                    // FRONT
                    //
                    tangents->push_back(MathCore::vec3f(-1, 0, 0));
                    tangents->push_back(MathCore::vec3f(-1, 0, 0));
                    tangents->push_back(MathCore::vec3f(-1, 0, 0));
                    tangents->push_back(MathCore::vec3f(-1, 0, 0));
                    //
                    // BACK
                    //
                    tangents->push_back(MathCore::vec3f(1, 0, 0));
                    tangents->push_back(MathCore::vec3f(1, 0, 0));
                    tangents->push_back(MathCore::vec3f(1, 0, 0));
                    tangents->push_back(MathCore::vec3f(1, 0, 0));
                }

                //
                // Construct Index
                //
                indices->clear();
                for (int i = 0; i < vertices->size(); i += 4)
                {
                    indices->push_back(i);
                    indices->push_back(i + 2);
                    indices->push_back(i + 1);

                    indices->push_back(i);
                    indices->push_back(i + 3);
                    indices->push_back(i + 2);
                }
            }

            void setSphere(float radius, int sectorCount, int stackCount,
                           std::vector<uint32_t> *indices,
                           std::vector<MathCore::vec3f> *vertices,
                           std::vector<MathCore::vec3f> *uv,
                           std::vector<MathCore::vec3f> *normals,
                           std::vector<MathCore::vec3f> *tangents,
                           std::vector<MathCore::vec3f> *binormals)
            {

                // clear allbuffers
                indices->clear();
                vertices->clear();
                if (uv != nullptr)
                    uv->clear();
                if (normals != nullptr)
                    normals->clear();
                if (tangents != nullptr)
                    tangents->clear();
                if (binormals != nullptr)
                    binormals->clear();

                float x, y, z, xy;                           // vertex position
                float nx, ny, nz, lengthInv = 1.0f / radius; // vertex normal
                float s, t;                                  // vertex texCoord

                float sectorStep = 2 * MathCore::CONSTANT<float>::PI / sectorCount;
                float stackStep = MathCore::CONSTANT<float>::PI / stackCount;
                float sectorAngle, stackAngle;

                for (int i = 0; i <= stackCount; ++i)
                {
                    stackAngle = MathCore::CONSTANT<float>::PI / 2 - i * stackStep; // starting from pi/2 to -pi/2
                    xy = radius * cosf(stackAngle);      // r * cos(u)
                    z = radius * sinf(stackAngle);       // r * sin(u)

                    // add (sectorCount+1) vertices per stack
                    // the first and last vertices have same position and normal, but different tex coords
                    for (int j = 0; j <= sectorCount; ++j)
                    {
                        sectorAngle = j * sectorStep; // starting from 0 to 2pi

                        // vertex position (x, y, z)
                        x = xy * cosf(sectorAngle); // r * cos(u) * cos(v)
                        y = xy * sinf(sectorAngle); // r * cos(u) * sin(v)

                        // normalized vertex normal (nx, ny, nz)
                        nx = x * lengthInv;
                        ny = y * lengthInv;
                        nz = z * lengthInv;

                        // vertex tex coord (s, t) range between [0, 1]
                        s = (float)j / sectorCount;
                        t = (float)i / stackCount;

                        MathCore::vec3f p = MathCore::vec3f(x, z, -y);
                        MathCore::vec3f normal = MathCore::vec3f(nx, nz, -ny);
                        MathCore::vec3f tangent = MathCore::OP<MathCore::vec3f>::normalize(MathCore::OP<MathCore::vec3f>::cross(MathCore::vec3f(0, 1, 0), normal));
                        MathCore::vec3f binormal = MathCore::OP<MathCore::vec3f>::normalize(MathCore::OP<MathCore::vec3f>::cross(normal, tangent));

                        /*
                        GeneralVertexAttrib va(
                        p,// position;
                        MathCore::vec2f(s, t),// uv;
                        normal,// normal;
                        tangent,// tangent;
                        binormal//MathCore::vec3f(0),// binormal;
                        );

                        result->vertices.push_back(va);
                        */

                        vertices->push_back(p);
                        if (uv != nullptr)
                            uv->push_back(MathCore::vec3f(MathCore::vec2f(s, t), 0));
                        if (normals != nullptr)
                            normals->push_back(normal);
                        if (tangents != nullptr)
                            tangents->push_back(tangent);
                        if (binormals != nullptr)
                            binormals->push_back(binormal);
                    }
                }

                int k1, k2;
                int i = 0;

                //
                // Fix tangent and binormal from poles
                //
                k1 = i * (sectorCount + 1); // beginning of current stack
                k2 = k1 + sectorCount + 1;  // beginning of next stack

                for (int j = 0; j < sectorCount; ++j, ++k1, ++k2)
                {
                    // k1+1 => k2 => k2+1
                    int index1 = k1 + 1;
                    int index2 = k2;
                    int index3 = k2 + 1;

                    if (tangents != nullptr)
                        (*tangents)[index1] = MathCore::OP<MathCore::vec3f>::normalize((*tangents)[index2] + (*tangents)[index3]);
                    if (binormals != nullptr && normals != nullptr && tangents != nullptr)
                        (*binormals)[index1] = MathCore::OP<MathCore::vec3f>::normalize(MathCore::OP<MathCore::vec3f>::cross((*normals)[index1], (*tangents)[index1]));
                    /*
                    result->vertices[index1].tangent = normalize(result->vertices[index2].tangent + result->vertices[index3].tangent);
                    result->vertices[index1].binormal = normalize(cross(result->vertices[index1].normal, result->vertices[index1].tangent));
                    */
                }

                i = (stackCount - 1);
                k1 = i * (sectorCount + 1); // beginning of current stack
                k2 = k1 + sectorCount + 1;  // beginning of next stack

                for (int j = 0; j < sectorCount; ++j, ++k1, ++k2)
                {
                    // k1+1 => k2 => k2+1
                    int index1 = k1;
                    int index2 = k2;
                    int index3 = k1 + 1;

                    if (tangents != nullptr)
                        (*tangents)[index2] = MathCore::OP<MathCore::vec3f>::normalize((*tangents)[index1] + (*tangents)[index3]);
                    if (binormals != nullptr && normals != nullptr && tangents != nullptr)
                        (*binormals)[index2] = MathCore::OP<MathCore::vec3f>::normalize(MathCore::OP<MathCore::vec3f>::cross((*normals)[index2], (*tangents)[index2]));

                    /*
                    result->vertices[index2].tangent = normalize(result->vertices[index1].tangent + result->vertices[index3].tangent);
                    result->vertices[index2].binormal = normalize(cross(result->vertices[index2].normal, result->vertices[index2].tangent));
                    */
                }

                for (i = 0; i < stackCount; ++i)
                {
                    k1 = i * (sectorCount + 1); // beginning of current stack
                    k2 = k1 + sectorCount + 1;  // beginning of next stack

                    for (int j = 0; j < sectorCount; ++j, ++k1, ++k2)
                    {
                        // 2 triangles per sector excluding first and last stacks
                        // k1 => k2 => k1+1
                        if (i != 0)
                        {
                            indices->push_back(k1);
                            indices->push_back(k1 + 1);
                            indices->push_back(k2);
                        }

                        // k1+1 => k2 => k2+1
                        if (i != (stackCount - 1))
                        {
                            indices->push_back(k1 + 1);
                            indices->push_back(k2 + 1);
                            indices->push_back(k2);
                        }
                    }
                }
            }

        }
    }
}
