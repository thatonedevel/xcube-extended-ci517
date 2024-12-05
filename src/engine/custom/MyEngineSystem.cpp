#include "MyEngineSystem.h"

std::vector<std::string> splitString(std::string input, char splitDelimeter)
{
	int componentStartIndex = 0;
	int componentEndIndex = 0;

	std::vector<std::string> components = {};

	for (int charIndex = 0; charIndex < input.length(); charIndex++)
	{
		componentEndIndex = charIndex;

		if (input[charIndex] == splitDelimeter)
		{
			// vector syntax https://www.w3schools.com/cpp/cpp_vectors.asp
			components.push_back(input.substr(componentStartIndex, componentEndIndex - componentStartIndex));
			componentStartIndex = componentEndIndex + 1;
		}
		else if (charIndex == input.length() - 1)
		{
			// push last part of string to component vector
			components.push_back(input.substr(componentStartIndex, input.length() - componentStartIndex));
		}
	}

	return components;
}

std::string joinString(std::string joinedStrs[], std::string joinSeq="")
{
	/* 
		Joins the array joinedStrs into a single string,
		with each entry of joinedStrs seperated
		in the final string by the string joinSeq
	*/
	std::string result = "";

	for (int i = 0; i < sizeof((*joinedStrs)) / sizeof(std::string); i++)
	{
		result += joinedStrs[i] + joinSeq;
	}

	return result;
}

MyEngineSystem::MyEngineSystem(std::shared_ptr<GraphicsEngine> gfx)
{
	GLint compileLogLen = 0;
	GLint compileStatus = 0;
	std::string tmpSource = "";
	// initialise vertex stream
	vertexStream = new std::vector<float>();
	renderCameras = new std::vector<Camera>();
	// reserve space for 10000 vertices
	//std::cout << "Vector capacity: " << vertexStream->max_size() << std::endl;
	vertexStream->resize(3000000, 0);
	std::cout << "Vertex stream size: " << vertexStream->size() << std::endl;
	fragmentShaderSource = R"glsl(
#version 150 core

out vec4 outColor;

void main()
{
    outColor = vec4(1.0, 1.0, 1.0, 1.0);
})glsl";

	gfxInstance = gfx;
	// load in the shaders
	std::ifstream vertShaderFile = std::ifstream("res/xcubeVertShader.vs");
	if (!vertShaderFile.fail())
	{
		std::cout << "Vertex Shader Loaded from external file\n";
	}
	else
	{
		std::cout << "Vertex Shader loading failed\n";
	}
	std::string shaderLine = "";

	while (std::getline(vertShaderFile, shaderLine))
	{
		tmpSource += shaderLine + "\n";
	}
	vertexShaderSource = tmpSource.c_str();
	vertShaderFile.close();

	// create buffer objects
	std::cout << "Creating Vertex Buffer Object...\n";
	glGenBuffers(1, &myEngineSysVBO);
	std::cout << "";
	glGenVertexArrays(1, &vertexArrObj);

	std::cout << "Binding Vertex Array Object & Vertex Buffer Object...\n";
	glBindBuffer(GL_ARRAY_BUFFER, myEngineSysVBO);
	glBindVertexArray(vertexArrObj);

	// init shaders
	std::cout << "Creating Shader Program...\n";
	myEngineShaderProg = glCreateProgram();

	std::cout << "Creating Vertex Shader...\n";
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	std::cout << "Vertex shader initialised with code " << glGetError() << "\n";
	
	std::cout << "Creating Fragment Shader...\n";
	fragShader = glCreateShader(GL_FRAGMENT_SHADER);
	std::cout << "Fragment Shader initialised with code " << glGetError() << "\n";

	// inject shader source code
	std::cout << "Adding Vertex Shader source...\n";
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	std::cout << "Shader source added with code " << glGetError() << "\n";
	
	std::cout << "Adding Fragment Shader source...\n";
	glShaderSource(fragShader, 1, &fragmentShaderSource, NULL);
	std::cout << "Shader source added with code " << glGetError() << "\n";

	std::cout << "Compiling Vertex Shader...\n";
	glCompileShader(vertexShader);
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &compileStatus);

	if (compileStatus == GL_TRUE) 
	{ 
		std::cout << "Vertex Shader compiled successfully\n";
	}

	// fragment shader
	std::cout << "Compiling Fragment Shader...\n";
	glCompileShader(fragShader);
	glGetShaderiv(fragShader, GL_COMPILE_STATUS, &compileStatus);
	if (compileStatus == GL_TRUE) std::cout << "Fragment Shader compiled successfully\n";
	else std::cout << "Compilation error for Fragment Shader\n";

	// add shaders to program, then link it and tell opengl to use this program
	std::cout << "Adding shaders to engine program...\n";
	glAttachShader(myEngineShaderProg, vertexShader);
	std::cout << "Vertex Shader added to program with code " << glGetError() << "\n";
	glAttachShader(myEngineShaderProg, fragShader);
	std::cout << "Fragment Shader added to program with code " << glGetError() << "\n";

	std::cout << "Linking Shader Program...\n";
	glLinkProgram(myEngineShaderProg);
	std::cout << "Shader Program linked with code " << glGetError() << "\n";
	glUseProgram(myEngineShaderProg);
}

MyEngineSystem::~MyEngineSystem()
{
	// subsystem destructor
	delete vertexStream;
	delete renderCameras;
}

void MyEngineSystem::addCamera(Camera cam) { renderCameras->push_back(cam); };
Camera MyEngineSystem::getCamera(int camIndex) { return (*renderCameras)[camIndex]; };

Camera::Camera(Vector3F position, float fov, Dimension2i winDimensions, float nearPlane, float farPlane)
{
	// creates a view frustum as this is no longer implemented in opengl
	pos = position;
	fieldOfView = fov;
	// set up projection matrix for the camera
	cameraMat.initCameraTransform(pos, Vector3F(0, -1, 0));
	//cameraMat.setPerspectiveProjection(fov, winDimensions.w, winDimensions.h, nearPlane, farPlane);
	// set up pixel positions for frustum planes
	near = nearPlane;
	far = farPlane;
	left = 0;
	top = 0;
	bottom = winDimensions.h;
	right = winDimensions.w;
}

Vector3F MyEngineSystem::translateWorldSpaceToDeviceSpace(Vector3F worldSpaceCoords)
{
	// get the window size to determine ratio (device space has 0,0 as the centre)
	Dimension2i winDimension = gfxInstance->getCurrentWindowSize();
	Vector2f origin = Vector2f(winDimension.w / 2, winDimension.h / 2);

	// z origin = 0 at screen centre.

	float offsetX = worldSpaceCoords.getX() - origin.x;
	float offsetY = (worldSpaceCoords.getY() - origin.y) * -1;
	float offsetZ = worldSpaceCoords.getZ() - zDepth / 2; // zDepth / 2 = z axis origin

	// divide this by the values of the origin coords

	Vector3F resultCoords = Vector3F(offsetX / origin.x, offsetY / origin.y, worldSpaceCoords.getZ() / (zDepth / 2));

	return resultCoords;
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

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);
	//GLenum err = glGetError();

	//std::cout << err << std::endl;

}

void MyEngineSystem::drawMeshObjects(int camIndex, Mesh3D mesh)
{
	// purge vertex stream vector
	// reserve enough space for the mesh's vertices to appear in the vector multiple times
	// * 9 to get the amt of vertices needed multiplied by dimensions of position vector (3)
	GLfloat matrix[16] = {};
	
	int ind = 0;
	for (int row = 0; row < 4; row++)
	{
		for (int col = 0; col < 4; col++)
		{
			matrix[ind] = (*renderCameras)[camIndex].cameraMat.m[row][col];
			ind++;
		}
	}

	for (int i = 0; i < vertexStream->size(); i++)
	{
		// set element at index i to 0. vector::clear causes issues with allocation
		(*vertexStream)[i] = 0;
	}

	std::cout << "Faces to render: " << mesh.getFaceCount() << std::endl;

	// use GL_TRIANGLES for rendering faces
	// produces an vector of 3n but will work immediately for meshes not consisting of single triangle strips
	for (int faceIndex = 0; faceIndex < mesh.getFaceCount(); faceIndex++)
	{
		// check face can be added to vertex stream
		if (faceIndex + 8 < vertexStream->size())
		{
			Face3D currentFace = mesh.getFaceAtIndex(faceIndex);
			Vector3F coordA = mesh.getVertexCoordinate(currentFace.getVertexIndA() - 1);
			Vector3F coordB = mesh.getVertexCoordinate(currentFace.getVertexIndB() - 1);
			Vector3F coordC = mesh.getVertexCoordinate(currentFace.getVertexIndC() - 1);

			// add the coordinates to the vertex stream
			(*vertexStream)[faceIndex] = coordA.getX();
			(*vertexStream)[faceIndex + 1] = coordA.getY();
			(*vertexStream)[faceIndex + 2] = coordA.getZ();
			
			(*vertexStream)[faceIndex + 3] = coordB.getX();
			(*vertexStream)[faceIndex + 4] = coordB.getY();
			(*vertexStream)[faceIndex + 5] = coordB.getZ();
			
			(*vertexStream)[faceIndex + 6] = coordC.getX();
			(*vertexStream)[faceIndex + 7] = coordC.getY();
			(*vertexStream)[faceIndex + 8] = coordC.getZ();

			std::cout << "Added face to vertex stream\n";
		}
		else
		{
			std::cout << "Insufficient space in vertex stream for mesh of " << mesh.getFaceCount() << " tris" << std::endl;
		}
		// yes i know this is terrible but i can't think of anything else OXKJHGKCJH
	}

	std::cout << "Vertex stream filled\n";

	// TODO: currently throws debug assertation failure (subscript out of range)
	glBufferData(GL_ARRAY_BUFFER, sizeof(*vertexStream), vertexStream, GL_STATIC_DRAW);
	// get vertex attribute and enable it
	GLuint vertexPos = glGetAttribLocation(myEngineShaderProg, "worldSpacePosition"); // error ?
	//GLuint matrixPos = glGetUniformLocation(myEngineShaderProg, "projection");
	GLuint leftFrustumPos = glGetUniformLocation(myEngineShaderProg, "frustumLeft");
	GLuint rightFrustumPos = glGetUniformLocation(myEngineShaderProg, "frustumRight");
	GLuint topFrustumPos = glGetUniformLocation(myEngineShaderProg, "frustumTop");
	GLuint bottomFrustumPos = glGetUniformLocation(myEngineShaderProg, "frustumBottom");
	GLuint nearPlanePos = glGetUniformLocation(myEngineShaderProg, "nearPlaneDist");
	GLuint farPlanePos = glGetUniformLocation(myEngineShaderProg, "farPlaneDist");

	GLuint cameraSpacePos = glGetUniformLocation(myEngineShaderProg, "cameraPos");
	GLuint cameraFOVPos = glGetUniformLocation(myEngineShaderProg, "cameraFOV");

	glVertexAttribPointer(vertexPos, 3, GL_FLOAT, false, 0, 0);
	//glUniformMatrix4fv(matrixPos, 1, GL_FALSE, matrix);

	// upload camera values to shader
	glUniform1f(leftFrustumPos, (*renderCameras)[camIndex].getLeftPlane());
	glUniform1f(rightFrustumPos, (*renderCameras)[camIndex].getRightPlane());
	glUniform1f(topFrustumPos, (*renderCameras)[camIndex].getTopPlane());
	glUniform1f(bottomFrustumPos, (*renderCameras)[camIndex].getBottomPlane());
	glUniform1f(nearPlanePos, (*renderCameras)[camIndex].getNearPlane());
	glUniform1f(farPlanePos, (*renderCameras)[camIndex].getFarPlane());

	glEnableVertexArrayAttrib(vertexArrObj, vertexPos);

	// draw the mesh
	std::cout << "Drawing Mesh" << std::endl;
	glDrawArrays(GL_TRIANGLES, 0, mesh.getFaceCount() * 9);
	std::cout << glGetError() << std::endl;
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

float Vector3F::dot(Vector3F other)
{
	// dot product
	// lengyel. e, mathematics for game programming and computer graphics third edition (2012) course technology ptr, boston (massachussets)
	return (x * other.getX()) + (y * other.getY()) + (z * other.getZ());
}

Vector3F Vector3F::cross(Vector3F other)
{
	return Vector3F(
		(y * other.getZ()) - (z * other.getY()),
		(z * other.getX()) - (other.getZ() - x),
		(x * other.getY()) - (other.getX() - y)
	);
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

Mesh3D::Mesh3D()
{
	// initialise mesh data
	faces = new std::vector<Face3D>();
	vertices = new std::vector<Vector3F>();
	normals = new std::vector<Vector3F>();
}

Mesh3D::Mesh3D(std::string path, Vector3F position)
{
	originPosition = position;
	faces = new std::vector<Face3D>();
	vertices = new std::vector<Vector3F>();
	normals = new std::vector<Vector3F>();

	std::ifstream modelFile;
	modelFile.open(path);
	// check if it worked
	if (!modelFile.fail())
		std::cout << "Successfully opened file " << path << std::endl;
	std::string objFileLine = "";
	// read the data in from the obj file
	while (std::getline(modelFile, objFileLine))
	{
		// check if the line is a null terminator - taken from http://courses.washington.edu/css342/timots/Notes/eof.html
		// also refer to https://docs.blender.org/manual/en/2.80/addons/io_scene_obj.html
		
		// determine the content of the line
		std::cout << "Current line: " << objFileLine << std::endl;

		if (objFileLine.substr(0, 2) == "v ")
		{
			// vertex, will be followed by a space
			// seperate the line into its vertex coordinates
			std::vector<std::string> coords = splitString(objFileLine.substr(2, objFileLine.length() - 3), ' ');
			// parse these as floating point values
			Vector3F vertex = Vector3F(std::stof(coords[0]), std::stof(coords[1]), std::stof(coords[2]));
			// add this to the collection of vertices
			vertices->push_back(vertex);
			std::cout << "Added vertex" << std::endl;
		}
		else if (objFileLine.substr(0, 2) == "vn")
		{
			// vertex normals
			std::vector<std::string> normalStr = splitString(objFileLine.substr(3, objFileLine.length() - 3), ' ');
			Vector3F normal = Vector3F(std::stof(normalStr[0]), std::stof(normalStr[1]), std::stof(normalStr[2]));
			normals->push_back(normal);
			std::cout << "Added normal" << std::endl;
		}
		else if (objFileLine.substr(0, 2) == "f ")
		{
			// faces
			std::vector<std::string> faceData = splitString(objFileLine.substr(2, objFileLine.length() - 3), ' ');
			std::vector<std::string> faceA = splitString(faceData[0], '/');
			std::vector<std::string> faceB = splitString(faceData[1], '/');
			std::vector<std::string> faceC = splitString(faceData[2], '/');

			faces->push_back(Face3D(std::stoi(faceA[0]), std::stoi(faceB[0]), std::stoi(faceC[0]), std::stoi(faceA[2])));
			std::cout << "Added face" << std::endl;
		}
		
	}

	modelFile.close();
	std::cout << "Closed obj file " << path << std::endl;
}

Vector3F Mesh3D::getFaceNormal(int faceIndex)
{
	Face3D reqFace = (*faces)[faceIndex];
	Vector3F normal = (*normals)[reqFace.getNormalIndex() - 1];
	return normal;
}

std::vector<Vector3F> Mesh3D::evaluateYAxisRotation()
{
	std::vector<Vector3F> translatedVertices = {};
	float angle = eulerRotation.getY();
	float hypotenuse = 0;
	
	for (Vector3F vertex : *vertices)
	{
		// note: cmath trig functions use radians: https://cplusplus.com/reference/cmath/sin/
		hypotenuse = sqrt(pow(vertex.getX(), 2) + pow(vertex.getZ(), 2));
		Vector3F vertexTranslation(cos(angle) * hypotenuse * DEG2RAD, 0, sin(angle) * hypotenuse * DEG2RAD);

	}

	return {};
}

Face3D::Face3D(int vertexA, int vertexB, int vertexC, int normal)
{
	vertexIndA = vertexA;
	vertexIndB = vertexB;
	vertexIndC = vertexC;
	normalIndex = normal;
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