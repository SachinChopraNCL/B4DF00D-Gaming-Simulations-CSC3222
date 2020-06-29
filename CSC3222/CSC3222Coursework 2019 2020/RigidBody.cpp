#include "RigidBody.h"

using namespace NCL;
using namespace CSC3222;

RigidBody::RigidBody()	{
	inverseMass = 1.5f;
	elasticity  = 0.5f;
	friction = 0.8;
	max_velocity = Vector2(64, 64);
}

RigidBody::RigidBody(bool isStatic)
{
	_isStatic = isStatic;
}


RigidBody::~RigidBody()	{

}
