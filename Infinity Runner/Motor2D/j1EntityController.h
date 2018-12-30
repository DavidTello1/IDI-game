#ifndef __j1ENTITYCONTROLLER_H__
#define __j1ENTITYCONTROLLER_H__

#include "j1Module.h"
#include "p2List.h"
#include "p2Point.h"
#include "j1Entity.h"
#include "j1Player.h"

class Entity;

class j1EntityController : public j1Module
{
public:
	j1EntityController();
	~j1EntityController();

	bool Awake(pugi::xml_node & config);
	bool Start();
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();
	bool Restart();
	bool Draw(float dt);
	bool DebugDraw();

	Entity* AddEntity(Entity::entityType type, iPoint position, iPoint Size);
	bool DeleteEntity(Entity* entity);
	void DeleteEntities();

public:
	p2SString		folder;
	p2SString		texture_path;
	SDL_Texture*	texture;
	p2List<Entity*> Entities;

	j1Player*		info;

	bool godmode = false;
};

#endif //