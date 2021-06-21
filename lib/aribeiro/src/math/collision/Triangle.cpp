#include "Triangle.h"

#include <aribeiro/all_math.h>

namespace aRibeiro {
namespace collision {
    Triangle::Triangle() {

    }

    Triangle::Triangle(const vec3 &a, const vec3 &b, const vec3 &c) {
        this->a = a;
        this->b = b;
        this->c = c;
    }

    //https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
    bool Triangle::raycastTriangle(const Ray &ray, const vec3 &vertex0, const vec3 &vertex1, const vec3&vertex2, float *outT, vec3 *outNormal) {

        const float EPSILON = 1e-6f;// 0.0000001;
        vec3 edge1, edge2, h, s, q;
        float a, f, u, v;

        edge1 = vertex1 - vertex0;
        edge2 = vertex2 - vertex0;

        h = cross(ray.dir, edge2);
        a = dot(edge1, h);
        if (a > -EPSILON && a < EPSILON)
            return false;    // This ray is parallel to this triangle.
        f = 1.0 / a;
        s = ray.origin - vertex0;
        u = f * dot(s, h);
        if (u < 0.0 || u > 1.0)
            return false;
        q = cross(s, edge1);
        v = f * dot(ray.dir, q);
        if (v < 0.0 || u + v > 1.0)
            return false;
        // At this stage we can compute t to find out where the intersection point is on the line.
        float t = f * dot(edge2, q);
        if (t > EPSILON) // ray intersection
        {
            *outT = t;
            *outNormal = normalize(cross(edge1,edge2));
            //outIntersectionPoint = r_origin + r_dir * t;
            return true;
        }
        else // This means that there is a line intersection but not a ray intersection.
            return false;
    }

    bool Triangle::raycastTriangle(const Ray &ray, const Triangle &t, float *outT, vec3 *outNormal) {
        return raycastTriangle(ray, t.a, t.b, t.c, outT, outNormal);
    }

    vec3 Triangle::closestPointToTriangle(const vec3& p, const vec3& a, const vec3& b, const vec3& c) {
        // Check if P in vertex region outside A
        vec3 ab = b - a;
        vec3 ac = c - a;
        vec3 ap = p - a;
        float d1 = dot(ab, ap);
        float d2 = dot(ac, ap);
        if (d1 <= 0.0f && d2 <= 0.0f) return a; // barycentric coordinates (1,0,0)

        // Check if P in vertex region outside B
        vec3 bp = p - b;
        float d3 = dot(ab, bp);
        float d4 = dot(ac, bp);
        if (d3 >= 0.0f && d4 <= d3) return b; // barycentric coordinates (0,1,0)

        // Check if P in edge region of AB, if so return projection of P onto AB
        float vc = d1 * d4 - d3 * d2;
        if (vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f) {
            float v = d1 / (d1 - d3);
            return a + v * ab; // barycentric coordinates (1-v,v,0)
        }

        // Check if P in vertex region outside C
        vec3 cp = p - c;
        float d5 = dot(ab, cp);
        float d6 = dot(ac, cp);
        if (d6 >= 0.0f && d5 <= d6) return c; // barycentric coordinates (0,0,1)

        // Check if P in edge region of AC, if so return projection of P onto AC
        float vb = d5 * d2 - d1 * d6;
        if (vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f) {
            float w = d2 / (d2 - d6); return a + w * ac; // barycentric coordinates (1-w,0,w)
        }

        // Check if P in edge region of BC, if so return projection of P onto BC
        float va = d3 * d6 - d5 * d4;
        if (va <= 0.0f && (d4 - d3) >= 0.0f && (d5 - d6) >= 0.0f) {
            float w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
            return b + w * (c - b); // barycentric coordinates (0,1-w,w)
        }

        // P inside face region. Compute Q through its barycentric coordinates (u,v,w)
        float denom = 1.0f / (va + vb + vc);
        float v = vb * denom;
        float w = vc * denom;
        return a + ab * v + ac * w; //=u*a+v*b+w*c,u=va*denom=1.0f-v-w
    }

    vec3 Triangle::closestPointToTriangle(const vec3& p, const Triangle &t) {
        return closestPointToTriangle(p, t.a, t.b, t.c);
    }


    //Given line pq and ccw triangle abc, return whether line pierces triangle. If
    //so, also return the barycentric coordinates (u,v,w) of the intersection point
    bool Triangle::segmentIntersectsTriangle(const vec3& p, const vec3& q,
        const vec3& a, const vec3& b, const vec3& c,
        float *_u, float *_v, float *_w) {
        float u, v, w;
        vec3 pq = q - p;
        vec3 pa = a - p;
        vec3 pb = b - p;
        vec3 pc = c - p;
        // Test if pq is inside the edges bc, ca and ab. Done by testing
        // that the signed tetrahedral volumes, computed using scalar triple
        // products, are all positive

        vec3 m = cross(pq, pc);
        u = dot(pb, m); // ScalarTriple(pq, pc, pb);
        if (u < 0.0f)
            return false;
        v = -dot(pa, m); // ScalarTriple(pq, pa, pc);
        if (v < 0.0f)
            return false;
        m = cross(pq, pb);
        w = -dot(pa, m); // ScalarTriple(pq, pb, pa);
        if (w < 0.0f)
            return false;

        // Compute the barycentric coordinates (u, v, w) determining the
        // intersection point r, r = u*a + v*b + w*c
        float denom = 1.0f / (u + v + w);
        u *= denom;
        v *= denom;
        w *= denom; // w = 1.0f - u - v;

        *_u = u;
        *_v = v;
        *_w = w;
        return true;
    }

    bool Triangle::segmentIntersectsTriangle(const LineSegment& ls,
        const vec3& a, const vec3& b, const vec3& c,
        float *u, float *v, float *w) {
        return segmentIntersectsTriangle(ls.a, ls.b,
            a, b, c,
            u, v, w);
    }

    bool Triangle::segmentIntersectsTriangle(const LineSegment& ls,
        const Triangle& t,
        float *u, float *v, float *w) {
        return segmentIntersectsTriangle(ls.a, ls.b,
            t.a, t.b, t.c,
            u, v, w);
    }

    bool Triangle::segmentIntersectsTriangle(const vec3& p, const vec3& q,
        const Triangle& t,
        float *u, float *v, float *w) {
        return segmentIntersectsTriangle(p, q,
            t.a, t.b, t.c,
            u, v, w);
    }

    bool Triangle::sphereIntersectsTriangle(const vec3& center, const float &radius, const vec3& a, const vec3& b, const vec3& c, vec3 *penetration) {
        vec3 closestPointInTriangle = closestPointToTriangle(center, a, b, c);

        vec3 SphereToTriangle = closestPointInTriangle - center;
        float sqrLength_SphereToTriangle = dot(SphereToTriangle, SphereToTriangle);

        float Max_Radius_sqr = radius * radius;

        if (sqrLength_SphereToTriangle > 0.00002f && sqrLength_SphereToTriangle < Max_Radius_sqr) {

            float Length_SphereToTriangle = sqrtf(sqrLength_SphereToTriangle);
            vec3 SphereToTriangleNorm = SphereToTriangle * (1.0f / Length_SphereToTriangle); //normalize(SphereToTriangle);
            //Vector3 triangleNormal = Vectormath::Aos::normalize( Vectormath::Aos::cross( p2-p1 , p3-p1 ) );

            //EPSILON - to avoid process the same triangle again...
            const float EPSILON = 0.002f;
            *penetration = SphereToTriangleNorm * (radius - Length_SphereToTriangle + EPSILON);
            return true;
        }
        return false;
    }

    bool Triangle::sphereIntersectsTriangle(
        const Sphere& sphere,
        const vec3& a, const vec3& b, const vec3& c,
        vec3 *penetration) {
        return Triangle::sphereIntersectsTriangle(sphere.center, sphere.radius,a, b, c,penetration);
    }

    bool Triangle::sphereIntersectsTriangle(const vec3& center, const float &radius, const Triangle& t, vec3 *penetration) {
        return Triangle::sphereIntersectsTriangle(center, radius, t.a, t.b, t.c, penetration);
    }

    bool Triangle::sphereIntersectsTriangle(const Sphere& sphere, const Triangle& t, vec3 *penetration) {
        return Triangle::sphereIntersectsTriangle(sphere.center, sphere.radius, t.a, t.b, t.c, penetration);
    }

    bool Triangle::aabbIntersectsTriangle(const AABB &box, const vec3 &v0, const vec3 &v1, const vec3 &v2) {
        return AABB::triangleIntersectsAABB(v0, v1, v2, box);
    }

    bool Triangle::aabbIntersectsTriangle(const AABB &box, const Triangle &triangle) {
        return AABB::triangleIntersectsAABB(triangle.a, triangle.b, triangle.c, box);
    }

}
}
