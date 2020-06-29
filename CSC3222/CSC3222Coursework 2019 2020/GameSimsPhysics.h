#pragma once
#include <vector>
#include "../../Common/Vector2.h"
#include "BadFoodGame.h"
#include <map>
namespace NCL {
	namespace CSC3222 {
		class RigidBody;
		class CollisionVolume;

		class GameSimsPhysics	{

		public:
			GameSimsPhysics(BadFoodGame* currentGame);
			~GameSimsPhysics();
			float totalDT = 0;

			void Update(float dt);
			void ResolveCollision(CollisionVolume* a, CollisionVolume* b);

			void AddRigidBody(RigidBody* b);
			void RemoveRigidBody(RigidBody* b);

			void AddCollider(CollisionVolume* c);
			void RemoveCollider(CollisionVolume* c);

			std::vector<CollisionVolume*> getVolumes();
			bool reset = false;

		protected:
			void Integration(float dt);
			void CollisionDetection(float dt);
			std::vector<RigidBody*>			allBodies;
			std::vector<CollisionVolume*>	allColliders;
			BadFoodGame* game;
		};
	}
}

