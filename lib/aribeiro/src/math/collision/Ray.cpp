#include "Ray.h"

#include "../all_math.h"

namespace aRibeiro {
namespace collision {
    Ray::Ray() {

    }

    Ray::Ray(const vec3 &origin, const vec3 &dir) {
        this->origin = origin;
        this->dir = dir;
    }


    bool Ray::raycastAABB(const Ray &r, const AABB& a, float *outTmin, vec3 *outNormal) {
        return AABB::raycastAABB(r, a, outTmin, outNormal);
    }

    bool Ray::raycastPlane(const Ray &r, const Plane &plane, float *outT, vec3 *outNormal) {
        return Plane::raycastPlane(r, plane, outT, outNormal);
    }

    bool Ray::raycastSphere(const Ray &r, const vec3 &center, const float& radius, float *outT, vec3 *outNormal) {
        return Sphere::raycastSphere(r, center, radius, outT, outNormal);
    }

    bool Ray::raycastSphere(const Ray &r, const Sphere &sphere, float *outT, vec3 *outNormal) {
        return Sphere::raycastSphere(r, sphere.center, sphere.radius, outT, outNormal);
    }

    bool Ray::raycastTriangle(const Ray &ray, const vec3 &a, const vec3 &b, const vec3&c, float *outT, vec3 *outNormal) {
        return Triangle::raycastTriangle(ray, a, b, c, outT, outNormal);
    }

    bool Ray::raycastTriangle(const Ray &ray, const Triangle &t, float *outT, vec3 *outNormal) {
        return Triangle::raycastTriangle(ray, t.a, t.b, t.c, outT, outNormal);
    }
}

}

