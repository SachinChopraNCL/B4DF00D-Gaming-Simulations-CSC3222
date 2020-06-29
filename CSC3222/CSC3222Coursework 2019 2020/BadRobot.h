#pragma once
#include "SimObject.h"
#include "PlayerCharacter.h"
#include "Pathing.h"
#include <vector>
#include "BadFoodGame.h"


namespace NCL {
	namespace CSC3222 {
		class BadRobot : public SimObject
		{
		public:
			vector<Pathing::Node> currentPath;

			enum class State {
				PATROL,
				ATTACK,
				PROTECT,
				TRANSITION 
			};
			Pathing* currentPather;
			PlayerCharacter* currentPlayer; 
			BadRobot(Pathing* pather, BadFoodGame* currentGame);
			~BadRobot();

			bool UpdateObject(float dt) override;
			void DrawObject(GameSimsRenderer& r) override;
			void Separation(float dt);
			vector<BadRobot*> allRobots;
			bool isStunned = false;
			float stunnedTimer = 0;
			bool onGrass = false; 
		protected:
			enum class RobotType {
				Green,
				White,
			};
			RobotType type;
			State currentState; 
			State oldState; 
			int tileIndex = 0;
			int tileMax;
			Vector2 distanceToNextNode;
			float timeToWait = 0;
			float timeToFire = 0;
			bool resetPath = false;
			bool protectMode = false;
			BadFoodGame* _currentGame; 
		
		};
	}
}

