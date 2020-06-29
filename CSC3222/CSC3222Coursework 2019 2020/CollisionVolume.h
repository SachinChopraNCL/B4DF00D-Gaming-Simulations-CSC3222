#pragma once

#include "../../Common/MeshGeometry.h"
#include "SimObject.h"

namespace NCL {
	namespace CSC3222 {
		enum type { SQUARE, CIRCLE };
		class CollisionVolume {
		public:
			type _volumeType;
			float _radius;
			float extent; 
			Vector2 _halfSize = Vector2(0,0);
			Vector2 _position;
			SimObject* _collisionObj;
			CollisionVolume(type volumeType, float val);
			CollisionVolume(type volumeType, float val, Vector2 position);
			CollisionVolume(type volumeType, Vector2 val, Vector2 position);
			~CollisionVolume();
			void setPosition(Vector2 position);
			void setCollisionObj(SimObject* collisionObj);
	
		};
	}
}

