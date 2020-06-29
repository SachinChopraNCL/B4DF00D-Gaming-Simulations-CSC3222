#include "SimObject.h"
#include "CollisionVolume.h"

using namespace NCL;
using namespace CSC3222;

TextureManager*  SimObject::texManager = nullptr;
BadFoodGame* SimObject::game = nullptr;

SimObject::SimObject()	{
	texture		= nullptr;
	collider	= nullptr;

	animFrameCount	= 1;
	currentanimFrame	= 0;
	frameTime			= 0;
}

SimObject::~SimObject()	{
	delete collider;
}

void SimObject::InitObjects(BadFoodGame* game, TextureManager* texManager) {
	SimObject::game			= game;
	SimObject::texManager	= texManager;
}

void SimObject::UpdateAnimFrame(float dt) {
	frameTime -= dt;

	if (frameTime < 0) {
		currentanimFrame = (currentanimFrame + 1) % animFrameCount;
		frameTime += 1.0f / 12.0f;
	}
}