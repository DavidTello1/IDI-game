#ifndef __OBSTACLES_H__
#define __OBSTACLES_H__

#include "p2List.h"
#include "p2Point.h"
#include "j1Module.h"
#include "j1Entity.h"

struct SDL_Texture;
struct ObjectsData;

class Obstacles : public Entity
{
public:

	Obstacles(entityType type, bool ground);

	// Destructor
	~Obstacles();

	//Called before the first frame
	bool Start();
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();

	// Called before quitting
	void CleanUp();

public:
	Animation	idle;
	bool ground;

};
#endif // !__OBSTACLES_H__