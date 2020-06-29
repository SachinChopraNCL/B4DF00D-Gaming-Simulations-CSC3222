#include "Balloon.h"
#include "TextureManager.h"
#include "GameSimsRenderer.h"

using namespace NCL;
using namespace CSC3222;

Balloon::Balloon(SimObject* player) : SimObject() {
	playerBound = true;
	texture = texManager->GetTexture("TL_Creatures.png");
	offset = Vector2(0,-5);
	objType = BALLOON; 
	_player = player;
	friction = 0.1;

}

Balloon::~Balloon() {

}

bool Balloon::UpdateObject(float dt) {
	timer += dt;
	if (playerBound == true && !detatched) {
		float diffX = _player->GetPosition().x - this->position.x;
		float diffY = _player->GetPosition().y - this->position.y;
		float distance = sqrt((diffY * diffY) + (diffX * diffX));
		if (_player->GetVelocity().x > 0 && diffX > 0)
			this->force = Vector2(distance * 1.5, 0);
		else if (_player->GetVelocity().x < 0 && diffX < 0)
			this->force = Vector2(-distance * 1.5, 0);
		else if (_player->GetVelocity().y > 0 && diffY > 0)
			this->force = Vector2(0, distance * 1.5);
		else if (_player->GetVelocity().y < 0 && diffY < 0)
			this->force = Vector2(0, -distance * 1.5);
		else
			this->force = Vector2(0,0);
	}
	
	if (timer > 5)
		detatched = true;

	if (detatched) {
		position.y -= 0.3;
		if (position.y < 0) {
			isAlive = false;
		}
	}

	return isAlive;
}

void Balloon::DrawObject(GameSimsRenderer& r) {
	Vector2 texPos	= Vector2(73, 99);
	Vector2 texSize = Vector2(14, 28);
	r.DrawTextureArea((OGLTexture*)texture, texPos, texSize, position);
}