#include "Plane.h"

#include <aribeiro/all_math.h>


namespace aRibeiro {
namespace collision {
    Plane::Plane() {
        normal = vec3(0, 1, 0);
        distance = 0.0f;
    }

    Plane::Plane(const vec3 &point, const vec3 &normal) {
        this->normal = aRibeiro::normalize(normal);
        distance = dot(normal, point);
    }

    Plane::Plane(const vec3 &a, const vec3 &b, const vec3 &c) {
        normal = aRibeiro::normalize(cross(b - a, c - a));
        distance = dot(normal, a);
    }

    Plane Plane::fromTriangle(const vec3 &a, const vec3 &b, const vec3 &c) {
        return Plane(a,b,c);
    }

    Plane Plane::fromTriangle(const Triangle &t) {
        return fromTriangle(t.a, t.b, t.c);
    }

    void Plane::normalize() {
        float mag2 = dot(normal, normal);
        const float TOLERANCE = 1e-4f;//0.001f;
        if (absv(mag2) > TOLERANCE && absv(mag2 - 1.0f) > TOLERANCE)
        {
            mag2 = sqrt(mag2);
            mag2 = 1.0f / mag2;
            normal *= mag2;
            distance *= mag2;
        }
    }

    /*
   Quando se aplica os valores: x,y e z à equacao de um plano,
     pode-se obter um numero positivo, negativo ou zero.
   Quando é zero, significa que o ponto (x,y,z) está dentro do plano
   Quando é negativo ou positivo, o número é a distância que este ponto
     está do plano em relação a sua normal, que em seu sentido positivo,
     deixa o numero positivo, e negativo o mesmo.
*/
    float Plane::pointDistanceToPlane(const vec3 &q, const Plane &plane) {
        float t = (dot(plane.normal, q) - plane.distance);// / dot(plane.normal, plane.normal);
        return t;
    }

    vec3 Plane::closestPointToPlane(const vec3 &q, const Plane &plane)
    {
        float t = (dot(plane.normal, q) - plane.distance);// / dot(plane.normal, plane.normal);
        // If the plane equation is known to be normalized
        //float t = (dot(p_norm, q) - p_dist);
        return q - t * plane.normal;
    }

    bool Plane::raycastPlane(const Ray &ray, const Plane &plane, float *outT, vec3 *outNormal) {
        float aux_vd, v0, t;
        aux_vd = dot(plane.normal, ray.dir);
        if (aux_vd >= -EPSILON)
            return false; //paralelo na direção do raio ou na direção contrária do raio
        // if ray nearly parallel to plane, no intersection
        //if (absv(aux_vd) < 1e-6f)
            //return false;
        v0 = plane.distance - dot(plane.normal, ray.origin);
        //if (absv(v0) < EPSILON)
        if (v0 < EPSILON)
            return false;// surface pointing wrong way
        t = v0 / aux_vd;

        *outT = t;
        *outNormal = plane.normal;
        return true;
    }

    bool Plane::segmentIntersectsPlane(const vec3 &a, const vec3 &b, const Plane &plane)
    {
        // Compute the t value for the directed line ab intersecting the plane
        vec3 ab = b - a;
        float t = (plane.distance - dot(plane.normal, a)) / dot(plane.normal, ab);
        // If t in [0..1] compute and return intersection point
        if (t >= 0.0f && t <= 1.0f) {
            //*outT = t;
            //q = a + t * ab;
            return true;
        }
        // Else no intersection
        return false;
    }

    bool Plane::segmentIntersectsPlane(const LineSegment &ls, const Plane &plane) {
        return segmentIntersectsPlane(ls.a, ls.b, plane);
    }

    // Given planes p1 and p2, compute line L = p+t*d of their intersection.
    // Return 0 if no such line exists
    bool Plane::intersectPlaneToPlane(const Plane &p1, const Plane &p2, vec3 *outP, vec3 *outD)
    {
        // Compute direction of intersection line
        vec3 d = cross(p1.normal, p2.normal);
        // If d is (near) zero, the planes are parallel (and separated)
        // or coincident, so they’re not considered intersecting
        float denom = dot(d, d);
        if (denom < EPSILON)
            return false;
        // Compute point on intersection line
        *outP = cross(p1.distance*p2.normal - p2.distance*p1.normal, d) / denom;
        *outD = d;
        return true;
    }

    // Compute the point p at which the three planes p1, p2 and p3 intersect (if at all)
    bool Plane::intersectPlanes(const Plane &p1, const Plane &p2, const Plane &p3, vec3 *outP)
    {
        vec3 u = cross(p2.normal, p3.normal);
        float denom = dot(p1.normal, u);
        if (absv(denom) < EPSILON)
            return false; // Planes do not intersect in a point
        *outP = (p1.distance*u + cross(p1.normal, p3.distance * p2.normal - p2.distance * p3.normal)) / denom;
        return true;
    }

    bool Plane::aabbIntersectsPlane(const AABB &b, const Plane &plane) {
        return AABB::planeIntersectsAABB(plane, b);
    }

}
}
