#include "CollisionVolume.h"

using namespace NCL;
using namespace CSC3222;

CollisionVolume::CollisionVolume(type volumeType, float val){
	_volumeType = volumeType;
	if (_volumeType == CIRCLE) {
		_radius = val;
	}
	if (_volumeType == SQUARE) {
		_halfSize = Vector2(val, val);
	}
}
CollisionVolume::CollisionVolume(type volumeType, float val,  Vector2 position){
	_volumeType = volumeType;
	if (_volumeType == CIRCLE) {
		_radius = val;
	}
	if (_volumeType == SQUARE) {
		_halfSize = Vector2(val, val);
	}
	_position = position; 
}
CollisionVolume::CollisionVolume(type volumeType, Vector2 val, Vector2 position) {
	_volumeType = volumeType;
	if (_volumeType == CIRCLE) {
		_radius = val.x;
	}
	if (_volumeType == SQUARE) {
		_halfSize = val;
	}
	_position = position;

}
CollisionVolume::~CollisionVolume()
{
}

void CollisionVolume::setPosition(Vector2 position)
{
	_position = position;
}

void CollisionVolume::setCollisionObj(SimObject* collisionObj)
{
	_collisionObj = collisionObj;
}

