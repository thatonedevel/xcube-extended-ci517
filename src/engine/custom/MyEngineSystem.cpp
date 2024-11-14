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

MyEngineSystem::MyEngineSystem(std::shared_ptr<GraphicsEngine> gfx)
{
	// initialise vertex stream
	vertexStream = new std::vector<float>();
	// reserve space for 10000 vertices
	//std::cout << "Vector capacity: " << vertexStream->max_size() << std::endl;
	vertexStream->resize(3000000, 0);

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
	in vec3 position;
	
	void main()
	{
		// gl_Position is the shader output for where to display the vertices
		// vec4 is used to add data for the position. components can be passed individually or through a vec
		gl_Position = vec4(position, 1.0);
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
	delete vertexStream;
}

Vector3F MyEngineSystem::translateWorldSpaceToDeviceSpace(Vector3F worldSpaceCoords)
{
	// get the window size to determine ratio (device space has 0,0 as the centre)
	Dimension2i winDimension = gfxInstance->getCurrentWindowSize();
	Vector2f origin = Vector2f(winDimension.w / 2, winDimension.h / 2);

	float offsetX = worldSpaceCoords.getX() - origin.x;
	float offsetY = (worldSpaceCoords.getY() - origin.y) * -1;

	// divide this by the values of the origin coords
	Vector3F resultCoords = Vector3F(offsetX / origin.x, offsetY / origin.y, worldSpaceCoords.getZ());

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

void MyEngineSystem::drawMeshObjects(Mesh3D mesh, Vector3F position)
{
	size_t streamSize = 0;
	// purge vertex stream vector
	// reserve enough space for the mesh's vertices to appear in the vector multiple times
	// * 9 to get the amt of vertices needed multiplied by dimensions of position vector (3)

	for (int i = 0; i < vertexStream->size(); i++)
	{
		// set element at index i to 0. vector::clear causes issues with allocation
		(*vertexStream)[i] = 0;
	}

	// use GL_TRIANGLES for rendering faces
	// produces an vector of 3n but will work immediately for meshes not consisting of single triangle strips
	for (int faceIndex = 0; faceIndex < mesh.getFaceCount(); faceIndex++)
	{
		// check face can be added to vertex stream
		if (faceIndex + 8 < vertexStream->size())
		{
			Face3D currentFace = mesh.getFaceAtIndex(faceIndex);
			Vector3F coordA = translateWorldSpaceToDeviceSpace(mesh.getVertexCoordinate(currentFace.getVertexIndA()));
			Vector3F coordB = translateWorldSpaceToDeviceSpace(mesh.getVertexCoordinate(currentFace.getVertexIndB()));
			Vector3F coordC = translateWorldSpaceToDeviceSpace(mesh.getVertexCoordinate(currentFace.getVertexIndC()));

			// add the coordinates to the vertex stream
			(*vertexStream)[faceIndex] = coordA.getX();
			(*vertexStream)[faceIndex + 1] = coordA.getY();
			(*vertexStream)[faceIndex + 2] = coordA.getZ();
			
			(*vertexStream)[faceIndex + 3] = coordB.getX();
			(*vertexStream)[faceIndex + 4] = coordB.getY();
			(*vertexStream)[faceIndex + 5] = coordB.getZ();
			
			(*vertexStream)[faceIndex + 6] = coordC.getX();
			(*vertexStream)[faceIndex = 7] = coordC.getY();
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

	streamSize = vertexStream->size() * sizeof(float);

	// TODO: currently throws debug assertation failure (subscript out of range)
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertexStream->size(), vertexStream, GL_STATIC_DRAW);
	// get vertex attribute and enable it
	GLuint vertexPos = glGetAttribLocation(myEngineShaderProg, "position");
	glVertexAttribPointer(vertexPos, 3, GL_FLOAT, false, 0, 0);
	glEnableVertexArrayAttrib(vertexArrObj, vertexPos);

	// draw the mesh
	glDrawArrays(GL_TRIANGLES, 0, vertexStream->size());
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

Mesh3D::Mesh3D()
{
	// initialise mesh data
	faces = new std::vector<Face3D>();
	vertices = new std::vector<Vector3F>();
	normals = new std::vector<Vector3F>();
}

Mesh3D::Mesh3D(std::string path)
{
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