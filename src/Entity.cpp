#include "Entity.h"

#ifdef _WIN32
#include <res/Texture.h>
#else
#include <trutle/res/Texture.h>
#endif

Vec2 ToVec2(b2Vec2 bvec)
{
	return Vec2(bvec.x * PTM, bvec.y * PTM);
}

b2Vec2 Tob2Vec2(Vec2 vec)
{
	return b2Vec2(vec.x / PTM, vec.y / PTM);
}


float Deg2Rad(float deg)
{
	return deg * (M_PI / 180.f);
}

float Rad2Deg(float rad)
{
	return rad * (180.f / M_PI);
}


/*
================
Entity Public 
================
 */
Entity::Entity(b2World *world)
	:	_world(world),
		_body(NULL)
{
	
}

Entity::~Entity()
{
	if (_body) {
		_world->DestroyBody(_body);
	}
}


void Entity::Update(const DeltaTime &dt)
{
	GameObject::Update(dt);

	_body->SetTransform(Tob2Vec2(Position()), 0.f);
}

Vec2 Entity::GetScreenPosition()
{
	Vec2 layerPos = GetParentLayer()->Position();
	Vec2 pos = Position();

	pos.x -= layerPos.x;
	pos.y -= layerPos.y;
	return pos;
}


/*
================
Entity Protected
================
*/
void Entity::CreateSquareBody(b2BodyType type)
{
	b2BodyDef bd;
	bd.position = Tob2Vec2(Position());
	bd.type = type;
	bd.userData = this;

	b2Vec2 dim = Tob2Vec2(GetTexture()->GetDimensions());
	b2PolygonShape shape;
	shape.SetAsBox(dim.x, dim.y);
	
	b2FixtureDef fd;
	fd.isSensor = false;
	fd.density = 3.5f;
	fd.userData = this;
	fd.shape = &shape;

	_body = _world->CreateBody(&bd);
	_body->CreateFixture(&fd);
}
