#include "MyEngineSystem.h"

MyEngineSystem::MyEngineSystem(std::shared_ptr<GraphicsEngine> gfx)
{
	gfxInstance = gfx;
	// create vertex array object - this stores the links between the vertex attributes and buffer objects
	glGenVertexArrays(1, &vertexArrObj);
	glBindVertexArray(vertexArrObj);
	// generate the buffer and bind it
	glGenBuffers(1, &myEngineSysVBO);
	glBindBuffer(GL_ARRAY_BUFFER, myEngineSysVBO);

	// set up vertex shaders (taken from open.gl)
	// version no is for the glsl version (i.e. 150)
	vertexShaderSource = R"glsl(
	#version 150 core
	in vec2 position;
	
	void main()
	{
		// gl_Position is the shader output for where to display the vertices
		// vec4 is used to add data for the position. components can be passed individually or through a vec
		gl_Position = vec4(position, 0.0, 1.0);
	}
)glsl";

	// fragment shader
	// needs output of the final colour (outColor)
	// uses vec4 for colour data (rgba)
	// use fragment shader to control shape colours
	fragmentShaderSource = R"glsl(
	# version 150 core
	out vec4 outColor;

	void main()
	{
		outColor = vec4(1.0, 1.0, 1.0, 1.0);
	}
)glsl";

	// compile the shaders - create a shader object, then bind the shader source to it
	// memory is handled in the same way as it is for vertex buffer objects
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	fragShader = glCreateShader(GL_FRAGMENT_SHADER);

	// bind shader source to the shader objects
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glShaderSource(fragShader, 1, &fragmentShaderSource, NULL);

	// compile the shaders
	glCompileShader(vertexShader);

	// check for errors
	GLint res;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &res);

	if (res == GL_TRUE)
	{
		std::cout << "Vertex shader compiled successfully\n";
	}

	glCompileShader(fragShader);
	glGetShaderiv(fragShader, GL_COMPILE_STATUS, &res);
	if (res == GL_TRUE)
	{
		std::cout << "Fragment shader compiled successfully\n";
	}

	// attach the shaders to the program
	myEngineShaderProg = glCreateProgram();

	glAttachShader(myEngineShaderProg, vertexShader);
	glAttachShader(myEngineShaderProg, fragShader);

	glBindFragDataLocation(fragShader, 0, "outColor");
	
	glLinkProgram(myEngineShaderProg);
	glUseProgram(myEngineShaderProg);
}

MyEngineSystem::~MyEngineSystem()
{
	// subsystem destructor

}

Vector2f MyEngineSystem::translateWorldSpaceToDeviceSpace(Vector2f worldSpaceCoords)
{
	// get the window size to determine ratio (device space has 0,0 as the centre)
	Dimension2i winDimension = gfxInstance->getCurrentWindowSize();
	Vector2f origin = Vector2f(winDimension.w / 2, winDimension.h / 2);

	float offsetX = worldSpaceCoords.x - origin.x;
	float offsetY = (worldSpaceCoords.y - origin.y) * -1;

	// divide this by the values of the origin coords
	Vector2f resultCoords = Vector2f(offsetX / origin.x, offsetY / origin.y);

	return resultCoords;
}

void MyEngineSystem::drawTriangle2D(Vector2f pointA, Vector2f pointB, Vector2f pointC)
{
	// translate the world coords of the vertex into screen space coordinates
	Vector2f devicePointA = translateWorldSpaceToDeviceSpace(pointA);
	Vector2f devicePointB = translateWorldSpaceToDeviceSpace(pointB);
	Vector2f devicePointC = translateWorldSpaceToDeviceSpace(pointC);

	// array to add to the vertex buffer
	float deviceSpaceVertices[] =
	{
		devicePointA.x, devicePointA.y,
		devicePointB.x, devicePointB.y,
		devicePointC.x, devicePointC.y
	};
	// make this the active vertex buffer (GL_ARRAY_BUFFER is the memory address of the active buffer)
	glBufferData(GL_ARRAY_BUFFER, sizeof(deviceSpaceVertices), deviceSpaceVertices, GL_STATIC_DRAW);
	
	// send vertex data to the vertex shader
	// takes shader program and the attribute we want access to
	GLuint positionInput = glGetAttribLocation(myEngineShaderProg, "position");
	// takes the input attribute, the dimensions of the input vector, normalisation (convert to -1.0 to 1.0)
	// last params are stride and offset (amount of bytes of space between vertices and amount of bytes before first vertex)
	glVertexAttribPointer(positionInput, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(positionInput);

	glDrawArrays(GL_TRIANGLES, 0, 3);
	GLenum err = glGetError();

	std::cout << err << std::endl;

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