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
		GRID,
		BOX,
		PLAYER,
		FLYING_ENEMY,
		LAND_ENEMY,
		COIN
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
	virtual void Save(pugi::xml_node& file) const {};
	virtual void Load(pugi::xml_node& file) {};
	virtual void Restart() {};
	virtual void LoadAnimations() {};
	virtual void ChangeAnimation() {};
	virtual void UpdateSide() {};

	void Collider_Overlay();
	void PositionCollider();
	void CheckBounds();
	void Draw(float dt);

public:
	Animation* Current_Animation = nullptr;
	entityType type;
	
	iPoint position;
	iPoint initial_pos;
	iPoint size;
	iPoint speed;
	fPoint final_speed;
	fPoint current_speed;
	int direction; // 1 right, -1 left
	int gravity;
	int lives;

	SDL_Rect Collider;
	SDL_Rect SightCollider;
	
	bool flip = false;
	bool chasing_player = false;
	bool hurt = false;

	bool moving_left = false;
	bool moving_right = true;

	iPoint grid_speed;
	iPoint box_speed;
	iPoint box_position;
	int grid_direction;
	int jumpSpeed;
	bool left = false;
	bool right = false;
	bool grid_moving = false;
	bool dead = false;
	bool grounded = false;
	bool jumping = false;
	bool jump_able = false;
	bool grid = false;
	bool sliding = false;
	bool landing = false;
	bool falling = false;
	bool gripping = false;
	bool wall_left = false;
	bool wall_right = false;
	bool gravity_active = false;
	bool grid_collision = false;
	bool top_grid = false;
	bool ceiling = false;
	bool landed = false;
	bool attack = false;
	bool attack_able = false;
	bool box_collision = false;
	bool gripped = false;
	bool first_ground = false;

	p2SString grid_type;
	iPoint initial_size;
	iPoint prev_size;
	SDL_Rect rect;
	int frame = 0;
	int type_int;
	bool flip_ver = false;
	bool flip_hor = false;
	bool vertical = false;
	bool is_static = false;

	SDL_Rect Bounds;
	bool box_moving = false;
	bool end_moving = false;
	bool box_flag = false;

};

#endif