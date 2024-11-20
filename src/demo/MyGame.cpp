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

	// test the mesh object
	currentMesh = new Mesh3D("res/cubeOfXCube.obj", Vector3F(400, 300, 100));
	// do somthing here for a breakpoint to check mesh data has been read in correctly
	std::cout << "Loaded mesh" << std::endl;
}

MyGame::~MyGame() {
	delete currentMesh;
}

void MyGame::handleKeyEvents() {

}

void MyGame::update() {

}

void MyGame::render() {
	mySystem->drawMeshObjects(*currentMesh);
}

void MyGame::renderUI() {

}