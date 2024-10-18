#ifndef __MY_ENGINE_H__
#define __MY_ENGINE_H__

#include "../EngineCommon.h"
#include "../XCube2d.h"
//#include "../GraphicsEngine.h"


class MyEngineSystem {
	friend class XCube2Engine;
	private:
		std::shared_ptr<GraphicsEngine> gfxInstance = nullptr;
		std::string vertexShader = "";
		std::string fragmentShader = "";
		Vector2f translateWorldSpaceToDeviceSpace(Vector2f);

	public:
		MyEngineSystem();
		void drawTriangle2D(Vector2f, Vector2f, Vector2f);
};

struct Vector3F
{
public:
	Vector3F(float, float, float);

	// coord getter methods
	float getX() { return x; };
	float getY() { return y; };
	float getZ() { return z; };

	float calculateMagnitude();
	Vector3F getUnitVector();

	// math operations - reference https://learn.microsoft.com/en-us/cpp/cpp/operator-overloading?view=msvc-170
	Vector3F operator+( Vector3F& );
	Vector3F operator-(Vector3F&);
	Vector3F operator*(int);
	Vector3F operator*(float);
	Vector3F operator/(int);
	Vector3F operator/(float);

private:
	float x = 0;
	float y = 0;
	float z = 0;
};

#endif