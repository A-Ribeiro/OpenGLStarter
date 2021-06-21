#ifndef RaytracerObjects_H
#define RaytracerObjects_H

#include <aribeiro/aribeiro.h>
using namespace aRibeiro;

#include "FileReader.h"


//-----------------------------------------------------------------------------
//
// Collision Code
//
//-----------------------------------------------------------------------------
bool RayCastSphere(const collision::Ray &ray, const collision::Sphere &sphere, float *outT, vec3 *outNormal) {

    vec3 OC, r, R0, Ec;
    float auxDoc2, tm, ta, ti;
    R0 = ray.origin;
    r = ray.dir;
    Ec = sphere.center;
    OC = Ec - R0;
    auxDoc2 = dot(OC, OC);
    float sqrRadius = sphere.radius*sphere.radius;
    if (auxDoc2 > sqrRadius + sqrRadius * EPSILON) {//o raio nao esta dentro da esfera
        tm = dot(OC, r);
        if (tm > EPSILON) {//raio na direcao da esfera
            ta = sqrRadius - auxDoc2 + tm * tm;
            if (ta > EPSILON) {//raio intercepta a esfera
                ta = sqrt(ta);
                ti = tm - ta;
                if (ti > EPSILON) {
                    *outT = ti;
                    *outNormal = normalize(ray.origin + ray.dir*ti - sphere.center);
                    return true;
                }
            }
        }
    }
    else if (auxDoc2 < sqrRadius - sqrRadius * EPSILON) {
        //o raio esta dentro da esfera
        tm = dot(OC, r);
        ta = sqrRadius - auxDoc2 + tm * tm;
        ta = sqrt(ta);
        ti = tm + ta;
        if (ti > EPSILON) {
            *outT = ti;
            *outNormal = -normalize(ray.origin + ray.dir*ti - sphere.center);
            return true;
        }
    }
    return false;
}

bool RayCastPlane(const collision::Ray &ray, const collision::Plane &plane, float *outT, vec3 *outNormal) {
    vec3 planeNormal;
    float aux_vd, v0, t;
    planeNormal = plane.normal;
    aux_vd = dot(planeNormal, ray.dir);
    if (aux_vd >= -EPSILON)
        return false; //paralelo na direcao do raio ou na direcao contraria do raio
    v0 = -(dot(planeNormal, ray.origin) + plane.distance);
    if (absv(v0) < EPSILON)
        return false;
    t = v0 / aux_vd;
    if (t > EPSILON) {
        *outT = t;
        *outNormal = -sign(aux_vd) * plane.normal;
        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------
class Camera {
public:

    vec3 eyePos;
    vec3 eyeLookToPosition;
    vec3 eyeUp;
    float fieldOfView;

    static Camera readFromFile(FileReader *inputFile) {
        Camera result;

        result.eyePos.x = inputFile->readFloat();
        result.eyePos.y = inputFile->readFloat();
        result.eyePos.z = inputFile->readFloat();

        result.eyeLookToPosition.x = inputFile->readFloat();
        result.eyeLookToPosition.y = inputFile->readFloat();
        result.eyeLookToPosition.z = inputFile->readFloat();

        result.eyeUp.x = inputFile->readFloat();
        result.eyeUp.y = inputFile->readFloat();
        result.eyeUp.z = inputFile->readFloat();

        result.fieldOfView = inputFile->readFloat();

        return result;
    }

};
//-----------------------------------------------------------------------------
class Light {
public:
    vec3 pos;
    vec3 rgb;
    float constantAttenuation;
    float linearAttenuation;
    float QuadraticAttenuation;

    static Light readFromFile(FileReader *inputFile) {
        Light result;
        result.pos.x = inputFile->readFloat();
        result.pos.y = inputFile->readFloat();
        result.pos.z = inputFile->readFloat();

        result.rgb.x = inputFile->readFloat();
        result.rgb.y = inputFile->readFloat();
        result.rgb.z = inputFile->readFloat();

        result.constantAttenuation = inputFile->readFloat();
        result.linearAttenuation = inputFile->readFloat();
        result.QuadraticAttenuation = inputFile->readFloat();

        return result;
    }

};

//-----------------------------------------------------------------------------
enum PigmentType {
    PigmentTypeNone,
    PigmentTypeSolid,
    PigmentTypeChecker,
};

class Pigment {
public:

    PigmentType pigmentType;

    std::string name;

    //solid parameters
    vec3 solidRGB;

    //checker parameters
    vec3 checkerRGB1;
    vec3 checkerRGB2;
    float checkersize;

    Pigment() {
        pigmentType = PigmentTypeNone;
    }

    static Pigment readFromFile(FileReader *inputFile) {
        Pigment result;

        result.name = inputFile->readString();

        if (result.name.compare("solid") == 0) {

            result.pigmentType = PigmentTypeSolid;

            result.solidRGB.r = inputFile->readFloat();
            result.solidRGB.g = inputFile->readFloat();
            result.solidRGB.b = inputFile->readFloat();

        }
        else if (result.name.compare("checker") == 0) {

            result.pigmentType = PigmentTypeChecker;

            result.checkerRGB1.r = inputFile->readFloat();
            result.checkerRGB1.g = inputFile->readFloat();
            result.checkerRGB1.b = inputFile->readFloat();

            result.checkerRGB2.r = inputFile->readFloat();
            result.checkerRGB2.g = inputFile->readFloat();
            result.checkerRGB2.b = inputFile->readFloat();

            result.checkersize = inputFile->readFloat();
        }

        return result;
    }

    vec3 getColor(vec3 pos) {
        if (pigmentType == PigmentTypeSolid) {
            return solidRGB;
        }
        else if (pigmentType == PigmentTypeChecker) {
            int a = (int)(pos.x / checkersize) % 2,
                b = (int)(pos.y / checkersize) % 2,
                c = (int)(pos.z / checkersize) % 2;
            if (pos.x < 0) a = (a + 1) % 2;
            if (pos.y < 0) b = (b + 1) % 2;
            if (pos.z < 0) c = (c + 1) % 2;
            if ((a + b + c) % 2 == 0)
                return checkerRGB1;
            else
                return checkerRGB2;
        }
        return vec3(1.0,0.0,1.0);//magenta
    }
};
//-----------------------------------------------------------------------------
class Finishing {
public:

    //illumination parameters
    float ambientReflection;
    float diffuseReflection;
    float specularReflection;
    float specularShininess;

    //reflection and refraction parameters
    float reflectionPercentage;
    float transmissionPercentage;
    float refractiveIndex;

    static Finishing readFromFile(FileReader *inputFile) {
        Finishing result;

        result.ambientReflection = inputFile->readFloat();
        result.diffuseReflection = inputFile->readFloat();
        result.specularReflection = inputFile->readFloat();
        result.specularShininess = inputFile->readFloat();

        result.reflectionPercentage = inputFile->readFloat();
        result.transmissionPercentage = inputFile->readFloat();
        result.refractiveIndex = inputFile->readFloat();

        return result;
    }

};
//-----------------------------------------------------------------------------
enum ObjectType {
    ObjectTypeNone,
    ObjectTypeSphere,
    ObjectTypePlane
};

class Object {
public:

    ObjectType objectType;

    Pigment *pigment;
    Finishing *finishing;

    std::string name;

    //sphere
    collision::Sphere sphere;

    //plane
    collision::Plane plane;

    Object() {
        objectType = ObjectTypeNone;
    }

    static Object readFromFile(FileReader *inputFile,
        aligned_vector<Pigment> *pigmentListRef,
        aligned_vector<Finishing> *finishingListRef ) {
        Object result;

        int pigmentIndex = inputFile->readInt();
        result.pigment = &(*pigmentListRef)[pigmentIndex];

        int finishingIndex = inputFile->readInt();
        result.finishing = &(*finishingListRef)[finishingIndex];

        result.name = inputFile->readString();

        if (result.name.compare("sphere") == 0) {
            result.objectType = ObjectTypeSphere;

            vec3 sphereCenter;
            sphereCenter.x = inputFile->readFloat();
            sphereCenter.y = inputFile->readFloat();
            sphereCenter.z = inputFile->readFloat();

            float sphereRadius = inputFile->readFloat();

            result.sphere.center = sphereCenter;
            result.sphere.radius = sphereRadius;

        }
        else if (result.name.compare("plane") == 0) {

            result.objectType = ObjectTypePlane;

            float planeA = inputFile->readFloat();
            float planeB = inputFile->readFloat();
            float planeC = inputFile->readFloat();
            float planeD = inputFile->readFloat();

            result.plane.normal.x = planeA;
            result.plane.normal.y = planeB;
            result.plane.normal.z = planeC;
            result.plane.distance = planeD;
        }

        return result;
    }


    bool raycast(const collision::Ray &ray, float *outT, vec3 *outNormal) {
        if (objectType == ObjectTypeSphere) {
            vec3 normalAux;
            float tAux;
            //if (collision::Ray::raycastSphere(ray, objects[i].sphere, &tAux, &normalAux)) {
            if (RayCastSphere(ray, sphere, &tAux, &normalAux)) {
                *outT = tAux;
                *outNormal = normalAux;
                return true;
            }
        }
        else if (objectType == ObjectTypePlane) {
            vec3 normalAux;
            float tAux;
            //if (collision::Ray::raycastPlane(ray, plane, &tAux, &normalAux)) {
            if (RayCastPlane(ray, plane, &tAux, &normalAux)) {
                *outT = tAux;
                *outNormal = normalAux;
                return true;
            }
        }
        return false;
    }

};
//-----------------------------------------------------------------------------

#endif
