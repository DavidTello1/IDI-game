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

Entity::Entity(entityType type, iPoint position, iPoint Size) : type(type), position(position), size(Size)
{
	if (type == entityType::BOX)
	{
	}
}

Entity::~Entity()
{
}

void Entity::Draw()
{
	if (flip)
	{
		App->render->Blit(App->entitycontroller->texture, position.x, position.y, &(current_animation->GetCurrentFrame()), SDL_FLIP_HORIZONTAL);
	}
	else
	{
		App->render->Blit(App->entitycontroller->texture, position.x, position.y, &(current_animation->GetCurrentFrame()), SDL_FLIP_NONE);
	}
}

void Entity::Collider_Overlay()
{
	SDL_Rect result;

	p2List_item<Entity*>* tmp = App->entitycontroller->Entities.start; //grid
	while(tmp != nullptr)
	{
		if (tmp->data->type == Entity::entityType::BOX)
		{
			if (SDL_IntersectRect(&Collider, &tmp->data->Collider, &result))
			{
				//box_collision = true;
				//box_position = tmp->data->position;
				//box_moving = tmp->data->box_moving;
			}
		}

		tmp = tmp->next;
	}
}

void Entity::PositionCollider()
{
	Collider.x = position.x;
	Collider.y = position.y;
	Collider.w = current_animation->GetCurrentFrame().w;
	Collider.h = current_animation->GetCurrentFrame().h;
}