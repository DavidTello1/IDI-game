#include "j1EntityController.h"
#include "j1Box.h"
#include "j1Scene.h"
#include "j1Render.h"
#include "j1Map.h"
#include "j1Input.h"
#include "Brofiler\Brofiler.h"


j1Box::j1Box() : Entity(entityType::BOX)
{
}

j1Box::~j1Box()
{
}

j1Box::j1Box(iPoint pos, iPoint Size, p2SString Type, p2SString side) : Entity(entityType::BOX)
{
	LOG("Loading Box");

	pugi::xml_document	config_file;
	pugi::xml_node		config;

	config = App->LoadConfig(config_file);

	config = config.child("entitycontroller").child("box");

	position.x = pos.x;
	position.y = pos.y;
	size.x = Size.x;
	size.y = Size.y;
	speed.x = config.child("speed").attribute("x").as_int();
	speed.y = config.child("speed").attribute("y").as_int();

	if (side == "right")
	{
		left = false;
	}
	else if (side == "left")
	{
		left = true;
	}

	if (Type == "Box1")
	{
		type_int = 1;
	}
	else if (Type == "Box2")
	{
		type_int = 2;
	}
	else if (Type == "Box3")
	{
		type_int = 3;
	}
	else if (Type == "Box4")
	{
		type_int = 4;
	}


	idle.PushBack({ 352, 96, size.x, size.y });
	gripped.PushBack({ 384, 96, size.x, size.y });

	Current_Animation = &idle;
	CheckBounds();
	PositionCollider();
	CheckSide();
}

bool j1Box::Update(float dt)
{
	BROFILER_CATEGORY("Box Update", Profiler::Color::Lavender);

	if (box_flag == false)
	{
		CheckSide();
		box_flag = true;
	}

	final_speed = { 0,0 };
	PositionCollider();

	if (App->scene->currentMap == 0)
	{
		if (left == true && box_moving == false)
		{
			direction = 1;
		}
		else if (left == false && box_moving == false)
		{
			direction = -1;
		}
	}
	else if (App->scene->currentMap == 1)
	{
		if (left == true && box_moving == false)
		{
			direction = -1;
		}
		else if (left == false && box_moving == false)
		{
			direction = 1;
		}
	}

	if (box_moving == false)
	{
		if (App->scene->currentMap == 0)
		{
			if (left == true)
			{
				position.x = Bounds.x;
				position.y = Bounds.y;
			}
			else if (left == false)
			{
				position.x = Bounds.x + Bounds.w - Collider.w;
				position.y = Bounds.y + Bounds.h - Collider.h;
			}
		}
		else if (App->scene->currentMap == 1)
		{
			if (left == true)
			{
				position.x = Bounds.x + Bounds.w - Collider.w;
				position.y = Bounds.y + Bounds.h - Collider.h;
			}
			else if (left == false)
			{
				position.x = Bounds.x;
				position.y = Bounds.y;
			}

		}
	}

	p2List_item<Entity*>* player = nullptr;
	for (p2List_item<Entity*>* i = App->entitycontroller->Entities.start; i != nullptr; i = i->next)
	{
		if (i->data->type == Entity::entityType::PLAYER)
		{
			player = i;
			break;
		}
	}
	if (SDL_HasIntersection(&Collider, &player->data->Collider))
	{
		box_collision = true;
	}
	else
	{
		box_collision = false;
	}
	
	if (player->data->gripping == true)
	{
		end_moving = false;
	}

	if (player->data->gripped == true && box_collision == true && end_moving == false)
	{
		Current_Animation = &gripped;
		box_moving = true;
	}
	else
	{
		Current_Animation = &idle;
		box_moving = false;
	}

	if (box_moving == true)
	{
		if (direction == 1) //going right
		{
			if (position.x + Collider.w < Bounds.x + Bounds.w)
			{
				position.x += ceilf(speed.x*dt);
			}
			else
			{
				direction = 2;
			}
		}
		else if (direction == -1) //going left
		{
			if (position.x > Bounds.x)
			{
				position.x -= ceilf(speed.x*dt);
			}
			else
			{
				direction = -2;
			}
		}
		else if (direction == 2) //going down
		{
			if (position.y + Collider.h < Bounds.y + Bounds.h)
			{
				position.y += ceilf(speed.y*dt);
			}
			else
			{
				if (App->scene->currentMap == 0)
				{
					left = false;
				}
				else if (App->scene->currentMap == 1)
				{
					left = true;
				}
				box_moving = false;
				end_moving = true;
			}
		}
		else if (direction == -2) //going up
		{
			if (position.y > Bounds.y)
			{
				position.y -= ceilf(speed.y*dt);
			}
			else
			{
				if (App->scene->currentMap == 0)
				{
					left = true;
				}
				else if (App->scene->currentMap == 1)
				{
					left = false;
				}
				box_moving = false;
				end_moving = true;
			}
		}
	}

	UpdateSide();

	return true;
}

void j1Box::CleanUp()
{
	LOG("---Box Deleted");
}

void j1Box::Load(pugi::xml_node& data)
{
	position.x = data.child("position").attribute("x").as_int();
	position.y = data.child("position").attribute("y").as_int();
	size.x = data.child("size").attribute("width").as_int();
	size.y = data.child("size").attribute("height").as_int();
	speed.x = data.child("speed").attribute("x").as_int();
	speed.y = data.child("speed").attribute("y").as_int();
	Bounds.x = data.child("Bounds").attribute("x").as_int();
	Bounds.y = data.child("Bounds").attribute("y").as_int();
	Bounds.w = data.child("Bounds").attribute("width").as_int();
	Bounds.h = data.child("Bounds").attribute("height").as_int();
	left = data.child("side").attribute("value").as_bool();
	box_moving = data.child("box_moving").attribute("value").as_bool();

	LOG("--- Box Loaded");
}

void j1Box::Save(pugi::xml_node& data) const
{
	pugi::xml_node box = data.append_child("box");

	box.append_child("position").append_attribute("x") = position.x;
	box.child("position").append_attribute("y") = position.y;
	box.append_child("size").append_attribute("width") = size.x;
	box.child("size").append_attribute("height") = size.y;
	box.append_child("speed").append_attribute("x") = speed.x;
	box.child("speed").append_attribute("y") = speed.y;
	box.append_child("Bounds").append_attribute("x") = Bounds.x;
	box.child("Bounds").append_attribute("y") = Bounds.y;
	box.child("Bounds").append_attribute("width") = Bounds.w;
	box.child("Bounds").append_attribute("height") = Bounds.h;
	box.append_child("side").append_attribute("value") = left;
	box.append_child("box_moving").append_attribute("value") = box_moving;

	LOG("--- Box Saved");
}

void j1Box::UpdateSide()
{
	if (type_int == 1)
	{
		App->scene->box_1_side = left;
	}
	else if (type_int == 2)
	{
		App->scene->box_2_side = left;
	}
	else if (type_int == 3)
	{
		App->scene->box_3_side = left;
	}
	else if (type_int == 4)
	{
		App->scene->box_4_side = left;
	}
}

void j1Box::CheckSide()
{
	if (type_int == 1)
	{
		left = App->scene->box_1_side;
	}
	else if (type_int == 2)
	{
		left = App->scene->box_2_side;
	}
	else if (type_int == 3)
	{
		left = App->scene->box_3_side;
	}
	else if (type_int == 4)
	{
		left = App->scene->box_4_side;
	}
}