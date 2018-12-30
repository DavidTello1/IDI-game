#ifndef __j1ENTITY_H__
#define __j1ENTITY_H__

#include "p2Log.h"
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

	void PositionCollider();
	void Draw();

public:
	Animation* current_animation = nullptr;
	entityType type;
	iPoint position;
	iPoint size;
	iPoint speed;
	SDL_Rect Collider;
	bool dead;
};

#endif