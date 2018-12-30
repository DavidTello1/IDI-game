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

Obstacles::Obstacles(entityType Type, bool ground)
{
	type = Type;

	if (type == entityType::BOX)
	{
		dead = false;
		idle.PushBack({});
	}
	else if (type == entityType::SAW)
	{
		idle.PushBack({});
		idle.PushBack({});
	}
	else if (type == entityType::WALL)
	{
		idle.PushBack({});
	}
}

Obstacles::~Obstacles()
{
}

bool Obstacles::Start()
{
	current_animation = &idle;

	if (ground == true)
	{
		position.y = 20;
	}
	else
	{
		position.y = 50;
	}
	position.x = App->win->width;

	return true;
}

bool Obstacles::PreUpdate()
{
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
	if (type == entityType::BOX && dead == true	|| position.x < 0)
	{
		App->entitycontroller->DeleteEntity(this);
	}

	return true;
}

void Obstacles::CleanUp()
{
	LOG("---Obstacles Deleted");
}