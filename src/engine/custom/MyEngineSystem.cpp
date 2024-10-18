#include "MyEngineSystem.h"

MyEngineSystem::MyEngineSystem()
{
	std::cout << "test" << std::endl;
}

Vector2f MyEngineSystem::translateWorldSpaceToDeviceSpace(Vector2f worldSpaceCoords)
{
	// get instance of graphics engine
	std::shared_ptr<GraphicsEngine> gfx = XCube2Engine::getInstance()->getGraphicsEngine();
	// get the window size to determine ratio (device space has 0,0 as the centre)
	Dimension2i winDimension = gfx->getCurrentWindowSize();
	Vector2f origin = Vector2f(winDimension.w / 2, winDimension.h / 2);

	float offsetX = worldSpaceCoords.x - origin.x;
	float offsetY = (worldSpaceCoords.y - origin.y) * -1;

	// divide this by the values of the origin coords
	Vector2f resultCoords = Vector2f(offsetX / origin.x, offsetY / origin.y);

	return resultCoords;
}

Vector3F::Vector3F(float newX, float newY, float newZ)
{
	x = newX;
	y = newY;
	z = newZ;
}

Vector3F Vector3F::operator*(int scalar)
{
	// scalar multiplication
	float newX = x * scalar;
	float newY = y * scalar;
	float newZ = z * scalar;

	Vector3F result = Vector3F(newX, newY, newZ);

	return result;
}

// error was due to not specfying the struct this method belongs to
// see https://stackoverflow.com/questions/23505423/why-c-operator-overloading-requires-having-at-least-one-parameter-of-class-ty
Vector3F Vector3F::operator*(float scalar)
{
	float newX = x * scalar;
	float newY = y * scalar;
	float newZ = z * scalar;

	return Vector3F(newX, newY, newZ);
}

Vector3F Vector3F::operator/(int scalar)
{
	float newX = x / scalar;
	float newY = y / scalar;
	float newZ = z / scalar;

	return Vector3F(newX, newY, newZ);
}

Vector3F Vector3F::operator/(float scalar)
{
	float newX = x / scalar;
	float newY = y / scalar;
	float newZ = z / scalar;

	return Vector3F(newX, newY, newZ);
}

Vector3F Vector3F::operator+(Vector3F& operand)
{
	return Vector3F(x + operand.getX(), y + operand.getY(), z + operand.getZ());
}

Vector3F Vector3F::operator-(Vector3F& operand)
{
	return Vector3F(x - operand.getX(), y - operand.getY(), z - operand.getZ());
}

float Vector3F::calculateMagnitude()
{
	float squareSum = (x * x) + (y * y) + (z * z);
	float res = sqrtf(squareSum);

	return res;
}

Vector3F Vector3F::getUnitVector()
{
	float mag = calculateMagnitude();

	return Vector3F(x, y, z) / mag;
}


/*
	Steps to create a shape:
	1. define coordinates for each vertex in world space
	2. translate these to device space
	3. add vertex coordinates to vertex buffer array
	4. construct vertex buffer object (use glGenBuffers, pass in no of buffers to gen and address of reference value [GLuint])
	5. make last created buffer the active one
	6. 
	5. send data to graphics card
*/