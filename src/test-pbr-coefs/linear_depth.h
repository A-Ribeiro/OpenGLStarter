#pragma once

#include <InteractiveToolkit/MathCore/MathCore.h>

using namespace MathCore;


float DepthToWorld(float n, float f, float z) {
    z = z * 2.0 - 1.0;
    return (2.0 * n * f) / (f + n - z * (f - n));
}

static void linear_depth_Values() {

    //configure projection
    float fov = 60.0f;
    float aspect = 1.0f;
    float _near = 0.01f;
    float _far = 10.0f;

    mat4f projection = GEN<mat4f>::projection_perspective_lh_negative_one(fov, aspect, _near, _far);

    const int MAX_POINTS = 10;
    for(int i=0;i<=MAX_POINTS;i++){
        float z = OP<float>::lerp(_near, _near *2.0f, (float)i/(float)MAX_POINTS);
        vec4f point_src = vec4f(1,0,z,1);

        //project the point
        vec4f proj_pt = projection * point_src;
        vec3f zbuffer_pt = CVT<vec4f>::toVec3_PerspDiv(proj_pt);

        //OpenGL returns the value in the range: 0..1
        zbuffer_pt.z = zbuffer_pt.z * 0.5f + 0.5f;
        //zbuffer_pt.z = (zbuffer_pt.z + 1.0f) * 0.5f;

        printf("point_src z = %f\n", point_src.z);
        printf("zbuffer_pt z = %f\n", zbuffer_pt.z);
        printf("DepthToWorld z = %f\n", DepthToWorld(_near, _far, zbuffer_pt.z));
        
        printf("\n");

    }
   
}


static void linear_ortho_Values() {

    //configure projection
    //float fov = 60.0f;
    //float aspect = 1.0f;
    float _near = 0.01f;
    float _far = 10.0f;

    mat4f projection = GEN<mat4f>::projection_ortho_lh_negative_one(
        -1, 1,
        -1, 1,
        _near,
        _far
    );

    const int MAX_POINTS = 10;
    for (int i = 0; i <= MAX_POINTS; i++) {
        float z = OP<float>::lerp(_near, _far, (float)i / (float)MAX_POINTS);
        vec4f point_src = vec4f(1, 0, z, 1);

        //project the point
        vec4f proj_pt = projection * point_src;
        vec3f zbuffer_pt = CVT<vec4f>::toVec3_PerspDiv(proj_pt);

        //OpenGL returns the value in the range: 0..1
        zbuffer_pt.z = zbuffer_pt.z * 0.5f + 0.5f;
        //zbuffer_pt.z = (zbuffer_pt.z + 1.0f) * 0.5f;

        printf("point_src z = %f\n", point_src.z);
        printf("zbuffer_pt z = %f\n", zbuffer_pt.z);

        printf("\n");
    }



}