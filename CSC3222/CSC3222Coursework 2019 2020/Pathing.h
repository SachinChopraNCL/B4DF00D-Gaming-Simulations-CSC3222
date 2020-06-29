#pragma once
#include <cmath>
#include <vector>
#include <stack>
using namespace std;
namespace NCL {
	namespace CSC3222 {
		class Pathing {
		public:
			/* Represents one 16x16 tile on the map */
			struct Node {
				int y;
				int x;
				int parentX;
				int parentY;
				float gCost;
				float hCost;
				float fCost;

				/* Operator overload to compare Node costs*/
				bool operator < (const Node& rhs) {
					return this->fCost < rhs.fCost;
				}
			};
			
			/* Loaded in from GameMap.cpp, See BadFoodGame.cpp */
			Pathing(int* costingMap) { _costingMap = costingMap; }

			vector<Node> findPath(Node entity, Node destination) {
				return aStar(destination, entity);
			}

			/* Need to check if a node is valid, i.e. it can be traversed, such as grass or road, exlusive.*/
			bool isValid(int x, int y) {
				int mapId = x + y * 30;
				if ((_costingMap[mapId] == 0 || _costingMap[mapId] == 9) && x != -1 && y != -1) {
					if (x < 0 || y < 0 || x >= 30 || y >= 20) {
						return false;
					}
					return true;
				}
				return false;
			}
		
		private: 
			int* _costingMap;
			/* Main Algorithm */
			vector<Node> aStar(Node dest, Node entity) {
				vector<Node> empty;
				if (isValid(dest.x, dest.y) == false) {
					return empty;
				}
				if (isDestination(_costingMap, entity.x, entity.y, dest)) {
					return empty;
				}

				/* closedList to track visited nodes */
				bool closedList[30][20];

				/* Initialise all Nodes to store FLT_MAX as they have not been explored */
				Node allMap[30][20];
				for (int x = 0; x < 30; x++) {
					for (int y = 0; y < 20; y++) {
						allMap[x][y].fCost = FLT_MAX;
						allMap[x][y].gCost = FLT_MAX;
						allMap[x][y].hCost = FLT_MAX;
						allMap[x][y].parentX = -1;
						allMap[x][y].parentY = -1;
						allMap[x][y].x = x;
						allMap[x][y].y = y;
						closedList[x][y] = false;
					}
				}

				/*Visiting first node */
				int x = entity.x;
				int y = entity.y;
				allMap[x][y].fCost = 0.0;
				allMap[x][y].gCost = 0.0;
				allMap[x][y].hCost = 0.0;
				allMap[x][y].parentX = x;
				allMap[x][y].parentY = y;

				vector<Node> openList;
				openList.emplace_back(allMap[x][y]);
				bool destinationFound = false;

				/* While there are nodes to still visit */
				while (!openList.empty() && (openList.size() < 20 * 30)) {
					Node node;
					/* Find the node with the lowesst possible fcost, and erase it from the openList, as it will be explored*/
					do {
						float temp = FLT_MAX;
						vector<Node>::iterator itNode;
						for (vector<Node>::iterator it = openList.begin(); it != openList.end(); it = next(it)) {
							Node n = *it;
							if (n.fCost < temp) {
								temp = n.fCost;
								itNode = it;
							}
						}
						node = *itNode;
						openList.erase(itNode);
					} while (isValid(node.x, node.y) == false);

					x = node.x;
					y = node.y;
					closedList[x][y] = true;
					/* Explore all neighbours of lowest fcost */
					for (int newX = -1; newX <= 1; newX++) {
						for (int newY = -1; newY <= 1; newY++) {
							double gNew, hNew, fNew;
							if (isValid(x + newX, y + newY)) {
								/* Check if we found the destination */
								if (isDestination(_costingMap, x + newX, y + newY, dest)) {
									allMap[x + newX][y + newY].parentX = x;
									allMap[x + newX][y + newY].parentY = y;
									destinationFound = true;
									return makePath(allMap, dest);
								}
								/* This node hasn't been fully explored */
								else if (closedList[x + newX][y + newY] == false) {
									/* Update g Values depending if it's a grass tile or a road tile */
									int mapId = x+newX + ((y+newY) * 30);
									gNew = node.gCost ;
									if (_costingMap[mapId] == 9) {
										gNew = node.gCost + 1;
									}
									else if (_costingMap[mapId] == 0) {
										gNew = node.gCost + 3;
									}
									hNew = calculateH( x + newX, y + newY, dest);
									fNew = gNew + hNew;
									/* If this node hasn't been explored before, or the new calculated cost is better than the current fvalue, update the information of the node*/
									if (allMap[x + newX][y + newY].fCost == FLT_MAX || allMap[x + newX][y + newY].fCost > fNew) {
										allMap[x + newX][y + newY].fCost = fNew;
										allMap[x + newX][y + newY].gCost = gNew;
										allMap[x + newX][y + newY].hCost = hNew;
										allMap[x + newX][y + newY].parentX = x;
										allMap[x + newX][y + newY].parentY = y;
										openList.emplace_back(allMap[x + newX][y + newY]);
									}
								}
							}
						}
					}

				}
				if (destinationFound == false) {
					printf("No path found!");
					return empty;
				}
			}

			
			/* Heuristic calculation, using Manhattan Distance */
			double calculateH(int x, int y, Node dest) {
				return abs(x - dest.x) + abs(y - dest.y);
			}

			bool isDestination(int* costingMap, int x, int y, Node dest) {
				if (x == dest.x && y == dest.y) {
					return true;
				}
				return false;
			}

			vector<Node> makePath(Node allMap[30][20], Node dest) {
				int x = dest.x;
				int y = dest.y;
				stack<Node> path;
				vector<Node> usablePath;
				/* Unravel the path following the parent node direction back to the source node */
				while (!(allMap[x][y].parentX == x && allMap[x][y].parentY == y) && allMap[x][y].x != -1 && allMap[x][y].y != -1) {
					path.push(allMap[x][y]);
					int tempX = allMap[x][y].parentX;
					int tempY = allMap[x][y].parentY;
					x = tempX;
					y = tempY;
				}
				path.push(allMap[x][y]);
				while (!path.empty()) {
					Node top = path.top();
					path.pop();
					usablePath.emplace_back(top);
				}

				return usablePath;
			}

		
		};
	}	
}


