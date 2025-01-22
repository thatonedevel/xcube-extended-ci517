#ifndef __MY_ENGINE_H__
#define __MY_ENGINE_H__

#include "../EngineCommon.h"
#include "../GraphicsEngine.h"

#define DEG2RAD 0.017453292519943295769236907684f

#define SCREEN_METRE 100

// 1 "metre" = 32 pixels

std::vector<std::string> splitString(std::string input, char splitDelimeter);

std::string joinString(std::string joinedStrs[], std::string joinChar);

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
	Vector3F operator+(Vector3F&);
	Vector3F operator-(Vector3F&);
	Vector3F operator*(int);
	Vector3F operator*(float);
	Vector3F operator/(int);
	Vector3F operator/(float);
	float dot(Vector3F);
	Vector3F cross(Vector3F);

private:
	float x = 0;
	float y = 0;
	float z = 0;
};

class Matrix4f { // reference https://github.com/AlmasB/tetris3d/blob/master/src/engine/math/GameMath.h
	/*
		4x4 Projection Matrix
		Almas Baimagambetov (2013-2014)
		https://github.com/AlmasB/tetris3d/blob/master/src/engine/math/GameMath.h (accessed 15/11/2024)
	*/
public:
	float m[4][4];

	/**
	* Matrix is deliberately left uninitialised because values
	* will be rewritten anyway
	*/
	Matrix4f() {}

	inline void loadIdentity() {
		m[0][0] = 1.0f; m[0][1] = 0.0f; m[0][2] = 0.0f; m[0][3] = 0.0f;
		m[1][0] = 0.0f; m[1][1] = 1.0f; m[1][2] = 0.0f; m[1][3] = 0.0f;
		m[2][0] = 0.0f; m[2][1] = 0.0f; m[2][2] = 1.0f; m[2][3] = 0.0f;
		m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f; m[3][3] = 1.0f;
	}

	inline Matrix4f operator*(const Matrix4f& right) const {
		Matrix4f ret;

		for (unsigned int i = 0; i < 4; ++i) {
			for (unsigned int j = 0; j < 4; ++j) {
				ret.m[i][j] = m[i][0] * right.m[0][j] +
					m[i][1] * right.m[1][j] +
					m[i][2] * right.m[2][j] +
					m[i][3] * right.m[3][j];
			}
		}

		return ret;
	}

	void scale(float scaleX, float scaleY, float scaleZ) {
		m[0][0] = scaleX; m[0][1] = 0.0f;   m[0][2] = 0.0f;   m[0][3] = 0.0f;
		m[1][0] = 0.0f;   m[1][1] = scaleY; m[1][2] = 0.0f;   m[1][3] = 0.0f;
		m[2][0] = 0.0f;   m[2][1] = 0.0f;   m[2][2] = scaleZ; m[2][3] = 0.0f;
		m[3][0] = 0.0f;   m[3][1] = 0.0f;   m[3][2] = 0.0f;   m[3][3] = 1.0f;
	}

	void rotate(float rotateX, float rotateY, float rotateZ) {
		Matrix4f rx, ry, rz;

		float x = rotateX * DEG2RAD;
		float y = rotateY * DEG2RAD;
		float z = rotateZ * DEG2RAD;

		rx.m[0][0] = 1.0f; rx.m[0][1] = 0.0f;	 rx.m[0][2] = 0.0f;		rx.m[0][3] = 0.0f;
		rx.m[1][0] = 0.0f; rx.m[1][1] = cosf(x); rx.m[1][2] = -sinf(x); rx.m[1][3] = 0.0f;
		rx.m[2][0] = 0.0f; rx.m[2][1] = sinf(x); rx.m[2][2] = cosf(x);	rx.m[2][3] = 0.0f;
		rx.m[3][0] = 0.0f; rx.m[3][1] = 0.0f;	 rx.m[3][2] = 0.0f;		rx.m[3][3] = 1.0f;

		ry.m[0][0] = cosf(y);	ry.m[0][1] = 0.0f; ry.m[0][2] = -sinf(y);	ry.m[0][3] = 0.0f;
		ry.m[1][0] = 0.0f;		ry.m[1][1] = 1.0f; ry.m[1][2] = 0.0f;		ry.m[1][3] = 0.0f;
		ry.m[2][0] = sinf(y);	ry.m[2][1] = 0.0f; ry.m[2][2] = cosf(y);	ry.m[2][3] = 0.0f;
		ry.m[3][0] = 0.0f;		ry.m[3][1] = 0.0f; ry.m[3][2] = 0.0f;		ry.m[3][3] = 1.0f;

		rz.m[0][0] = cosf(z); rz.m[0][1] = -sinf(z); rz.m[0][2] = 0.0f; rz.m[0][3] = 0.0f;
		rz.m[1][0] = sinf(z); rz.m[1][1] = cosf(z);  rz.m[1][2] = 0.0f; rz.m[1][3] = 0.0f;
		rz.m[2][0] = 0.0f;	  rz.m[2][1] = 0.0f;	 rz.m[2][2] = 1.0f; rz.m[2][3] = 0.0f;
		rz.m[3][0] = 0.0f;	  rz.m[3][1] = 0.0f;	 rz.m[3][2] = 0.0f; rz.m[3][3] = 1.0f;

		*this = rz * ry * rx;
	}

	void translate(float x, float y, float z) {
		m[0][0] = 1.0f; m[0][1] = 0.0f; m[0][2] = 0.0f; m[0][3] = x;
		m[1][0] = 0.0f; m[1][1] = 1.0f; m[1][2] = 0.0f; m[1][3] = y;
		m[2][0] = 0.0f; m[2][1] = 0.0f; m[2][2] = 1.0f; m[2][3] = z;
		m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f; m[3][3] = 1.0f;
	}

	void initCameraTransform(const Vector3F& Target, const Vector3F& Up) {
		Vector3F N = Target;
		N = N.getUnitVector();
		Vector3F U = Up;
		U = U.getUnitVector();
		U = U.cross(N);
		Vector3F V = N.cross(U);

		m[0][0] = U.getX();   m[0][1] = U.getY();   m[0][2] = U.getZ();   m[0][3] = 0.0f;
		m[1][0] = V.getX();   m[1][1] = V.getY();   m[1][2] = V.getZ();   m[1][3] = 0.0f;
		m[2][0] = N.getX();   m[2][1] = N.getY();   m[2][2] = N.getZ();   m[2][3] = 0.0f;
		m[3][0] = 0.0f;  m[3][1] = 0.0f;  m[3][2] = 0.0f;  m[3][3] = 1.0f;
	}

	void setPerspectiveProjection(float fov, float width, float height, float zNear, float zFar) {
		float ar = width / height;
		float zRange = zNear - zFar;
		float tanHalfFOV = tanf((fov / 2.0f) * DEG2RAD);

		m[0][0] = 1.0f / (tanHalfFOV * ar); m[0][1] = 0.0f;            m[0][2] = 0.0f;          m[0][3] = 0.0;
		m[1][0] = 0.0f;                   m[1][1] = 1.0f / tanHalfFOV; m[1][2] = 0.0f;          m[1][3] = 0.0;
		m[2][0] = 0.0f;                   m[2][1] = 0.0f;            m[2][2] = (-zNear - zFar) / zRange; m[2][3] = 2.0f * zFar * zNear / zRange;
		m[3][0] = 0.0f;                   m[3][1] = 0.0f;            m[3][2] = 1.0f;          m[3][3] = 0.0;
	}

	std::vector<GLfloat> getMatAs1dArray()
	{
		std::vector<float> mat = {};

		for (int row = 0; row < 4; row++)
		{
			for (int col = 0; col < 4; col++)
			{
				mat.push_back(m[row][col]);
			}
		}
	}
};

class Camera
{
private:
	float fieldOfView = 0;
	float near = 0;
	float far = 0;
	float left = 0;
	float right = 0;
	float top = 0;
	float bottom = 0;
	Vector3F pos = Vector3F(0, 0, 0);
public:
	Camera(Vector3F position, float fov, Dimension2i winSize, float nearPlane, float farPlane);
	Matrix4f cameraMat = Matrix4f();

	// getter methods for camera info
	float getFOV() { return fieldOfView; };
	float getNearPlane() { return near; };
	float getFarPlane() { return far; };
	float getLeftPlane() { return left; };
	float getRightPlane() { return right; };
	float getTopPlane() { return top; };
	float getBottomPlane() { return bottom; };
	Vector3F getPos() { return pos; };
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
	Mesh3D(std::string path, Vector3F position);
	Vector3F getEulerRotation() { return eulerRotation; };
	Vector3F getOriginPosition() { return originPosition; };
	Vector3F getVertexCoordinate(int index) { return (*vertices)[index]; };
	size_t getVertexCount() { return vertices->size(); };
	void moveObject(Vector3F translation);
	void rotateYAxis(float angle) { eulerRotation = eulerRotation + Vector3F(0, angle, 0); };
	Vector3F getFaceNormal(int faceIndex);
	Vector3F getScale() { return scale; };
	void setScale(Vector3F newScale) { scale = newScale; };

	// getters
	Face3D getFaceAtIndex(int index) { return (*faces)[index]; };
	size_t getFaceCount() { return faces->size(); };
private:
	Vector3F originPosition = Vector3F(0, 0, 0);
	Vector3F eulerRotation = Vector3F(0, 0, 0);
	Vector3F scale = Vector3F(1, 1, 1);
	std::vector<Vector3F> evaluateYAxisRotation();

	std::vector<Vector3F>* vertices = nullptr; // use a dynamic structure to allow for vertices to be added / removed
	std::vector<Face3D>* faces = nullptr;
	std::vector<Vector3F>* normals = nullptr;
};

class MyEngineSystem {
	friend class XCube2Engine;
private:
	std::shared_ptr<GraphicsEngine> gfxInstance = nullptr;
	const char* vertexShaderSource = "";
	const char* fragmentShaderSource = "";
	const char* vertexShader2DSource = "";
	Vector2f translateWorldSpaceToDeviceSpace(Vector2f);
	Vector3F translateWorldSpaceToDeviceSpace(Vector3F);

	// shaders
	GLuint vertexShader;
	GLuint vertexShader2D;
	GLuint fragShader;

	// shader program (used to combine shaders)
	GLuint myEngineShaderProg;
	GLuint myEngineSysVBO;
	GLuint vertexArrObj;

	// vertex stream vector, declared on heap
	std::vector<float>* vertexStream = nullptr;
	int zDepth = 100; // absolute depth value, 100 units either side of the origin on the z axis

	// cameras that can be used for rendering
	std::vector<Camera>* renderCameras = nullptr;
	void populateVertexStream(Mesh3D, bool);

public:
	MyEngineSystem(std::shared_ptr<GraphicsEngine> gfx);
	~MyEngineSystem();
	void drawMeshesIn2D(int camIndex, Mesh3D model);
	void drawMeshObjects(int camIndex, Mesh3D, GLint useOrthoProjection);
	void setZDepth(int zVal) { zDepth = zVal; };
	void addCamera(Camera cam);
	Camera getCamera(int camIndex);
	void loadShader(std::string, GLenum, GLuint* target, const char* source);
};

// test versions of shader functions to determine where the faulty data is
Vector2f rotateVertex(Vector2f, float);
Vector2f scaleVertex(Vector2f, Vector2f);
Vector2f projectCoordinate(Vector2f, Vector2f);

#endif