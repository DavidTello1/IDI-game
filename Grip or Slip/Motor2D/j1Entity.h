#ifndef __j1ENTITY_H__
#define __j1ENTITY_H__

#include "p2Point.h"
#include "p2Log.h"
#include "p2DynArray.h"
#include "p2Animation.h"
#include "j1Map.h"
#include "j1App.h"


class Entity
{
public:
	enum entityType
	{
		NO_TYPE,
		WALL,
		BOX,
		SAW,
		PLAYER,
	};

public:
	Entity();
	Entity(entityType type);
	Entity(entityType type, iPoint position, iPoint Size);
	~Entity();
	virtual bool Awake(pugi::xml_node & config) { return true; };
	virtual bool Start() { return true; };
	virtual bool PreUpdate() { return true; };
	virtual bool Update(float dt) { return true; };
	virtual bool PostUpdate() { return true; };

	virtual void CleanUp() {};
	virtual void Restart() {};
	virtual void LoadAnimations() {};
	virtual void ChangeAnimation() {};

	void Collider_Overlay();
	void PositionCollider();
	void Draw(float dt);

public:
	Animation* Current_Animation = nullptr;
	entityType type;
	
	iPoint position;
	iPoint size;
	iPoint speed;
	int gravity;
	int lives;

	SDL_Rect Collider;
	
	bool flip = false;
	int jumpSpeed;
	bool dead = false;
	bool grounded = false;
	bool jumping = false;
	bool sliding = false;
	bool attack = false;
};

#endif