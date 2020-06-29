#pragma once
#include <vector>
#include "RigidBody.h"
namespace NCL {
	namespace Maths {
		class Vector2;
	}
	namespace CSC3222 {
		class GameMap;
		class SimObject;
		class GameSimsRenderer;
		class GameSimsPhysics;
		class TextureBase;
		class TextureManager;
		class PlayerCharacter;
		class Pathing;
		class BadRobot;
		class Food;
		class BadFoodGame {
		public:
			Pathing* pathingEntity;

			BadFoodGame();
			~BadFoodGame();

			void Update(float dt);
			
			void DrawColliders();

			void AddNewObject(SimObject* object);
			void AddNewObject(RigidBody* object, Vector2 halfSize, Vector2 position);
			void SetScore(int score) { currentScore = score; }
			void IncrFoodCount() { 
				if (foodCount++ > 5) {
					foodCount = 5;
				}

			}
			void ResetGame();
			void CheckGrass();
			void BuildingInteractions();
			void DecrLives();
			void IncrBalloons() { balloons++; }
			void IncrCoins() { coins++; }
			void SpawnRobot();
			void SpawnFood();
			void SpawnCoin();
			void SpawnBalloon();
			void UpdateRobot();
			void SpawnLaser(Vector2 Position, Vector2 velocity);
			int  GetFoodCount() { return foodCount; }

			PlayerCharacter* player;

		protected:
			void InitialiseGame();

			TextureManager*		texManager;
			GameSimsRenderer*	renderer;
			GameSimsPhysics*	physics;
			GameMap* currentMap;


			float gameTime;
			float RobotTimer = 0;
			int   RobotCounter = 0;
			float FoodTimer = 0; 
			float CoinTimer = 0;
			int currentScore;
			int foodCount;
			int coins;
			int balloons;
			int lives;
			bool debug = false;
			std::vector<SimObject*> gameObjects;
			std::vector<SimObject*> newObjects;
			std::vector<BadRobot*> robotList;
			std::vector<Food*> foodList;
		};
	}
}

