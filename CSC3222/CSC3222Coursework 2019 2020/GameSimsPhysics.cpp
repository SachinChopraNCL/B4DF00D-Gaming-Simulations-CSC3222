#include "GameSimsPhysics.h"
#include "RigidBody.h"
#include "CollisionVolume.h"
#include "../../Common/Vector2.h"
#include "Balloon.h"
#include "PlayerCharacter.h"
#include <map>
#include "Laser.h"
#include "BadRobot.h"




using namespace NCL;
using namespace CSC3222;


GameSimsPhysics::GameSimsPhysics(BadFoodGame* currentGame)	{
	game = currentGame;
}

GameSimsPhysics::~GameSimsPhysics()	{

}

void GameSimsPhysics::Update(float dt) {
	Integration(dt);
}

void GameSimsPhysics::AddRigidBody(RigidBody* b) {
	allBodies.emplace_back(b);
}

void GameSimsPhysics::RemoveRigidBody(RigidBody* b) {
	auto at = std::find(allBodies.begin(), allBodies.end(), b);

	if (at != allBodies.end()) {
		//maybe delete too?
		allBodies.erase(at);
	}
}

void GameSimsPhysics::AddCollider(CollisionVolume* c) {
	allColliders.emplace_back(c);
}

void GameSimsPhysics::RemoveCollider(CollisionVolume* c) {
	auto at = std::find(allColliders.begin(), allColliders.end(), c);

	if (at != allColliders.end()) {
		//maybe delete too?
		allColliders.erase(at);
	}
}

std::vector<CollisionVolume*> GameSimsPhysics::getVolumes()
{
	return allColliders;
}

void GameSimsPhysics::Integration(float dt) {
	/* Solve Eulers Integration on all rigid bodies. A = Mi x F, V+1 = V + (A * dt), P+1 = P + V+1 * dt */
	if (reset) {
		totalDT = 0;
		reset = false;
	}
	totalDT += dt;

	/* Remove NULL colliders / Dead colliders */
	for (RigidBody* body : allBodies) {
		SimObject* obj = (SimObject*)body;
		if (obj && !obj->isAlive) {
			RemoveCollider(obj->GetCollider());
			RemoveRigidBody(obj);
		}
	}
	
	for (RigidBody* body : allBodies) {
		Vector2 acc = body->force * body->inverseMass;
		body->velocity += acc * dt;

		/* Restricting Max_velocity */
		if (body->velocity.x > body->max_velocity.x)
			body->velocity.x = body->max_velocity.x;
		if (body->velocity.x < -(body->max_velocity.x))
			body->velocity.x = -(body->max_velocity.x);
		if (body->velocity.y > body->max_velocity.y)
			body->velocity.y = body->max_velocity.y;
		if (body->velocity.y < -(body->max_velocity.y))
			body->velocity.y = -(body->max_velocity.y);
	}



	/* Collision Detection + Resolution */
	if (totalDT > 0.5)
		CollisionDetection(dt);
	
	for (RigidBody* body : allBodies) {
		SimObject* obj = (SimObject*)body;
		/* Applying friction */
		if (body->force.x == 0) {
			if (body->velocity.x > 0) {
				body->velocity.x = body->velocity.x - body->friction;
				if (body->velocity.x < 0.5)
					body->velocity.x = 0;
			}
			else if (body->velocity.x < 0) {
				body->velocity.x = body->velocity.x + body->friction;
				if (body->velocity.x > -0.5)
					body->velocity.x = 0;
			}
		}
		if (body->force.y == 0) {
			if (body->velocity.y > 0) {
				body->velocity.y = body->velocity.y - body->friction;
				if (body->velocity.y < 0.5)
					body->velocity.y = 0;
			}
			else if (body->velocity.y < 0) {
				body->velocity.y = body->velocity.y + body->friction;
				if (body->velocity.y > -0.5)
					body->velocity.y = 0;
			}

		}

		/* Positional changes */
		body->position += (body->velocity * dt);

		if (body->_isStatic == false) {
			obj->GetCollider()->setPosition(obj->GetPosition() + obj->offset);
		}

		body->force = Vector2(0, 0);

	}

}

/* Sort Method */
bool sortColliders(CollisionVolume* c1, CollisionVolume* c2) {
	return c1->extent < c2->extent;
}

/* Broadphase sorting collisions */
vector<CollisionVolume*> SortAllColliders(vector<CollisionVolume*>	allColliders) {
	std::vector<CollisionVolume*>  collisionList;
	std::vector<CollisionVolume*> collisionPairs;
	/* Update Extents of the Colliders*/
	for (CollisionVolume* collider : allColliders) {
		if (collider->_volumeType == SQUARE)
			collider->extent = collider->_position.x - collider->_halfSize.x;
		else
			collider->extent = collider->_position.x - collider->_radius;

	}
	/* Sort Colliders on the x axis */
	std::sort(allColliders.begin(), allColliders.end(), &sortColliders);

	collisionList.push_back(allColliders.at(0));

	/* Sweep and Prune */
	for (auto i = 1; i < allColliders.size(); i++) {
		CollisionVolume* currentCollider = allColliders.at(i);
		for (auto j = 0; j < collisionList.size(); j++) {
			bool addPair = false;
			bool removeElement = true; 
			if (collisionList.at(j)->_volumeType == SQUARE) {
				if ((currentCollider->extent < collisionList.at(j)->_position.x + collisionList.at(j)->_halfSize.x)) {
					addPair = true;
					removeElement = false;
				}
			}
			else if (collisionList.at(j)->_volumeType == CIRCLE) {
				if ((currentCollider->extent < collisionList.at(j)->_position.x + collisionList.at(j)->_radius)) {
					addPair = true;
					removeElement = false;
				}
			}

			SimObject* c1Object = (SimObject*)currentCollider->_collisionObj;
			SimObject* c2Object = (SimObject*)collisionList.at(j)->_collisionObj;

			/* Exludes collisions between non-static members*/
			if (addPair  && (c1Object || c2Object)) {
				collisionPairs.push_back(currentCollider);
				collisionPairs.push_back(collisionList.at(j));
			}
			
		}
		
		collisionList.push_back(currentCollider);
	}

	return collisionPairs;

}

bool AABBCol(CollisionVolume* currentCollider, CollisionVolume* testCollider) {
	return (abs(currentCollider->_position.x - testCollider->_position.x) * 2 < (currentCollider->_halfSize.x * 2 + testCollider->_halfSize.x * 2)) &&
		(abs(currentCollider->_position.y - testCollider->_position.y) * 2 < (currentCollider->_halfSize.y * 2 + testCollider->_halfSize.y * 2));
}

bool CircleCircleCol(CollisionVolume* currentCollider, CollisionVolume* testCollider) {
	return (pow(abs(currentCollider->_position.x - testCollider->_position.x), 2) + pow(abs(currentCollider->_position.y - testCollider->_position.y), 2) <= pow(currentCollider->_radius + testCollider->_radius, 2));
}


bool AABBCircleCol(CollisionVolume* currentCollider, CollisionVolume* testCollider){
	CollisionVolume* squareCollider;
	CollisionVolume* circleCollider;
	if (currentCollider->_volumeType == SQUARE) {
		squareCollider = currentCollider;
		circleCollider = testCollider;
	}
	else {
		squareCollider = testCollider;
		circleCollider = currentCollider;
	}

	float distX = abs(circleCollider->_position.x - squareCollider->_position.x);
	float distY = abs(circleCollider->_position.y - squareCollider->_position.y);

	if (distX > (squareCollider->_halfSize.x + circleCollider->_radius))
		 return false;
	else if (distY > (squareCollider->_halfSize.y + circleCollider->_radius))
		 return  false;
	else if (distX <= (squareCollider->_halfSize.x))
		 return true;
	else if (distY <= (squareCollider->_halfSize.y))
		 return true;
	else
		return (distX - (squareCollider->_halfSize.x) * (distX - (squareCollider->_halfSize.x)) + (distY - (squareCollider->_halfSize.y) * (distY - (squareCollider->_halfSize.y)) < circleCollider->_radius * circleCollider->_radius));
}

void GameSimsPhysics::CollisionDetection(float dt) {
	/* Main Collision Loop - Narrow Phase*/
	int playerCount = 0;
	bool collision = false;
	vector<CollisionVolume*> collisionPairs = SortAllColliders(allColliders);
	if (collisionPairs.size() >= 2) {
		for (auto i = 0; i < collisionPairs.size() - 1; i += 2) {
			CollisionVolume* c1 = collisionPairs.at(i);
			CollisionVolume* c2 = collisionPairs.at(i + 1);
			
			SimObject* c1Object = (SimObject*)c1->_collisionObj;
			SimObject* c2Object = (SimObject*)c2->_collisionObj;
			
			/* AABB Collisions */
			if (c1->_volumeType == SQUARE && c2->_volumeType == SQUARE) {
				collision = AABBCol(c1, c2);
			}
			/* Circle Circle Collisions*/
			else if (c1->_volumeType == CIRCLE && c2->_volumeType == CIRCLE) {
				collision = CircleCircleCol(c1, c2);
			}
			else {
				collision = AABBCircleCol(c1, c2);
			}
			/* Collision Resolution */
			if (collision) {
				/* Order of Parameter matters for resolution to reduce combinations of checks */
				if (c1Object) {
					ResolveCollision(c1, c2);
				}
				else if (!c1Object && c2Object) {
					ResolveCollision(c2, c1);
				}
			}
		}
	}
}

/* Helper Function */
float clamp(float value, float minValue, float maxValue) {
	value = std::min(value, maxValue);
	value = std::max(value, minValue);
	return value;
}

void AABBImpulseResolution(CollisionVolume* currentCollider, CollisionVolume* testCollider) {
	SimObject* currentObject = (SimObject*)currentCollider->_collisionObj;
	SimObject* testObject = (SimObject*)testCollider->_collisionObj;
	auto nearX = clamp(currentCollider->_position.x, testCollider->_position.x - testCollider->_halfSize.x, testCollider->_position.x + testCollider->_halfSize.x);
	auto nearY = clamp(currentCollider->_position.y, testCollider->_position.y - testCollider->_halfSize.y, testCollider->_position.y + testCollider->_halfSize.y);
	Vector2 collisionNormal = Vector2(currentCollider->_position.x - nearX, currentCollider->_position.y - nearY);
	collisionNormal.Normalise();
	/*Calculate Penetration Distance*/
	float distX = currentCollider->_halfSize.x + testCollider->_halfSize.x - (abs(currentCollider->_position.x - testCollider->_position.x));
	float distY = currentCollider->_halfSize.y + testCollider->_halfSize.y - (abs(currentCollider->_position.y - testCollider->_position.y));
	
	Vector2 displacement;
	Vector2 position;
	Vector2 currentPosition;
	Vector2 currentVelocity;
	
	/* If Both Objects are valid simulation objects, then apply impulse to both*/
	if (testObject && currentObject) {
		
		displacement = collisionNormal * Vector2(distX, distY);
		
		position = currentObject->GetPosition();
		currentPosition = position + displacement;
		currentObject->SetPosition(currentPosition);
		
		/* TestObject Positional calculation */
		position = testObject->GetPosition();
	    currentPosition = position - displacement;
		testObject->SetPosition(currentPosition);
		
		float e = 1;
		float conservedMomentum;
		currentVelocity = testObject->GetVelocity() - currentObject->GetVelocity();
		conservedMomentum = (float)(-(1 + e) * Vector2::Dot(currentVelocity, collisionNormal)) / 5;
		currentObject->AddImpulse(currentObject->GetVelocity() - (collisionNormal * conservedMomentum * 20));
		testObject->AddImpulse(testObject->GetVelocity() + (collisionNormal * conservedMomentum * 20));
	}
	/* If just collided with a wall */
	else if(currentObject) {
		collisionNormal.Normalise();
		displacement = collisionNormal * Vector2(distX, distY);
		position = currentObject->GetPosition();
		currentPosition = position + displacement;
		currentObject->SetPosition(currentPosition);
		currentVelocity = currentObject->GetVelocity();
		currentObject->AddImpulse(currentObject->GetVelocity() + (collisionNormal * 300));
	}


}


void CircleAABBImpulseResolution(CollisionVolume* currentCollider, CollisionVolume* testCollider) {
	SimObject* currentObject = (SimObject*)currentCollider->_collisionObj;
	SimObject* testObject = (SimObject*)testCollider->_collisionObj;
	/* Clamping values then working out collision Normal */
	auto nearX = clamp(currentCollider->_position.x, testCollider->_position.x - testCollider->_halfSize.x, testCollider->_position.x + testCollider->_halfSize.x);
	auto nearY = clamp(currentCollider->_position.y, testCollider->_position.y - testCollider->_halfSize.y, testCollider->_position.y + testCollider->_halfSize.y);
	Vector2 dist = Vector2(currentCollider->_position.x - nearX, currentCollider->_position.y - nearY);
	/*Directly move collider to stop further collision detection*/
	Vector2 collisionNormal = dist.Normalised();
	float displacement = currentCollider->_radius - dist.Length();
	Vector2 position = currentObject->GetPosition();
	Vector2 newPosition = position + (collisionNormal * displacement);
	currentObject->SetPosition(newPosition);
	/* Calculate Impulse for collision */
	Vector2 currentVelocity = currentObject->GetVelocity();
	float e = 1;
	float conservedMomentum = (float)(-(1 + e) * Vector2::Dot(currentVelocity, collisionNormal)) / 1.5;
	Vector2 newVelocity = currentVelocity + (collisionNormal * conservedMomentum * 1.5);
	currentObject->AddImpulse(newVelocity);
}

void GameSimsPhysics::ResolveCollision(CollisionVolume* currentCollider, CollisionVolume* testCollider) {
	SimObject* currentObject = (SimObject*)currentCollider->_collisionObj;
	SimObject* testObject = (SimObject*)testCollider->_collisionObj;
	/* Interactions with items */
	if (currentObject && testObject && currentObject->objType != ROBOT && testObject->objType != ROBOT) {
		if (currentObject->objType == PLAYER) {
			if (testObject->objType == FOOD) {
				game->IncrFoodCount();
				testObject->isAlive = false;
				RemoveCollider(testCollider);
				RemoveRigidBody(testObject);
			}
			else if (testObject->objType == COIN) {
				game->IncrCoins();
				testObject->isAlive = false;
				RemoveCollider(testCollider);
				RemoveRigidBody(testObject);
			}
			else if (testObject->objType == LASER) {
				/* Need to check if the player is invunerable due to balloon */
				PlayerCharacter* currentPlayer = (PlayerCharacter*)currentObject;
				if (!currentPlayer->isInvunerable) {
					game->DecrLives();
					testObject->isAlive = false;
					RemoveCollider(testCollider);
					RemoveRigidBody(testObject);
				}
			}
		}
	}
   
	if (currentObject && currentObject->objType == LASER && !testObject) {
		CircleAABBImpulseResolution(currentCollider, testCollider);
		Laser* currentLaser = (Laser*)currentObject;
		currentLaser->bounceCount++;
		if (currentLaser->bounceCount == 5) {
			currentObject->isAlive = false;
			RemoveCollider(currentCollider);
			RemoveRigidBody(currentObject);
		}
	}
	/* Interaction between robot and laser, i.e. stunned effect */
    if (currentObject && testObject) {
		if ((currentObject->objType == LASER && testObject->objType == ROBOT)) {
			BadRobot* currentRobot = (BadRobot*)testObject;
			currentRobot->isStunned = true;
			currentObject->isAlive = false;
			RemoveCollider(currentCollider);
			RemoveRigidBody(currentObject);
		}
		else if ((currentObject->objType == ROBOT && testObject->objType == LASER)) {
			BadRobot* currentRobot = (BadRobot*)currentObject;
			currentRobot->isStunned = true;
			testObject->isAlive = false;
			RemoveCollider(testCollider);
			RemoveRigidBody(testObject);
		}
	}
	/*Interactions with walls, Robots and Players*/
    if (currentObject || testObject) {
		if (currentObject && (currentObject->objType == PLAYER || currentObject->objType == ROBOT) && !testObject) {
			AABBImpulseResolution(currentCollider, testCollider);
		}
		else if (currentObject && testObject) {
			if (currentObject->objType == PLAYER && testObject->objType == ROBOT) {
				PlayerCharacter* currentPlayer = (PlayerCharacter*)currentObject;
				if(!currentPlayer->isInvunerable)
					AABBImpulseResolution(currentCollider, testCollider);
			}
			else if (currentObject->objType == ROBOT && testObject->objType == PLAYER) {
				PlayerCharacter* currentPlayer = (PlayerCharacter*)testObject;
				if (!currentPlayer->isInvunerable)
					AABBImpulseResolution(testCollider, currentCollider);
			}
			else if (currentObject->objType == ROBOT && testObject->objType == ROBOT) {
				AABBImpulseResolution(currentCollider, testCollider);
			}
		}
	}
}