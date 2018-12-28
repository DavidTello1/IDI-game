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

void Entity::Draw(float dt)
{

	if (type == GRID || type == COIN)
	{
		if (flip_ver)
		{
			App->render->Blit(App->entitycontroller->texture, position.x, position.y, &rect, SDL_FLIP_VERTICAL);
		}
		if (flip_hor)
		{
			App->render->Blit(App->entitycontroller->texture, position.x, position.y, &rect, SDL_FLIP_HORIZONTAL);
		}
		if (!flip_ver && !flip_hor)
		{
			App->render->Blit(App->entitycontroller->texture, position.x, position.y, &rect, SDL_FLIP_NONE);
		}

	}
	else
	{
		if (flip)
		{
			App->render->Blit(App->entitycontroller->texture, position.x, position.y, &(Current_Animation->GetCurrentFrame(dt)), SDL_FLIP_HORIZONTAL);
		}
		else
		{
			App->render->Blit(App->entitycontroller->texture, position.x, position.y, &(Current_Animation->GetCurrentFrame(dt)), SDL_FLIP_NONE);
		}
	}

}

void Entity::Collider_Overlay()
{
	SDL_Rect ObjectRect;
	SDL_Rect result;

	p2List_item<Entity*>* tmp = App->entitycontroller->Entities.start; //grid
	while(tmp != nullptr)
	{
		if (tmp->data->type == Entity::entityType::GRID)
		{
			if (SDL_IntersectRect(&Collider, &tmp->data->Collider, &result))
			{
				grid_collision = true;
				is_static = false;

				if (Collider.y + (3 * Collider.h / 4) <= tmp->data->position.y)
				{
					top_grid = true;
				}

				if (tmp->data->grid_type == "Hide_up") 
				{
					vertical = true;
					flip_ver = true;
				}
				else if (tmp->data->grid_type == "Hide_down")
				{
					vertical = true;
					flip_ver = false;
				}
				else if (tmp->data->grid_type == "Hide_left")
				{
					vertical = false;
					flip_hor = true;
				}
				else if (tmp->data->grid_type == "Hide_right")
				{
					vertical = false;
					flip_hor = false;
				}

				grid_speed = tmp->data->speed;
				grid_direction = tmp->data->direction;

			}
		}

		if (tmp->data->type == Entity::entityType::BOX)
		{
			if (SDL_IntersectRect(&Collider, &tmp->data->Collider, &result))
			{
				box_collision = true;
				box_position = tmp->data->position;
				box_moving = tmp->data->box_moving;
			}
		}

		tmp = tmp->next;
	}

	for (p2List_item<ObjectsGroup*>* object = App->map->data.objLayers.start; object; object = object->next) //objects colliders
	{
		if (object->data->name == ("Collision"))
		{
			for (p2List_item<ObjectsData*>* objectdata = object->data->objects.start; objectdata; objectdata = objectdata->next)
			{
				ObjectRect.x = objectdata->data->x;
				ObjectRect.y = objectdata->data->y;
				ObjectRect.w = objectdata->data->width;
				ObjectRect.h = objectdata->data->height;

				if (SDL_IntersectRect(&Collider, &ObjectRect, &result))
				{
					if (objectdata->data->name == "Wall")
					{
						if (objectdata->data->type == "Wall_left")
						{
							wall_left = true;
						}
						if (objectdata->data->type == "Wall_right")
						{
							wall_right = true;
						}
					}
					else if (objectdata->data->name == "Floor")
					{
						if (position.y + Collider.h - ceil(gravity*App->dt) <= ObjectRect.y)
						{
							if (result.h < result.w)
							{
								position.y -= result.h - 1;
								if (box_moving == false)
								{
									if (landed == false)
									{
										landing = true;
									}
									if (first_ground == false)
									{
										first_ground = true;
									}
									grounded = true;
								}
							}
						}
						else
						{
							if (position.x + Collider.w > ObjectRect.x + ObjectRect.w)
							{
								wall_left = true;
							}
							else
							{
  								wall_right = true;
							}
						}
					}
					else if (objectdata->data->name == "Spikes")
					{
						if (App->scene->godmode == false)
						{
							dead = true;
						}
					}
					else if (objectdata->data->name == "Ceiling")
					{
						if (App->scene->godmode == false)
						{
							ceiling = true;
						}
					}
					else if (objectdata->data->name == "Grid" && objectdata->data->type == "Static") 
					{
						grid_collision = true;
						is_static = true;

						if (Collider.y + (3 * Collider.h / 4) <= objectdata->data->y)
						{
							top_grid = true;
						}
					}
					else if (objectdata->data->name == "Finish")
					{
						if (App->scene->currentMap == 0)
						{
							if (App->scene->finish_2 == false)
							{
								App->scene->finish_1 = true;

								//change map
								App->map->angle = 0.0;
								App->map->rotate = true;
								App->map->rotate_end = false;
								App->scene->change = true;
							}
							else
							{
								App->scene->end_game = true;
								App->SaveGame();
							}
						}
						else if (App->scene->currentMap == 1)
						{
							if (App->scene->finish_1 == false)
							{
								App->scene->finish_2 = true;

								//change map
								App->map->angle = 0.0;
								App->map->rotate = true;
								App->map->rotate_end = false;
								App->scene->change = true;
							}
							else
							{
								App->scene->end_game = true;
								App->SaveGame();
							}
						}
					}
				}
			}
		}
	}
}

void Entity::PositionCollider()
{
	Collider.x = position.x;
	Collider.y = position.y;
	Collider.w = size.x;
	Collider.h = size.y;

	if (type == FLYING_ENEMY)
	{
		SightCollider.x = position.x - SightCollider.w / 2 + size.x / 2;
		SightCollider.y = position.y - SightCollider.h / 2 + size.y / 2;
	}
	else if (type == LAND_ENEMY)
	{
		SightCollider.x = position.x - SightCollider.w / 2 + size.x / 2;
		SightCollider.y = position.y - SightCollider.h / 2 + size.y / 2;
	}

}

void Entity::CheckBounds()
{
	for (p2List_item<ObjectsGroup*>* object = App->map->data.objLayers.start; object; object = object->next) //objects colliders
	{
		if (object->data->name == ("Boundaries"))
		{
			for (p2List_item<ObjectsData*>* objectdata = object->data->objects.start; objectdata; objectdata = objectdata->next)
			{
				if (type_int == 1 && objectdata->data->name == "Box1"
					|| type_int == 2 && objectdata->data->name == "Box2"
					|| type_int == 3 && objectdata->data->name == "Box3"
					|| type_int == 4 && objectdata->data->name == "Box4")
				{
					Bounds.x = objectdata->data->x;
					Bounds.y = objectdata->data->y;
					Bounds.w = objectdata->data->width;
					Bounds.h = objectdata->data->height;
				}
			}
		}
	}
}