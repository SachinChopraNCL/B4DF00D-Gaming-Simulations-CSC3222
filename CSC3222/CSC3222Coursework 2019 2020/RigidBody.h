#pragma once
#include "../../Common/Vector2.h"

namespace NCL {
	using namespace Maths;
	namespace CSC3222 {
		class RigidBody	{
			friend class GameSimsPhysics;
		public:
			bool _isStatic = false;
			RigidBody();
			RigidBody(bool isStatic);
			~RigidBody();

			Vector2 GetPosition() const {
				return position;
			}

			void SetPosition(const Vector2& newPosition) {
				position = newPosition;
			}

			Vector2 GetVelocity() const {
				return velocity;
			}

			void SetVelocity(const Vector2& newVelocity) {
				velocity = newVelocity;
			}

			void AddForce(const Vector2& newForce) {
				force += newForce;
			}

			Vector2 GetForce() {
				return force;
			}

			void AddImpulse(Vector2 _velocity) {
				velocity = (_velocity) ;
			}
			Vector2 max_velocity;


		protected:
			Vector2 position;
			Vector2 velocity;
			Vector2 force;
			Vector2 normal;
			float friction;
			float inverseMass;
			float elasticity;
		
		};
	}
}
