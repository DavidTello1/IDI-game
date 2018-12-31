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
		size = { 82,82 };
		idle.PushBack({ 214,0,82,82 });
		current_animation = &idle;
	}
	else if (type == entityType::SAW)
	{
		if (ground == true)
		{
			size = { 82,41 };
			saw_ground.PushBack({ 50,0,82,41 });
			saw_ground.PushBack({ 50,0,82,41 });
			saw_ground.PushBack({ 50,0,82,41 });
			saw_ground.PushBack({ 132,0,82,41 });
			saw_ground.PushBack({ 132,0,82,41 });
			saw_ground.PushBack({ 132,0,82,41 });
			saw_ground.loop = true;
			saw_ground.speed = 1.0f;
			current_animation = &saw_ground;
		}
		else
		{
			size = { 82,82 };
			idle.PushBack({ 50,0,82,82 });
			idle.PushBack({ 50,0,82,82 });
			idle.PushBack({ 50,0,82,82 });
			idle.PushBack({ 132,0,82,82 });
			idle.PushBack({ 132,0,82,82 });
			idle.PushBack({ 132,0,82,82 });
			idle.loop = true;
			idle.speed = 1.0f;
			current_animation = &idle;

		}
	}
	else if (type == entityType::WALL)
	{
		size = { 50,105 };
		idle.PushBack({ 0,0,50,105 });
		current_animation = &idle;
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
			position.y = App->map->data.height*App->map->data.tile_height - 111;
		}
		else if (type == entityType::BOX)
		{
			position.y = App->map->data.height*App->map->data.tile_height - 152;
		}
	}
	else
	{
		if (type == entityType::WALL)
		{
			position.y = App->map->data.height*App->map->data.tile_height - 220;
		}
		else
		{
			position.y = App->map->data.height*App->map->data.tile_height - 192;
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
	if (type == entityType::BOX && dead == true)
	{
		App->entitycontroller->DeleteEntity(this);
		App->scene->num_obstacles--;
		App->scene->score++;
		App->scene->boxes_killed++;
		App->scene->num_boxes++;
	}
	if (position.x < -size.x)
	{
		App->entitycontroller->DeleteEntity(this);
		App->scene->num_obstacles--;
		App->scene->score++;

		if (type == entityType::BOX)
		{
			App->scene->num_boxes++;
		}
	}


	return true;
}

void Obstacles::CleanUp()
{
	LOG("---Obstacles Deleted");
}