#include "Food.h"
#include "TextureManager.h"
#include "GameSimsRenderer.h"

using namespace NCL;
using namespace CSC3222;

Vector4 foodFrames[] = {
	Vector4(0,0,15,16), //Apple
	Vector4(17,0,14,16),//Can
	Vector4(31,0,18,16),//Pizza
	Vector4(49,0,15,16),//Sundae
	Vector4(64,0,16,16) //Bread
};

Food::Food() : SimObject() {
	texture = texManager->GetTexture("food_items16x16.png");
	objType = FOOD;
	foodItem = rand() % 5;
}

Food::~Food() {

}

bool Food::UpdateObject(float dt) {
	timeToDie += dt;
	if (timeToDie > 12) {
		isAlive = false;
	}
	return isAlive;
}

void Food::DrawObject(GameSimsRenderer& r) {
	Vector2 texPos	= Vector2(foodFrames[currentanimFrame + foodItem].x, foodFrames[currentanimFrame + foodItem].y);
	Vector2 texSize = Vector2(foodFrames[currentanimFrame + foodItem ].z, foodFrames[currentanimFrame + foodItem].w);
	r.DrawTextureArea((OGLTexture*)texture, texPos, texSize, position);
}