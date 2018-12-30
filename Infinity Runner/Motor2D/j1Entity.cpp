#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1Map.h"
#include "j1Input.h"
#include "j1SceneChange.h"
#include "j1Scene.h"
#include "j1Audio.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1EntityController.h"
#include "j1Entity.h"


Entity::Entity()
{
}

Entity::Entity(entityType type) : type(type)
{
}

Entity::~Entity()
{
}

void Entity::Draw()
{
	App->render->Blit(App->entitycontroller->texture, position.x, position.y, &(current_animation->GetCurrentFrame()), SDL_FLIP_NONE);
}

void Entity::PositionCollider()
{
	Collider.x = position.x;
	Collider.y = position.y;
	Collider.w = current_animation->GetCurrentFrame().w;
	Collider.h = current_animation->GetCurrentFrame().h;
}