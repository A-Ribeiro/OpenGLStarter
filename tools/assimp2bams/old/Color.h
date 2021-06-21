#ifndef Color_H
#define Color_H

#include <aribeiro/aribeiro.h>
//using namespace aRibeiro;

class Color{
public:
	union{
		unsigned char r,g,b,a;
		unsigned char array[4];
	};

	Color();
	Color(const aRibeiro::vec3& v);
	Color(const float& v);
	Color(const aRibeiro::vec4& v);
	Color(const Color& v);

	Color& operator=(const aRibeiro::vec3& v);
	Color& operator=(const aRibeiro::vec4& v);
	Color& operator=(const Color& v);
	Color& operator=(const float& v);

	aRibeiro::vec3 toVec3()const;
	aRibeiro::vec4 toVec4()const;

};

#endif
