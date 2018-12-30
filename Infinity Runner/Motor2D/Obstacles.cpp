#include "p2Log.h"
#include "j1App.h"
#include "Obstacles.h"
#include "j1Audio.h"
#include "j1Render.h"
#include "j1Map.h"
#include "j1Window.h"
#include "j1Textures.h"
#include "j1Input.h"
#include "j1Scene.h"
#include "j1SceneChange.h"
#include "j1EntityController.h"
#include "j1Entity.h"

Obstacles::Obstacles(entityType Type, bool Ground, iPoint pos)
{
	type = Type;
	ground = Ground;
	position = pos;

	if (type == entityType::BOX)
	{
		dead = false;
		idle.PushBack({ 190,0,70,70 });
		current_animation = &idle;

		size = { 70,70 };
	}
	else if (type == entityType::SAW)
	{
		if (ground == true)
		{
			saw_ground.PushBack({ 50,70,70,35 });
			saw_ground.PushBack({ 120,70,70,35 });
			saw_ground.loop = true;
			saw_ground.speed = 5.0f;
			current_animation = &saw_ground;

			size = { 70,35 };
		}
		else
		{
			idle.PushBack({ 50,0,70,70 });
			idle.PushBack({ 50,0,70,70 });
			idle.loop = true;
			idle.speed = 5.0f;
			current_animation = &idle;

			size = { 70,70 };
		}
	}
	else if (type == entityType::WALL)
	{
		idle.PushBack({ 0,0,50,105 });
		current_animation = &idle;

		size = { 50,105 };
	}
}

Obstacles::~Obstacles()
{
}

bool Obstacles::PreUpdate()
{
	if (ground == true)
	{
		if (type == entityType::WALL)
		{
			position.y = App->map->data.height*App->map->data.tile_height - 175;
		}
		else if (type == entityType::SAW)
		{
			position.y = App->map->data.height*App->map->data.tile_height - 105;
		}
		else if (type == entityType::BOX)
		{
			position.y = App->map->data.height*App->map->data.tile_height - 140;
		}
	}
	else
	{
		if (type == entityType::WALL)
		{
			position.y = App->map->data.height*App->map->data.tile_height - 175 /*-player sliding.h + 15*/;
		}
		else
		{
			position.y = App->map->data.height*App->map->data.tile_height - 140 /*-player sliding.h + 15*/;
		}
	}
	return true;
}

bool Obstacles::Update(float dt)
{
	position.x -= App->map->scroll_speed;
	PositionCollider();

	return true;
}

bool Obstacles::PostUpdate()
{
	if (type == entityType::BOX && dead == true	|| position.x < -size.x)
	{
		App->entitycontroller->DeleteEntity(this);
		App->scene->num_obstacles--;
	}

	return true;
}

void Obstacles::CleanUp()
{
	LOG("---Obstacles Deleted");
}