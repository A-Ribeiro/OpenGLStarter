#include "Color.h"

Color::Color(){
	r=0;
	g=0;
	b=0;
	a=255;
}

Color::Color(const aRibeiro::vec3& v){
	*this = v;
}
Color::Color(const float& v){
	*this = v;
}
Color::Color(const aRibeiro::vec4& v){
	*this = v;
}
Color::Color(const Color& v){
	*this = v;
}

Color& Color::operator=(const aRibeiro::vec3& v){
	aRibeiro::vec3 aux = aRibeiro::clamp(v, aRibeiro::vec3(0), aRibeiro::vec3(1))/255.0f + 0.5f;
	r = (unsigned char)aux.x;
	g = (unsigned char)aux.y;
	b = (unsigned char)aux.z;
	a = 255;
	return *this;
}

Color& Color::operator=(const aRibeiro::vec4& v){
	aRibeiro::vec4 aux = aRibeiro::clamp(v, aRibeiro::vec4(0), aRibeiro::vec4(1))/255.0f + 0.5f;
	r = (unsigned char)aux.x;
	g = (unsigned char)aux.y;
	b = (unsigned char)aux.z;
	a = (unsigned char)aux.w;
	return *this;
}

Color& Color::operator=(const Color& v){
	r = v.r;
	g = v.g;
	b = v.b;
	a = v.a;
	return *this;
}

Color& Color::operator=(const float& v){
	r = v;
	g = v;
	b = v;
	a = v;
	return *this;
}

aRibeiro::vec3 Color::toVec3()const{
	return aRibeiro::vec3(r,g,b)/255.0f;
}

aRibeiro::vec4 Color::toVec4()const{
	return aRibeiro::vec4(r,g,b,a)/255.0f;
}