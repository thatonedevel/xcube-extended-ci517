#ifndef __MY_ENGINE_H__
#define __MY_ENGINE_H__

#include "../EngineCommon.h"

class MyEngineSystem {
	friend class XCube2Engine;
	private:

	public:
		MyEngineSystem();
};

struct Vector3F
{
public:
	Vector3F(float, float, float);

	// coord getter methods
	float getX();
	float getY();
	float getZ();

	float calculateMagnitude();
	Vector3F getUnitVector();

	// math operations - reference https://learn.microsoft.com/en-us/cpp/cpp/operator-overloading?view=msvc-170
	Vector3F operator+( Vector3F& );
	Vector3F operator-(Vector3F&);
	Vector3F operator*(Vector3F&, int);
	Vector3F operator*(float);
	Vector3F operator/(int);
	Vector3F operator/(float);

private:
	float x = 0;
	float y = 0;
	float z = 0;
};

#endif