#include "j1App.h"
#include "j1Player.h"
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

j1Player::j1Player() : Entity(entityType::PLAYER)
{
	jumping = false;
	sliding = false;
	start_sliding = false;
	falling = false;
	grounded = false;
	dead = false;
}

j1Player::~j1Player()
{
}

bool j1Player::Awake(pugi::xml_node & config)
{
	LOG("Loading Player");
	bool ret = true;

	// Player starting point
	config = config.child("player");

	position.x = config.child("position").attribute("x").as_int();
	position.y = config.child("position").attribute("y").as_int();
	size.x = config.child("size").attribute("width").as_int();
	size.y = config.child("size").attribute("height").as_int();
	speed.x = config.child("speed").attribute("x").as_int();
	speed.y = config.child("speed").attribute("y").as_int();
	gravity = config.child("gravity").attribute("value").as_int();

	return ret;
}

bool j1Player::Start()
{	
	LoadAnimations();
	current_animation = &idle;

	//Sets the player in the start position
	for (p2List_item<ObjectsGroup*>* object = App->map->data.objLayers.start; object; object = object->next)
	{
		if (object->data->name == ("Collision"))
		{
			for (p2List_item<ObjectsData*>* objectdata = object->data->objects.start; objectdata; objectdata = objectdata->next)
			{
				if (objectdata->data->name == "StartPosition")
				{
					position.x = objectdata->data->x;
					position.y = objectdata->data->y;
					break;
				}
			}
		}
	}

	return true;
}

bool j1Player::PreUpdate()
{
	if (controls == WASD)
	{
		if ((App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) && !jumping && !falling)
		{
			jumping = true;
		}
		if (App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
		{
			sliding = true;
		}
		if (App->input->GetKey(SDL_SCANCODE_S) == KEY_UP && sliding)
		{
			finish_sliding = true;
		}
	}
	else if (controls == ARROWS)
	{
		if ((App->input->GetKey(SDL_SCANCODE_UP) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT) && !jumping && !falling)
		{
			jumping = true;
		}
		if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		{
			start_sliding = true;
		}
		if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_UP && sliding)
		{
			finish_sliding = true;
		}
	}
	else if (controls == UI_BUTTONS)
	{
		//...
	}
	else if (controls == DRAG_MOUSE)
	{
		//...
	}
	return true;
}

bool j1Player::Update(float dt)
{
	dx = 0;
	dy = 0;

	//gravity acting first
	if (!grounded)
	{
		speed.y += gravity;
	}

	if (jumping)
	{
		grounded = false;
		speed.y = jumpSpeed;
		dy -= speed.y;
		if (position.y <= top_pos)
		{
			jumping = false;
			falling = true;
		}
	}

	if (start_sliding)
	{
		speed.y -= gravity;
		if (position.y <= slide_pos)
		{
			start_sliding = false;
			sliding = true;
		}
	}

	
	return true;
}

bool j1Player::PostUpdate()
{
	PositionCollider();

	if (lives <= 0)
	{
		dead = true;
	}

	if (dead && !App->scenechange->IsChanging())
	{
		App->scenechange->ChangeMap(App->scene->currentMap, App->scene->fade_time);
		App->scene->delay.Start();
		App->audio->PlayFx(GAME_OVER);
	}

	return true;
}

void j1Player::CleanUp()
{
	LOG("---Player Deleted");

}

void j1Player::Restart()
{
	dead = false;
}

void j1Player::ChangeAnimation()
{

}

void j1Player::LoadAnimations()
{
	this-> idle = App->entitycontroller->info->idle;
	this-> slide = App->entitycontroller->info->slide;
	this-> jump = App->entitycontroller->info->jump;
}

void j1Player::Collider_Overlay()
{
	p2List_item<Entity*>* tmp = App->entitycontroller->Entities.start;
	while (tmp != nullptr)
	{
		if (SDL_HasIntersection(&Collider, &tmp->data->Collider))
		{
			if (tmp->data->type == Entity::entityType::BOX)
			{
				if (attack == true)
				{
					tmp->data->dead = true;
				}
				else
				{
					lives--;
					App->audio->PlayFx(DEAD);
				}
			}
			else if (tmp->data->type == Entity::entityType::WALL)
			{
				lives--;
				App->audio->PlayFx(DEAD);
			}
			else if (tmp->data->type == Entity::entityType::SAW)
			{
				dead = true;
			}
		}
		tmp = tmp->next;
	}
}
