#include "BadRobot.h"
#include "TextureManager.h"
#include "GameSimsRenderer.h"
#include "../../Common/Window.h"
#include "Pathing.h"
using namespace NCL;
using namespace CSC3222;

Vector4 animFramesGreen[] = {
	Vector4(71,142,18,18),
	Vector4(102,142,20,18),
	Vector4(135,142,19,18),
	Vector4(166,142,19,18),
	Vector4(198,140,20,18)
};

Vector4 animFramesWhite[] = {
	Vector4(71,266,17,22),
	Vector4(104,265,19,22),
	Vector4(137,266,17,22),
	Vector4(168,266,17,22),
	Vector4(200,264,17,23)
};

BadRobot::BadRobot(Pathing* pather, BadFoodGame* currentGame):SimObject() {
	srand(time(NULL));
	_currentGame = currentGame;
	currentPather = pather;
	texture = texManager->GetTexture("TL_Creatures.png");
	animFrameCount	= 4;
	objType = ROBOT;
	friction = 1.2;
	int r = rand() % 2;

	if (r == 0) {
		type = RobotType::Green;
	}
	else {
		type = RobotType::White;
	}
}

BadRobot::~BadRobot() {

}

bool BadRobot::UpdateObject(float dt) {
	UpdateAnimFrame(dt);
	/* Distance between Player and Robot */
	Vector2 playerPosition = currentPlayer->GetPosition();
	Vector2 robotPosition = this->position;
	float   diffX = playerPosition.x - robotPosition.x;
	float   diffY = playerPosition.y - robotPosition.y;
    float   dist = std::sqrt(diffX * diffX + diffY * diffY);
	if (isStunned) {
		stunnedTimer += dt;
		velocity = Vector2(0, 0);
		if (stunnedTimer > 3) {
			isStunned = false;
			stunnedTimer = 0;
		}
	}
	if (!isStunned) {
		/* Protect */
		if (_currentGame->GetFoodCount() == 5) {
			currentState = State::TRANSITION;
			if (!protectMode && !(position.x > 350 && position.x < 450 && position.y > 95 && position.y < 140)) {
				protectMode = true;
				currentState = State::PROTECT;
			}
		}
		/* Patrol Mode*/
		if (dist > 80 && _currentGame->GetFoodCount() < 5) {
			currentState = State::PATROL;
		}
		/* Attack */
		if (dist <= 80) {
			currentState = State::ATTACK;
		}
		Pathing::Node robot;
		robot.x = floor((float)robotPosition.x / 16);
		robot.y = floor((float)robotPosition.y / 16);
		Pathing::Node dest;
		switch (currentState) {
		case(State::PATROL): {
			timeToWait += dt;
			if (timeToWait > 5) {
				velocity = Vector2(0, 0);
				timeToFire = 0;
				if (timeToWait > 5.5) {
					/* Finding a valid patrol point */
					tileIndex = 0;
					timeToWait = 0;
					int x = -1, y = -1;
					while (!currentPather->isValid(x, y)) {
						x = rand() % 30;
						y = rand() % 20;
					}
					dest.x = x;
					dest.y = y;
					currentPath = currentPather->findPath(robot, dest);
					tileMax = currentPath.size() - 1;
				}
			}
		}; break;
		case(State::ATTACK): {
			/* Finds player to attack */
			timeToFire += dt;
			timeToWait = 0;
			tileIndex = 0;
			dest.x = floor((float)currentPlayer->GetPosition().x / 16);
			dest.y = floor((float)currentPlayer->GetPosition().y / 16);
			currentPath = currentPather->findPath(robot, dest);
			tileMax = currentPath.size() - 1;
			/* Periodically fires laser */
			if (timeToFire > 2) {
				Vector2 bulletVelocity = position - currentPlayer->GetPosition();
				bulletVelocity.Normalise();
				_currentGame->SpawnLaser(position + Vector2(-bulletVelocity.x, -bulletVelocity.y) * 20, Vector2(-bulletVelocity.x, bulletVelocity.y) * 90);
				timeToFire = 0;
			}
			Separation(dt);
		}; break;
		case(State::PROTECT): {
			/* Move Robot to protection zone */
			tileIndex = 0;
			int x = -1, y = -1;
			while (!currentPather->isValid(x, y)) {
				x = rand() % 4 + 24;
				y = rand() % 3 + 5;
			}
			dest.x = x;
			dest.y = y;
			currentPath = currentPather->findPath(robot, dest);
			tileMax = currentPath.size() - 1;
		}; break;
		case(State::TRANSITION): {
			if (currentPath.size() == 0 || currentState == State::ATTACK) {
				protectMode = false;
			}
			Separation(dt);
		}; break;
		}
		/* Main pathfinding segement, moves robot towards next node */
		if (currentPath.size() > 0) {
			resetPath = false;
			Pathing::Node currentNode = currentPath.at(tileIndex);
			float tileX = currentNode.x * 16;
			float tileY = currentNode.y * 16;
			if (tileX <= position.x && position.x <= (tileX + 16) && tileY <= position.y && position.y <= (tileY + 16)) {
				tileIndex++;
				if (tileIndex + 1 > tileMax) {
					currentPath.clear();
					resetPath = true;
				}
			}
			if (tileIndex < tileMax && !resetPath) {
				float nextX = ((currentPath.at(tileIndex).x * 16) + 8) - position.x;
				float nextY = ((currentPath.at(tileIndex).y * 16) + 8) - position.y;
				distanceToNextNode = Vector2(nextX, nextY);
				distanceToNextNode.Normalise();
				AddForce(distanceToNextNode* Vector2(64, 64));

			}

		}
	}
	if (onGrass) {
		max_velocity = Vector2(12.8, 12.8);
	}
	else if (!onGrass) {
		max_velocity = Vector2(32, 32);
	}
	return isAlive;
}

/* Applying Flocking to seperate robots that are too close */
void BadRobot::Separation(float dt) {
	Vector2 dir;
	float threshold = 15; 
	for (BadRobot* robot : allRobots) {
		if (robot == this)
			continue;
		Vector2 vecDist = this->position - robot->position;
		float numDist = vecDist.Length();
		if (numDist > threshold)
			continue;
		float strength = 1.f - (numDist / threshold);
		dir += vecDist.Normalised() * strength;
	}

	position += dir * dt;
}

void BadRobot::DrawObject(GameSimsRenderer& r) {
	Vector2	texPos;
	Vector2 texSize;

	switch (type) {
		case RobotType::Green:{
			texPos	= Vector2(animFramesGreen[currentanimFrame].x, animFramesGreen[currentanimFrame].y);
			texSize = Vector2(animFramesGreen[currentanimFrame].z, animFramesGreen[currentanimFrame].w);
		}break;
		case RobotType::White:{
			texPos	= Vector2(animFramesWhite[currentanimFrame].x, animFramesWhite[currentanimFrame].y);
			texSize = Vector2(animFramesWhite[currentanimFrame].z, animFramesWhite[currentanimFrame].w);
		}break;
	}

	r.DrawTextureArea((OGLTexture*)texture, texPos, texSize, position, false);
}