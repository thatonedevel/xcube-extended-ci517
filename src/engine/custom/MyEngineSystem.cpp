#include "MyEngineSystem.h"

MyEngineSystem::MyEngineSystem()
{
	std::cout << "test" << std::endl;
}

Vector3F::Vector3F(float newX, float newY, float newZ)
{
	x = newX;
	y = newY;
	z = newZ;
}

Vector3F operator*(Vector3F& main, int scalar)
{
	// scalar multiplication

}


float Vector3F::calculateMagnitude()
{
	float squareSum = (x*x) + (y*y) + (z*z);
	float res = sqrtf(squareSum);

	return res;
}

Vector3F Vector3F::getUnitVector()
{

}