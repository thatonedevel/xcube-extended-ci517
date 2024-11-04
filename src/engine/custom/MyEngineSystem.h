#ifndef __MY_ENGINE_H__
#define __MY_ENGINE_H__

#include "../EngineCommon.h"
#include "../GraphicsEngine.h"

std::vector<std::string> splitString(std::string input, char splitDelimeter);

class MyEngineSystem {
	friend class XCube2Engine;
	private:
		std::shared_ptr<GraphicsEngine> gfxInstance = nullptr;
		const char* vertexShaderSource = "";
		const char* fragmentShaderSource = "";
		Vector2f translateWorldSpaceToDeviceSpace(Vector2f);

		// shaders
		GLuint vertexShader;
		GLuint fragShader;

		// shader program (used to combine shaders)
		GLuint myEngineShaderProg;
		GLuint myEngineSysVBO;
		GLuint vertexArrObj;

	public:
		MyEngineSystem(std::shared_ptr<GraphicsEngine> gfx);
		~MyEngineSystem();
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

struct Face3D
{
public:
	Face3D(int, int, int, int);
	int getVertexIndA() { return vertexIndA; };
	int getVertexIndB() { return vertexIndB; };
	int getVertexIndC() { return vertexIndC; };
	int getNormalIndex() { return normalIndex; };

private:
	int vertexIndA;
	int vertexIndB;
	int vertexIndC;

	int normalIndex;
};

class Mesh3D
{
public:
	Mesh3D();
	Mesh3D(std::string path);
	Vector3F getEulerRotation() { return eulerRotation; };
	Vector3F getOriginPosition() { return originPosition; };
	void moveObject(Vector3F translation);
private:
	Vector3F originPosition = Vector3F(0, 0, 0);
	Vector3F eulerRotation = Vector3F(0, 0, 0);
	Vector3F scale = Vector3F(1, 1, 1);

	std::vector<Vector3F>* vertices; // use a dynamic structure to allow for vertices to be added / removed
	std::vector<Face3D>* faces;
	std::vector<Vector3F>* normals;
};



#endif