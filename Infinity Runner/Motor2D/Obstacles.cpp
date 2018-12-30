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
#include "p2Log.h"

Obstacles::Obstacles(entityType type, iPoint position, iPoint Size) : Entity(entityType::BOX)
{
}

Obstacles::Obstacles(entityType type, iPoint position, iPoint Size) : Entity(entityType::SAW)
{
}

Obstacles::Obstacles(entityType type, iPoint position, iPoint Size) : Entity(entityType::WALL)
{
}


Obstacles::~Obstacles()
{
}

bool Obstacles::Start()
{
	return true;
}

bool Obstacles::PreUpdate()
{
}

bool Obstacles::Update(float dt)
{
	return true;
}

bool Obstacles::PostUpdate()
{
	PositionCollider();
	return true;
}

void Obstacles::CleanUp()
{
	LOG("---Player Deleted");

}

void Obstacles::LoadAnimations()
{
}