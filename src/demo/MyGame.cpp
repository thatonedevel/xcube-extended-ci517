#include "MyGame.h"

MyGame::MyGame() : AbstractGame(), score(0), lives(3), numKeys(5), gameWon(false), box(5, 5, 30, 30) {

}

MyGame::~MyGame() {

}

void MyGame::handleKeyEvents() {

}

void MyGame::update() {

}

void MyGame::render() {
	mySystem->drawTriangle2D(Vector2f(200, 400), Vector2f(400, 200), Vector2f(600, 400));
}

void MyGame::renderUI() {

}