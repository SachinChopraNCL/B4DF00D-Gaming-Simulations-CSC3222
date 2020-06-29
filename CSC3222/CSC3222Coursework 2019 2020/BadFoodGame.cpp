#include "BadFoodGame.h"
#include "SimObject.h"
#include "GameMap.h"
#include "TextureManager.h"
#include "GameSimsRenderer.h"
#include "GameSimsPhysics.h"
#include "PlayerCharacter.h"
#include "Laser.h"
#include "Food.h"
#include "Coin.h"
#include "Balloon.h"
#include "BadRobot.h"

#include "CollisionVolume.h"
#include "Pathing.h"
#include "../../Common/Window.h"
#include "../../Common/TextureLoader.h"

using namespace NCL;
using namespace CSC3222;

BadFoodGame::BadFoodGame() {
	renderer = new GameSimsRenderer();
	texManager = new TextureManager();
	physics = new GameSimsPhysics(this);
	SimObject::InitObjects(this, texManager);
	InitialiseGame();
	pathingEntity = new Pathing(currentMap->getMapCosts());

}

BadFoodGame::~BadFoodGame()	{
	delete currentMap;
	delete texManager;
	delete renderer;
	delete physics;
}

void BadFoodGame::Update(float dt) {
	for (auto i : newObjects) {
		gameObjects.emplace_back(i);
	}
	newObjects.clear();

	gameTime += dt;
	/* Press F1 to turn on collider drawings! */
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::F1)) {
		debug = debug == true ? false : true; 
	}
	/* Reset Shortcut */
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::F2)) {
		lives = 0;
	}
	RobotTimer += dt;
	FoodTimer += dt;
	CoinTimer += dt;
	renderer->Update(dt);
	physics->Update(dt);
	currentMap->DrawMap(*renderer);

	srand((int)(gameTime * 1000.0f));
	
	for (auto i = gameObjects.begin(); i != gameObjects.end(); ) {
		if (!(*i)->UpdateObject(dt)) { //object has said its finished with
			i = gameObjects.erase(i);
		}
		else {
			(*i)->DrawObject(*renderer);
			++i;
		}
	}	

	renderer->DrawString("B4DF00D", Vector2(420, 700));

	renderer->DrawString("Score: " + std::to_string(currentScore), Vector2(10, 10));
	renderer->DrawString("Lives left: " + std::to_string(lives), Vector2(10, 30));
	renderer->DrawString("Food: " + std::to_string(foodCount), Vector2(10, 50));
	renderer->DrawString("Coins: " + std::to_string(coins), Vector2(10, 70));
	renderer->DrawString("Balloons: " + std::to_string(balloons), Vector2(10, 90));
	renderer->Render();
	
	if (lives <= 0) {
		ResetGame();
	}
	else {
		SpawnRobot();
		SpawnFood();
		SpawnCoin();
		UpdateRobot();
		BuildingInteractions();
		CheckGrass();
	}
	
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::SPACE) && balloons > 0) {
		SpawnBalloon();
	}



	if (debug)
		DrawColliders();
}

void BadFoodGame::ResetGame() {
	/* Set's all Entities apart from player to dead */
	for (auto i = gameObjects.begin(); i != gameObjects.end();) {
		if (!((*i) == player)) {
			(*i)->isAlive = false;
		}
		++i;
	}
	foodList.clear();
	robotList.clear();
	physics->reset = true;
	currentScore = 0;
	FoodTimer = 0;
	CoinTimer = 0;
	RobotTimer = 0;
	foodCount = 0;
	coins = 0;
	balloons = 0;
	lives = 3;
}

void BadFoodGame::DrawColliders() {
	/* Test Code */

	for (CollisionVolume* collider : physics->getVolumes()) {
		if (collider->_volumeType == CIRCLE) {
			renderer->DrawCircle(collider->_position, collider->_radius, Vector4(1, 0, 0, 1));
		}
		else {
			renderer->DrawBox(collider->_position, collider->_halfSize, Vector4(1, 0, 0, 1));
		}
	}
	/* End */
}

void BadFoodGame::BuildingInteractions() {
	/* Balloon Shop */
	if (336 < player->GetPosition().x && player->GetPosition().x < 352 && 272 < player->GetPosition().y && player->GetPosition().y < 288) {
		if (coins > 0) {
			coins--;
			balloons++;
		}
	}
	/* Scoring Points */
	if (384 < player->GetPosition().x && player->GetPosition().x < 400 && 96 < player->GetPosition().y && player->GetPosition().y < 112) {
		int t0 = 0, t1 = 1, next = 0, n, score;
		if (foodCount > 0) {
			n = foodCount + 3;
			for (int i = 1; i <= n; ++i) {
				if (i == 1) {
					score = 0;
				}
				if (i == 2) {
					score = 1;
				}

				next = t0 + t1;
				t0 = t1;
				t1 = next;
				score = next;
			}
			score *= 100;
			currentScore += score;
			foodCount = 0;
		}
	}
}

void BadFoodGame::CheckGrass() {
	/*Is the player on Grass ?*/
	int playerX = floor((float)player->GetPosition().x / 16);
	int playerY = floor((float)player->GetPosition().y / 16);
	if (currentMap->getMapCosts()[(playerY * 30) + playerX] == 0) {
		player->onGrass = true;
	}
	else {
		player->onGrass = false;
	}
	/* Checks for all Robots on grass */
	for (BadRobot* robot : robotList) {
		int robotX = floor((float)robot->GetPosition().x / 16);
		int robotY = floor((float)robot->GetPosition().y / 16);
		if (currentMap->getMapCosts()[(robotY * 30) + robotX] == 0) {
			robot->onGrass = true;
		}
		else {
			robot->onGrass = false;
		}
	}
}

void BadFoodGame::DecrLives() {
	lives--;
	player->SetVelocity(Vector2(0, 0));
	player->SetPosition(Vector2(40, 104));
}

void BadFoodGame::SpawnLaser(Vector2 position, Vector2 velocity) {
	/*Instantiate a laser*/
	Laser* testLaser = new Laser(Vector2(1, 0));
	CollisionVolume* collider_laser = new CollisionVolume(CIRCLE, 5);
	testLaser->SetPosition(position);
	testLaser->SetCollider(collider_laser);
	collider_laser->setCollisionObj(testLaser);
	testLaser->SetVelocity(velocity);
	AddNewObject(testLaser);
}
void BadFoodGame::SpawnRobot(){
	if (RobotTimer > 15 && RobotCounter < 20) {
		BadRobot* testRobot = new BadRobot(pathingEntity,this);
		testRobot->currentPlayer = player;
		testRobot->SetPosition(Vector2(240, 55));
		CollisionVolume* collider_robot = new CollisionVolume(SQUARE, 6);
		testRobot->SetCollider(collider_robot);
		collider_robot->setCollisionObj(testRobot);
		AddNewObject(testRobot);
		robotList.push_back(testRobot);
		RobotTimer = 0;
		RobotCounter++;
	}
}
void BadFoodGame::UpdateRobot() {
	for (BadRobot* robot : robotList) {
		robot->allRobots = robotList;
	}
}

void BadFoodGame::SpawnFood(){
	if (FoodTimer > 5) {
		/* Need to check if valid position for spawn */
		int x = -1; int y = -1;
		while (!pathingEntity->isValid(x, y)) {
			x = rand() % 30;
			y = rand() % 20;
		}
		Food* testFood = new Food();
		CollisionVolume* collider_food = new CollisionVolume(CIRCLE, 9);
		testFood->SetPosition(Vector2((x * 16) + 8, (y * 16) + 8));
		testFood->SetCollider(collider_food);
		collider_food->setCollisionObj(testFood);
		AddNewObject(testFood);
		foodList.push_back(testFood);
		FoodTimer = 0;
	}
}

void BadFoodGame::SpawnCoin() {
	if (CoinTimer > 15) {
		/* Need to check if valid position for spawn */
		int x = -1; int y = -1;
		while (!pathingEntity->isValid(x, y)) {
			x = rand() % 30;
			y = rand() % 20;
		}
		Coin* testCoin = new Coin();
		CollisionVolume* collider_coin = new CollisionVolume(CIRCLE, 9);
		testCoin->SetPosition(Vector2((x * 16) + 8, (y * 16) + 8));
		testCoin->SetCollider(collider_coin);
		collider_coin->setCollisionObj(testCoin);
		AddNewObject(testCoin);
		CoinTimer = 0;
	}
}

void BadFoodGame::SpawnBalloon() {
	Balloon* testBalloon = new Balloon(player);
	CollisionVolume* collider_bal = new CollisionVolume(CIRCLE, 9);
	testBalloon->SetPosition(Vector2(player->GetPosition().x, player->GetPosition().y - 20));
	testBalloon->SetCollider(collider_bal);
	collider_bal->setCollisionObj(testBalloon);
	AddNewObject(testBalloon);
	balloons--;
	player->isInvunerable = true;
}

void BadFoodGame::InitialiseGame() {
	delete currentMap;
	srand(time(NULL));
	for (auto o : gameObjects) {
		delete o;
	}
	gameObjects.clear();

	currentMap = new GameMap("BadFoodMap.txt", gameObjects, *texManager, this);

	renderer->SetScreenProperties(16, currentMap->GetMapWidth(), currentMap->GetMapHeight());

	player = new PlayerCharacter();
	player->SetCharacterType(PlayerCharacter::CharacterType::TYPE_A);
	player->SetVelocity(Vector2(0, 0));
	player->SetPosition(Vector2(40, 104));
	CollisionVolume* collider = new CollisionVolume(SQUARE, 6);
	player->SetCollider(collider);
	collider->setCollisionObj(player);
	AddNewObject(player);

	gameTime		= 0;
	currentScore	= 0;
	foodCount		= 0;
	coins			= 0;
	balloons		= 0;
	lives			= 3;
}


void BadFoodGame::AddNewObject(SimObject* object) {
	newObjects.push_back(object);
	physics->AddRigidBody(object);
	if (object->GetCollider()) {
		physics->AddCollider(object->GetCollider());
	}
}

void BadFoodGame::AddNewObject(RigidBody* object, Vector2 halfSize, Vector2 position)
{
	physics->AddRigidBody(object);
	CollisionVolume* objectVolume = new CollisionVolume(SQUARE, halfSize, position);
	physics->AddCollider(objectVolume);

}
