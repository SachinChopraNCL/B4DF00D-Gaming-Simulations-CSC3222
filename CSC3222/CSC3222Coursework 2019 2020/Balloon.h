#pragma once
#include "SimObject.h"

namespace NCL {
	namespace CSC3222 {
		class Balloon : public SimObject
		{
		public:
			Balloon(SimObject* player);
			~Balloon();

			bool UpdateObject(float dt) override;
			void DrawObject(GameSimsRenderer& r) override;

		protected:
			SimObject* _player; 
			bool detatched = false;
			float timer = 0;
		};
	}
}

