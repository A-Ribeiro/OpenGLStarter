#pragma once

#include <InteractiveToolkit/MathCore/MathCore.h>

using namespace MathCore;

vec3f st2normal(vec2f st){
	float phi = st.y * CONSTANT<float>::PI;
	float theta = st.x * 2.0f * CONSTANT<float>::PI;
	vec3f result;
	result.x = sin(phi) * cos(theta);
	result.y = cos(phi);
	result.z = sin(phi) * sin(theta);
	return result;
}

vec2f normal2st(vec3f normal){
	float t = acos(normal.y) / CONSTANT<float>::PI;
	float s = atan2(normal.z, normal.x) / (2.0f * CONSTANT<float>::PI);
	s = MathCore::OP<float>::fmod(s + 1.0f, 1.0f);
	return MathCore::vec2f(s, t);
}



void testPolar_Values() {

	for (int phi = 0; phi <= 180; phi +=45) {
		float t = (float)phi / (float)180;
		for (int theta = 0; theta <= 360; theta += 45) {
			float s = (float)theta / (float)360;
			vec3f normal = st2normal(MathCore::vec2f(s, t));
			printf("   s :  %.3f   t :  %.3f   ->  normal: %f %f %f\n", s, t, normal.x, normal.y, normal.z);
			vec2f st = normal2st(normal);
			printf("   s -> %.3f   t -> %.3f\n", st.x, st.y);
		}
		printf("\n");
	}



}