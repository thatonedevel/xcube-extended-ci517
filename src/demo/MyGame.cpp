#include "MyGame.h"

MyGame::MyGame() : AbstractGame(), score(0), lives(3), numKeys(5), gameWon(false), box(5, 5, 30, 30) {
	// test string splitting for .obj parser
	// set z depth to 500
	mySystem->setZDepth(500);
	std::string testVal = "1/2/3";
	std::string testVertexCoords = "1.000000 1.000000 -1.000000";
	std::vector<std::string> testVec = {};
	std::vector<std::string> testVertexVec = {};

	// call function 
	testVec = splitString(testVal, '/');
	testVertexVec = splitString(testVertexCoords, ' ');

	std::cout << "test vector output:" << std::endl;
	std::cout << "index 0: " << testVec[0] << std::endl;
	std::cout << "index 1: " << testVec[1] << std::endl;
	std::cout << "index 2: " << testVec[2] << std::endl;

	mySystem->addCamera(Camera(Vector3F(0, 0, 0), 60, gfx->getCurrentWindowSize(), 5, 100));
	// test the mesh object
	currentMesh = new Mesh3D("res/cubeOfXCube.obj", Vector3F(0, 0, -10));
	// do somthing here for a breakpoint to check mesh data has been read in correctly
	std::cout << "Loaded mesh" << std::endl;
}

MyGame::~MyGame() {
	delete currentMesh;
}

void MyGame::handleKeyEvents() {

	// transformations
	if (eventSystem->isPressed(Key::A))
	{
		currentMesh->moveObject(Vector3F(0.5, 0, 0));
	}
	else if (eventSystem->isPressed(Key::D))
	{
		currentMesh->moveObject(Vector3F(-0.5, 0, 0));
	}

	if (eventSystem->isPressed(Key::S))
	{
		currentMesh->moveObject(Vector3F(0, -0.5, 0));
	}
	else if (eventSystem->isPressed(Key::W))
	{
		currentMesh->moveObject(Vector3F(0, 0.5, 0));
	}

	// rotation

	if (eventSystem->isPressed(Key::RIGHT))
	{
		currentMesh->rotateZAxis(10);
	}
	else if (eventSystem->isPressed(Key::LEFT))
	{
		currentMesh->rotateZAxis(-10);
	}

	// scale
	if (eventSystem->isPressed(Key::UP))
	{
		currentMesh->scaleMesh(Vector3F(1.1, 1.1, 1.1));
	}
	else if (eventSystem->isPressed(Key::DOWN))
	{
		currentMesh->scaleMesh(Vector3F(-1.1, -1.1, -1.1));
	}

	// easter egg
	if (eventSystem->isPressed(Key::SPACE))
	{
		if (!eeKeyOnLastFrame)
		{
			eeKeyOnLastFrame = true;
			gfx->showInfoMessageBox("Subsystem by Nora Dwelly, AD1352", "Mesh Rendering subsystem easeteregg");
		}
	}
	else
	{
		if (eeKeyOnLastFrame)
			eeKeyOnLastFrame = false;
	}
}

void MyGame::update() {

}

void MyGame::render() {
	//mySystem->drawMeshObjects(0, *currentMesh, GL_TRUE);
	mySystem->drawMeshesIn2D(0, *currentMesh);
}

void MyGame::renderUI() {

}