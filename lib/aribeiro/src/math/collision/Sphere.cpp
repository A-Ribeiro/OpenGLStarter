#include "Sphere.h"

#include <aribeiro/all_math.h>

namespace aRibeiro {

    namespace collision {
        Sphere::Sphere() {
            radius = 1.0f;
        }

        Sphere::Sphere(const vec3 &center, float radius) {
            this->center = center;
            this->radius = radius;
        }

        vec3 Sphere::closestPointToSphere(const vec3& p, const vec3& center, const float& radius) {
            return quadraticClamp(p, center, radius);
        }

        vec3 Sphere::closestPointToSphere(const vec3& p, const Sphere& sphere) {
            return Sphere::closestPointToSphere(p, sphere.center, sphere.radius);
        }

        bool Sphere::sphereOverlapsSphere(const Sphere& a, const Sphere& b) {
            float totalRadius = a.radius + b.radius;
            return sqrLength(a.center - b.center) <= totalRadius * totalRadius;
        }

        Sphere Sphere::joinSpheres(const Sphere &s0, const Sphere &s1)
        {
            Sphere s;
            // Compute the squared distance between the sphere centers
            vec3 d = s1.center - s0.center;
            float dist2 = dot(d, d);
            float sqr_r = (s1.radius - s0.radius);
            sqr_r *= sqr_r;
            if (sqr_r >= dist2) {
                // The sphere with the larger radius encloses the other;
                // just set s to be the larger of the two spheres
                if (s1.radius >= s0.radius)
                    s = s1;
                else
                    s = s0;
            }
            else {
                // Spheres partially overlapping or disjoint
                float dist = sqrt(dist2);
                s.radius = (dist + s0.radius + s1.radius) * 0.5f;
                s.center = s0.center;
                if (dist > EPSILON)
                    s.center += ((s.radius - s0.radius) / dist) * d;
            }
            return s;
        }

        bool Sphere::raycastSphere(const Ray &ray, const vec3 &center, const float &radius, float *outT, vec3 *outNormal) {
            vec3 m = ray.origin - center;
            float b = dot(m, ray.dir);
            float c = dot(m, m) - radius * radius;
            // Exit if r’s origin outside s (c > 0) and r pointing away from s (b > 0)
            if (c > 0.0f && b > 0.0f)
                return false;
            float discr = b * b - c;
            // A negative discriminant corresponds to ray missing sphere
            if (discr < 0.0f)
                return false;
            // Ray now found to intersect sphere, compute smallest t value of intersection
            float t = -b - sqrt(discr);
            // If t is negative, ray started inside sphere so clamp t to zero
            if (t < 0.0f)
                t = 0.0f;
            //q = p + t * d;
            *outT = t;
            *outNormal = normalize((ray.origin + ray.dir * t) - center);
            return true;
        }

        // Intersects ray r = p + td, |d| = 1, with sphere s and, if intersecting,
        // returns t value of intersection and intersection point q
        bool Sphere::raycastSphere(const Ray &ray, const Sphere &sphere, float *outT, vec3 *outNormal)
        {
            return Sphere::raycastSphere(ray, sphere.center, sphere.radius, outT, outNormal);
        }

        bool Sphere::segmentIntersectsSphere(const vec3& p, const vec3& q, const vec3 &center, const float &radius) {
            float t;
            vec3 pq = q - p;
            float lengthPQ = dot(pq, pq);
            const float TOLERANCE = 0.001f;
            // check if can normalize segment
            if (absv(lengthPQ) > TOLERANCE && absv(lengthPQ - 1.0f) > TOLERANCE)
                pq *= 1.0f / lengthPQ;
            else {
                //test the point inside the sphere
                return pointInsideSphere(p, center, radius);
            }

            vec3 n;
            if (raycastSphere(Ray(p, pq), center, radius, &t, &n)) {
                return (t < lengthPQ);
            }
            return false;
        }

        bool Sphere::segmentIntersectsSphere(const LineSegment& ls, const vec3 &center, const float &radius) {
            return Sphere::segmentIntersectsSphere(ls.a, ls.b, center, radius);
        }

        bool Sphere::segmentIntersectsSphere(const vec3& p, const vec3& q, const Sphere &sphere) {
            return Sphere::segmentIntersectsSphere(p, q, sphere.center, sphere.radius);
        }

        bool Sphere::segmentIntersectsSphere(const LineSegment& ls, const Sphere &sphere) {
            return Sphere::segmentIntersectsSphere(ls.a, ls.b, sphere.center, sphere.radius);
        }

        bool Sphere::pointInsideSphere(const vec3& p, const vec3 &center, const float &radius) {
            vec3 p_sc = center - p;
            float sqrDst = dot(p_sc, p_sc);
            return sqrDst <= (radius * radius);
        }

        bool Sphere::pointInsideSphere(const vec3& p, const Sphere &sphere) {
            return Sphere::pointInsideSphere(p, sphere.center, sphere.radius);
        }

        bool Sphere::aabbOverlapsSphere(const AABB& aabb, const vec3 &center, const float &radius, vec3 *penetration) {
            bool result = AABB::sphereOverlapsAABB(center, radius, aabb, penetration);
            if (result)
                *penetration = -(*penetration);
            return result;
        }

        bool Sphere::aabbOverlapsSphere(const AABB& aabb, const Sphere &sphere, vec3 *penetration) {
            bool result = AABB::sphereOverlapsAABB(sphere.center, sphere.radius, aabb, penetration);
            if (result)
                *penetration = -(*penetration);
            return result;
        }

        bool Sphere::frustumOverlapsSphere(const Frustum &f, const vec3 &center, const float &radius) {
            return Frustum::sphereOverlapsFrustum(center, radius, f);
        }

        bool Sphere::frustumOverlapsSphere(const Frustum &f, const Sphere &s) {
            return Frustum::sphereOverlapsFrustum(s.center, s.radius, f);
        }

        bool Sphere::triangleIntersectsSphere(const vec3& a, const vec3& b, const vec3& c, const vec3 &center, const float &radius, vec3 *penetration) {
            bool result = Triangle::sphereIntersectsTriangle(center, radius, a, b, c, penetration);
            if (result)
                *penetration = -(*penetration);
            return result;
        }

        bool Sphere::triangleIntersectsSphere(const vec3& a, const vec3& b, const vec3& c, const Sphere& sphere, vec3 *penetration) {
            bool result = Triangle::sphereIntersectsTriangle(sphere.center, sphere.radius, a, b, c, penetration);
            if (result)
                *penetration = -(*penetration);
            return result;
        }

        bool Sphere::triangleIntersectsSphere(const Triangle& t, const vec3 &center, const float &radius, vec3 *penetration) {
            bool result = Triangle::sphereIntersectsTriangle(center, radius, t.a, t.b, t.c, penetration);
            if (result)
                *penetration = -(*penetration);
            return result;
        }

        bool Sphere::triangleIntersectsSphere(const Triangle& t, const Sphere& sphere, vec3 *penetration) {
            bool result = Triangle::sphereIntersectsTriangle(sphere.center, sphere.radius, t.a, t.b, t.c, penetration);
            if (result)
                *penetration = -(*penetration);
            return result;
        }

        Sphere Sphere::from4Points(const vec3&a, const vec3&b, const vec3&c, const vec3&d) {

            /*
            vec3 a = _a;
            vec3 b = _b;
            vec3 c = _c;
            vec3 d = _d;
            */
            
            //https://gamedev.stackexchange.com/questions/162731/welzl-algorithm-to-find-the-smallest-bounding-sphere
            //https://mathworld.wolfram.com/Circumsphere.html

            // Construct a matrix with the vectors as rows
            mat4 matrix = transpose(mat4(toPtn4(a), toPtn4(b), toPtn4(c), toPtn4(d)));
            float det_a = mat4_determinant(matrix);
            
            /*
             vec3 offset = vec3(0);
            if (absv(det_a) <= EPSILON)
            {
                offset = vec3(10,10,10);
                
                a += offset;
                b += offset;
                c += offset;
                d += offset;
                
                matrix = transpose(mat4(toPtn4(a), toPtn4(b), toPtn4(c), toPtn4(d)));
                det_a = mat4_determinant(matrix);
            }
            */
            
            ARIBEIRO_ABORT(det_a == 0,"All 4 points are coplanar.\n");

            // Copy the matrix so we can modify it 
            // and still read rows from the original.
            mat4 D = matrix;
            vec3 center;

            D.a1 = sqrLength(a);
            D.a2 = sqrLength(b);
            D.a3 = sqrLength(c);
            D.a4 = sqrLength(d);
            
            center.x = mat4_determinant(D);

            D[1] = matrix[0];
            center.y = - mat4_determinant(D);

            D[2] = matrix[1];
            center.z = mat4_determinant(D);

            center *=  1.0f / (2.0f * det_a);
            
            float radius = distance(a, center);
            
            /*
             
            float radius_a = distance(a, center);
            float radius_b = distance(b, center);
            float radius_c = distance(c, center);
            float radius_d = distance(d, center);
            
            float radius = maximum(maximum(radius_a,radius_b),maximum(radius_c,radius_d));
            
            
            printf("radius: %e\n", radius);
            printf("dst from a to center: %e\n", distance(a, center));
            printf("dst from b to center: %e\n", distance(b, center));
            printf("dst from c to center: %e\n", distance(c, center));
            printf("dst from d to center: %e\n", distance(d, center));
            */
            
            //center -= offset;
            
            return Sphere(center, radius);
        }

        Sphere Sphere::fromFrustum(const Frustum& frustum) {
            Sphere result = Sphere::from4Points(
                frustum.vertices[collision::FrustumVertex_Near_Right_Bottom],
                frustum.vertices[collision::FrustumVertex_Near_Left_Top],
                frustum.vertices[collision::FrustumVertex_Far_Left_Top],
                frustum.vertices[collision::FrustumVertex_Far_Left_Bottom]
            );
            
            return result;
        }

        Sphere Sphere::fromAABB(const AABB& aabb, bool discard_z) {
            if (discard_z) {
                vec3 min = vec3(aabb.min_box.x, aabb.min_box.y, 0);
                vec3 max = vec3(aabb.max_box.x, aabb.max_box.y, 0);
                vec3 center = (min + max) * 0.5f;
                float radius = distance(center, max);
                return Sphere(center, radius);
            }
            else {
                vec3 center = (aabb.min_box + aabb.max_box) * 0.5f;
                float radius = distance(center, aabb.max_box);
                return Sphere(center, radius);
            }
        }

        Sphere Sphere::fromLineSegment(const LineSegment& ls) {
            vec3 center = (ls.a + ls.b) * 0.5f;
            float radius = distance(center, ls.a);
            return Sphere(center, radius);
        }

        Sphere Sphere::fromTriangle(const Triangle& triangle) {
            //
            // Not tested...
            //
            vec3 ac = triangle.a - triangle.c;
            vec3 bc = triangle.b - triangle.c;

            vec3 _cross = cross(ac, bc);

            vec3 center = triangle.c + 
                cross( sqrLength(ac) * bc - sqrLength(bc) * ac, _cross) / (2.0f * sqrLength(_cross));

            float radius = distance(triangle.a, center);

            return Sphere(center, radius);
        }

    }
}
