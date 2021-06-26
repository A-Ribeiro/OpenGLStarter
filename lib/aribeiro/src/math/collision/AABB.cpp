#include "AABB.h"
#include "../all_math.h"

namespace aRibeiro {

namespace collision {
    //--------------------------------------------------------------------------
    AABB::AABB(const vec3 &a, const vec3 &b) {
#if defined(ARIBEIRO_SSE2) || defined(ARIBEIRO_NEON)
        min_box = minimum(a, b);
        max_box = maximum(a, b);
#else
        if (a.x < b.x) {
            min_box.x = a.x;
            max_box.x = b.x;
        }
        else {
            max_box.x = a.x;
            min_box.x = b.x;
        }
        if (a.y < b.y) {
            min_box.y = a.y;
            max_box.y = b.y;
        }
        else {
            max_box.y = a.y;
            min_box.y = b.y;
        }
        if (a.z < b.z) {
            min_box.z = a.z;
            max_box.z = b.z;
        }
        else {
            max_box.z = a.z;
            min_box.z = b.z;
        }
#endif
    }
    //--------------------------------------------------------------------------
    AABB::AABB(const vec2 &a, const vec2 &b) {
#if defined(ARIBEIRO_SSE2)
        min_box.array_sse = _mm_min_ps(a.array_sse, b.array_sse);
        max_box.array_sse = _mm_max_ps(a.array_sse, b.array_sse);
#elif defined(ARIBEIRO_NEON)
        min_box.array_neon = vminq_f32(a.array_neon, b.array_neon);
        max_box.array_neon = vmaxq_f32(a.array_neon, b.array_neon);
#else
        if (a.x < b.x) {
            min_box.x = a.x;
            max_box.x = b.x;
        }
        else {
            max_box.x = a.x;
            min_box.x = b.x;
        }
        if (a.y < b.y) {
            min_box.y = a.y;
            max_box.y = b.y;
        }
        else {
            max_box.y = a.y;
            min_box.y = b.y;
        }
#endif
        min_box.z = -1;
        max_box.z = 1;
    }
    //--------------------------------------------------------------------------
    AABB::AABB() {
        //const vec3 __zero = vec3(0);
        min_box = max_box = _vec3_zero_sse;
    }
    //--------------------------------------------------------------------------
    /*
    void AABB::glDrawLines(const AABB &aabb){
        #ifdef __gl_h_
        glBegin(GL_LINES);
          glVertex3f(aabb.min_box.x,aabb.min_box.y,aabb.min_box.z);glVertex3f(aabb.min_box.x,aabb.min_box.y,aabb.max_box.z);
          glVertex3f(aabb.max_box.x,aabb.min_box.y,aabb.min_box.z);glVertex3f(aabb.max_box.x,aabb.min_box.y,aabb.max_box.z);
          glVertex3f(aabb.max_box.x,aabb.max_box.y,aabb.min_box.z);glVertex3f(aabb.max_box.x,aabb.max_box.y,aabb.max_box.z);
          glVertex3f(aabb.min_box.x,aabb.max_box.y,aabb.min_box.z);glVertex3f(aabb.min_box.x,aabb.max_box.y,aabb.max_box.z);

          glVertex3f(aabb.min_box.x,aabb.min_box.y,aabb.min_box.z);glVertex3f(aabb.min_box.x,aabb.max_box.y,aabb.min_box.z);
          glVertex3f(aabb.min_box.x,aabb.min_box.y,aabb.max_box.z);glVertex3f(aabb.min_box.x,aabb.max_box.y,aabb.max_box.z);
          glVertex3f(aabb.max_box.x,aabb.min_box.y,aabb.max_box.z);glVertex3f(aabb.max_box.x,aabb.max_box.y,aabb.max_box.z);
          glVertex3f(aabb.max_box.x,aabb.min_box.y,aabb.min_box.z);glVertex3f(aabb.max_box.x,aabb.max_box.y,aabb.min_box.z);

          glVertex3f(aabb.min_box.x,aabb.min_box.y,aabb.min_box.z);glVertex3f(aabb.max_box.x,aabb.min_box.y,aabb.min_box.z);
          glVertex3f(aabb.min_box.x,aabb.min_box.y,aabb.max_box.z);glVertex3f(aabb.max_box.x,aabb.min_box.y,aabb.max_box.z);
          glVertex3f(aabb.min_box.x,aabb.max_box.y,aabb.max_box.z);glVertex3f(aabb.max_box.x,aabb.max_box.y,aabb.max_box.z);
          glVertex3f(aabb.min_box.x,aabb.max_box.y,aabb.min_box.z);glVertex3f(aabb.max_box.x,aabb.max_box.y,aabb.min_box.z);
        glEnd();
        #endif
    }
    void AABB::glDrawQuads(const AABB &aabb){
        #ifdef __gl_h_
        glBegin(GL_QUADS);
          glVertex3f(aabb.min_box.x,aabb.min_box.y,aabb.min_box.z);glVertex3f(aabb.min_box.x,aabb.max_box.y,aabb.min_box.z);
          glVertex3f(aabb.max_box.x,aabb.max_box.y,aabb.min_box.z);glVertex3f(aabb.max_box.x,aabb.min_box.y,aabb.min_box.z);

          glVertex3f(aabb.max_box.x,aabb.min_box.y,aabb.max_box.z);glVertex3f(aabb.max_box.x,aabb.max_box.y,aabb.max_box.z);
          glVertex3f(aabb.min_box.x,aabb.max_box.y,aabb.max_box.z);glVertex3f(aabb.min_box.x,aabb.min_box.y,aabb.max_box.z);

          glVertex3f(aabb.min_box.x,aabb.min_box.y,aabb.min_box.z);glVertex3f(aabb.max_box.x,aabb.min_box.y,aabb.min_box.z);
          glVertex3f(aabb.max_box.x,aabb.min_box.y,aabb.max_box.z);glVertex3f(aabb.min_box.x,aabb.min_box.y,aabb.max_box.z);

          glVertex3f(aabb.min_box.x,aabb.max_box.y,aabb.min_box.z);glVertex3f(aabb.min_box.x,aabb.max_box.y,aabb.max_box.z);
          glVertex3f(aabb.max_box.x,aabb.max_box.y,aabb.max_box.z);glVertex3f(aabb.max_box.x,aabb.max_box.y,aabb.min_box.z);

          glVertex3f(aabb.max_box.x,aabb.min_box.y,aabb.min_box.z);glVertex3f(aabb.max_box.x,aabb.max_box.y,aabb.min_box.z);
          glVertex3f(aabb.max_box.x,aabb.max_box.y,aabb.max_box.z);glVertex3f(aabb.max_box.x,aabb.min_box.y,aabb.max_box.z);

          glVertex3f(aabb.min_box.x,aabb.min_box.y,aabb.max_box.z);glVertex3f(aabb.min_box.x,aabb.max_box.y,aabb.max_box.z);
          glVertex3f(aabb.min_box.x,aabb.max_box.y,aabb.min_box.z);glVertex3f(aabb.min_box.x,aabb.min_box.y,aabb.min_box.z);
        glEnd();
        #endif
    }
    */
    //--------------------------------------------------------------------------
    bool AABB::pointInsideAABB(const vec2& ptn, const AABB& b) {
        return (ptn.x<b.max_box.x && ptn.x>b.min_box.x &&
            ptn.y<b.max_box.y && ptn.y>b.min_box.y);
    }
    bool AABB::pointInsideAABB(const vec3& ptn, const AABB& b) {
        return (ptn.x<b.max_box.x && ptn.x>b.min_box.x &&
            ptn.y<b.max_box.y && ptn.y>b.min_box.y &&
            ptn.z<b.max_box.z && ptn.z>b.min_box.z);
    }
    bool AABB::aabbOverlapsAABB(const AABB& a, const AABB& b) {
        
        if ( a.max_box.x < b.min_box.x || a.min_box.x > b.max_box.x ) return false;
        if ( a.max_box.y < b.min_box.y || a.min_box.y > b.max_box.y ) return false;
        if ( a.max_box.z < b.min_box.z || a.min_box.z > b.max_box.z ) return false;
        
        return true;
        
        /*
        return (
            (((a.max_box.x >= b.min_box.x) && (a.max_box.x <= b.max_box.x)) ||
            ((b.min_box.x >= a.min_box.x) && (b.min_box.x <= a.max_box.x))) ||
                (((b.max_box.x >= a.min_box.x) && (b.max_box.x <= a.max_box.x)) ||
            ((a.min_box.x >= b.min_box.x) && (a.min_box.x <= b.max_box.x)))
            ) && (
            (((a.max_box.y >= b.min_box.y) && (a.max_box.y <= b.max_box.y)) ||
                ((b.min_box.y >= a.min_box.y) && (b.min_box.y <= a.max_box.y))) ||
                (((b.max_box.y >= a.min_box.y) && (b.max_box.y <= a.max_box.y)) ||
                ((a.min_box.y >= b.min_box.y) && (a.min_box.y <= b.max_box.y)))
                ) && (
                (((a.max_box.z >= b.min_box.z) && (a.max_box.z <= b.max_box.z)) ||
                    ((b.min_box.z >= a.min_box.z) && (b.min_box.z <= a.max_box.z))) ||
                    (((b.max_box.z >= a.min_box.z) && (b.max_box.z <= a.max_box.z)) ||
                    ((a.min_box.z >= b.min_box.z) && (a.min_box.z <= b.max_box.z)))
                    );
        */
    }
    AABB AABB::joinAABB(const AABB& a, const AABB& b) {
        return AABB(minimum(a.min_box, b.min_box), maximum(a.max_box, b.max_box));
    }




    AABB AABB::fromTriangle(const vec3& a, const vec3& b, const vec3& c) {
        vec3 pos_min = minimum(minimum(a, b), c);
        vec3 pos_max = maximum(maximum(a, b), c);
        return AABB(pos_min, pos_max);
    }

    AABB AABB::fromTriangle(const Triangle& triangle) {
        return AABB::fromTriangle(triangle.a, triangle.b, triangle.c);
    }

    AABB AABB::fromSphere(const vec3& pos, const float &radius) {
        vec3 pos_min = pos - vec3(radius, radius, radius);
        vec3 pos_max = pos + vec3(radius, radius, radius);
        return AABB(pos_min, pos_max);
    }

    AABB AABB::fromSphere(const Sphere& sphere) {
        return AABB::fromSphere(sphere.center, sphere.radius);
    }

    AABB AABB::fromLineSegment(const vec3& a, const vec3& b) {
        return AABB(a, b);
    }

    AABB AABB::fromLineSegment(const LineSegment& ls) {
        return AABB(ls.a, ls.b);
    }

    AABB AABB::fromFrustum(const Frustum& frustum) {
        vec3 min = frustum.vertices[0], max = frustum.vertices[1];
        for (int i = 2; i < 8; i++) {
            min = minimum(min, frustum.vertices[i]);
            max = maximum(max, frustum.vertices[i]);
        }
        return AABB(min, max);
    }


    // Intersect ray R(t) = p + t*d against AABB a. When intersecting,
    // return intersection distance tmin and point q of intersection
    bool AABB::raycastAABB(const Ray &r, const AABB& a, float *outTmin, vec3 *outNormal) {

        const float EPSILON = 0.002f;
        int tminIndex = -1;
        float tmin = 0.0f;// set to -FLT_MAX to get first hit on line
        float tmax = FLT_MAX; // set to max distance ray can travel (for segment)
        // For all three slabs
        for (int i = 0; i < 3; i++) {
            if (absv(r.dir[i]) < EPSILON) {
                // Ray is parallel to slab. No hit if origin not within slab
                if (r.origin[i] < a.min_box[i] || r.origin[i] > a.max_box[i])
                    return false;
            }
            else {
                // Compute intersection t value of ray with near and far plane of slab
                float ood = 1.0f / r.dir[i];
                float t1 = (a.min_box[i] - r.origin[i]) * ood;
                float t2 = (a.max_box[i] - r.origin[i]) * ood;
                // Make t1 be intersection with near plane, t2 with far plane
                if (t1 > t2) {
                    //Swap(t1, t2);
                    float taux = t1;
                    t1 = t2;
                    t2 = taux;
                }
                // Compute the intersection of slab intersection intervals
                if (t1 > tmin){
                    tmin = t1;
                    tminIndex = i;
                }
                if (t2 < tmax)
                    tmax = t2;
                // Exit with no collision as soon as slab intersection becomes empty
                if (tmin > tmax)
                    return false;
            }
        }
        // Ray intersects all 3 slabs. Return point (q) and intersection t value (tmin)
        *outTmin = tmin;
        if (tminIndex == 0)
            *outNormal = vec3(-sign(r.dir.x),0.0f,0.0f);
        else if (tminIndex == 1)
            *outNormal = vec3(0.0f,-sign(r.dir.y),0.0f);
        else if (tminIndex == 2)
            *outNormal = vec3(0.0f,0.0f,-sign(r.dir.z));
        //*outQ = p + d * tmin;
        return true;
    }

    // Test if segment specified by points p0 and p1 intersects AABB b
    bool AABB::segmentIntersectsAABB(const vec3& p0, const vec3& p1, const AABB &b) {

        const float EPSILON = 0.002f;

        vec3 c = (b.min_box + b.max_box) * 0.5f; // Box center-point
        vec3 e = b.max_box - c; // Box halflength extents
        vec3 m = (p0 + p1) * 0.5f; // Segment midpoint
        vec3 d = p1 - m; // Segment halflength vector
        m = m - c; // Translate box and segment to origin
        // Try world coordinate axes as separating axes
        float adx = absv(d.x);
        if (absv(m.x) > e.x + adx)
            return false;
        float ady = absv(d.y);
        if (absv(m.y) > e.y + ady)
            return false;
        float adz = absv(d.z);
        if (absv(m.z) > e.z + adz)
            return false;
        // Add in an epsilon term to counteract arithmetic errors when segment is
        // (near) parallel to a coordinate axis (see text for detail)
        adx += EPSILON; ady += EPSILON; adz += EPSILON;
        // Try cross products of segment direction vector with coordinate axes
        if (absv(m.y * d.z - m.z * d.y) > e.y * adz + e.z * ady)
            return false;
        if (absv(m.z * d.x - m.x * d.z) > e.x * adz + e.z * adx)
            return false;
        if (absv(m.x * d.y - m.y * d.x) > e.x * ady + e.y * adx)
            return false;
        // No separating axis found; segment must be overlapping AABB
        return true;
    }

    bool AABB::segmentIntersectsAABB(const LineSegment& ls, const AABB &b) {
        return segmentIntersectsAABB(ls.a, ls.b, b);
    }

    // Given point p, return the point q on or in AABB b that is closest to p
    vec3 AABB::closestPointToAABB(const vec3 &p, const AABB &b)
    {
        vec3 q;
        // For each coordinate axis, if the point coordinate value is
        // outside box, clamp it to the box, else keep it as is
        for (int i = 0; i < 3; i++) {
            float v = p[i];
            if (v < b.min_box[i]) v = b.min_box[i]; // v = max(v, b.min[i])
            if (v > b.max_box[i]) v = b.max_box[i]; // v = min(v, b.max[i])
            q[i] = v;
        }
        return q;
    }


    bool AABB::sphereOverlapsAABB(const vec3 &center, const float &radius, const AABB& aabb, vec3 *penetration) {
        vec3 closestPointInTriangle = closestPointToAABB(center, aabb);

        vec3 SphereToAABB = closestPointInTriangle - center;
        float sqrLength_SphereToAABB = dot(SphereToAABB, SphereToAABB);

        float Max_Radius_sqr = radius * radius;

        if (sqrLength_SphereToAABB > 0.00002f && sqrLength_SphereToAABB < Max_Radius_sqr) {

            float Length_SphereToTriangle = sqrtf(sqrLength_SphereToAABB);
            vec3 SphereToTriangleNorm = SphereToAABB * (1.0f / Length_SphereToTriangle); //normalize(SphereToTriangle);
            //Vector3 triangleNormal = Vectormath::Aos::normalize( Vectormath::Aos::cross( p2-p1 , p3-p1 ) );

            //EPSILON - to avoid process the same triangle again...
            const float EPSILON = 0.002f;
            *penetration = SphereToTriangleNorm * (radius - Length_SphereToTriangle + EPSILON);
            return true;
        }
        return false;

    }

    bool AABB::sphereOverlapsAABB(
        const Sphere &sphere,
        const AABB& aabb,
        vec3 *penetration) {
        return AABB::sphereOverlapsAABB( sphere.center, sphere.radius, aabb, penetration );
    }


    // Test if AABB b intersects plane p
    // p_n = plane normal
    // p_d = plane distance
    bool AABB::planeIntersectsAABB(const Plane &plane, const AABB &b)
    {
        // These two lines not necessary with a (center, extents) AABB representation
        vec3 c = (b.max_box + b.min_box) * 0.5f; // Compute AABB center
        vec3 e = b.max_box - c; // Compute positive extents
        // Compute the projection interval radius of b onto L(t) = b.c + t * p.n
        float r = e[0] * absv(plane.normal[0]) + e[1] * absv(plane.normal[1]) + e[2] * absv(plane.normal[2]);
        // Compute distance of box center from plane
        float s = dot(plane.normal, c) - plane.distance;
        // Intersection occurs when distance s falls within [-r,+r] interval
        return absv(s) <= r;
    }




    static inline void projectOnAxis(const vec3 *points, int count, const vec3 &axis, float *outMin, float *outMax)
    {
        float min = FLT_MAX;// double.PositiveInfinity;
        float max = -FLT_MAX;// double.NegativeInfinity;
        for (int i = 0; i < count; i++)
        {
            const vec3 &p = points[i];
            float val = dot(axis, p);
            if (val < min)
                min = val;
            if (val > max)
                max = val;
        }
        *outMin = min;
        *outMax = max;
    }


    //
    // https://stackoverflow.com/questions/17458562/efficient-aabb-triangle-intersection-in-c-sharp
    //
    bool AABB::triangleIntersectsAABB(const vec3 &v0, const vec3 &v1, const vec3 &v2, const AABB &box)
    {
        const float EPSILON = 1e-6f;

        float triangleMin, triangleMax;
        float boxMin, boxMax;

        vec3 triangle_Vertices[] = {
            v0,v1,v2
        };

        // Test the box normals (x-, y- and z-axes)
        vec3 boxNormals[] = {
            vec3(1,0,0),
            vec3(0,1,0),
            vec3(0,0,1)
        };

        for (int i = 0; i < 3; i++)
        {
            projectOnAxis(triangle_Vertices, 3, boxNormals[i], &triangleMin, &triangleMax);
            if (triangleMax < box.min_box[i] - EPSILON || triangleMin > box.max_box[i] + EPSILON)
                return false; // No intersection possible.
        }

        // Test the triangle normal
        vec3 triangle_Normal = normalize(cross(v1 - v0, v2 - v0));
        float triangleOffset = dot(triangle_Normal, v0);

        vec3 box_Vertices[] = {
            vec3(box.min_box.x,box.min_box.y,box.min_box.z),// 000
            vec3(box.min_box.x,box.min_box.y,box.max_box.z),// 001
            vec3(box.min_box.x,box.max_box.y,box.min_box.z),// 010
            vec3(box.min_box.x,box.max_box.y,box.max_box.z),// 011
            vec3(box.max_box.x,box.min_box.y,box.min_box.z),// 100
            vec3(box.max_box.x,box.min_box.y,box.max_box.z),// 101
            vec3(box.max_box.x,box.max_box.y,box.min_box.z),// 110
            vec3(box.max_box.x,box.max_box.y,box.max_box.z),// 111
        };

        projectOnAxis(box_Vertices, 8, triangle_Normal, &boxMin, &boxMax);

        if (boxMax < triangleOffset - EPSILON || boxMin > triangleOffset + EPSILON)
            return false; // No intersection possible.

        //
        // TODO: test the nine edge cross-products or single edge case
        //
        // Test the single edge
        vec3 triangleEdges[] = {
            v0 - v1,
            v1 - v2,
            v2 - v0
        };
        
        for (int i = 0; i < 3; i++)
        {
            // The box normals are the same as it's edge tangents
            vec3 axis = normalize( cross(triangleEdges[i], triangle_Normal) );
            projectOnAxis(box_Vertices, 8, axis, &boxMin, &boxMax);
            projectOnAxis(triangle_Vertices, 3, axis, &triangleMin, &triangleMax);
            if (boxMax < triangleMin - EPSILON || boxMin > triangleMax + EPSILON)
                return false; // No intersection possible
        }
        
        /*
        // Test the nine edge cross-products
        vec3 triangleEdges[] = {
            v0 - v1,
            v1 - v2,
            v2 - v0
        };

        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 3; j++)
            {
                // The box normals are the same as it's edge tangents
                vec3 axis = cross(triangleEdges[i], boxNormals[j]);
                projectOnAxis(box_Vertices, 8, axis, &boxMin, &boxMax);
                projectOnAxis(triangle_Vertices, 3, axis, &triangleMin, &triangleMax);
                if (boxMax < triangleMin - EPSILON || boxMin > triangleMax + EPSILON)
                    return false; // No intersection possible
            }
        */
        
        // No separating axis found.
        return true;
    }

    bool AABB::triangleIntersectsAABB(const Triangle &t, const AABB &box) {
        return triangleIntersectsAABB(t.a, t.b, t.c, box);
    }

    bool AABB::frustumOverlapsAABB(const Frustum &f, const AABB &aabb) {
        return Frustum::aabbOverlapsFrustum(aabb, f);
    }

}

}

