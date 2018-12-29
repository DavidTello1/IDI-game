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
	gravity_active = config.child("gravity_active").attribute("value").as_bool();

	dead = false;
	final_speed = { 0,0 };
	first_ground = false;

	return ret;
}

bool j1Player::Start()
{	
	LoadAnimations();
	Current_Animation = &idle;

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

bool j1Player::Update(float dt)
{
	

	return true;
}

bool j1Player::PostUpdate()
{
	PositionCollider();

	if (dead && !App->scenechange->IsChanging())
	{
		App->scene->player_lives--;
		if (App->scene->player_lives > 0)
		{
			App->audio->PlayFx(DEAD);
			App->scenechange->ChangeMap(App->scene->currentMap, App->scene->fade_time);
		}
		else
		{
			App->scene->delay.Start();
			App->audio->PlayFx(GAME_OVER);
		}
	}

	return true;
}

void j1Player::Load(pugi::xml_node& data)
{
	size.x = data.child("size").attribute("width").as_int();
	size.y = data.child("size").attribute("height").as_int();
	position.x = data.child("position").attribute("x").as_int();
	position.y = data.child("position").attribute("y").as_int();
	gravity = data.child("gravity").attribute("value").as_int();
	speed.x = data.child("speed").attribute("x").as_int();
	speed.y = data.child("speed").attribute("y").as_int();
	grounded = data.child("grounded").attribute("value").as_bool();
	sliding = data.child("sliding").attribute("value").as_bool();
	jumping = data.child("jumping").attribute("value").as_bool();
	grid = data.child("grid").attribute("value").as_bool();
	gripped = data.child("gripped").attribute("value").as_bool();
	gravity_active = data.child("gravity_active").attribute("value").as_bool();
	
	LOG("--- Player Loaded");
}

// Save Game State
void j1Player::Save(pugi::xml_node& data) const
{
	pugi::xml_node player = data.append_child("player");

	LOG("---Player Saved");
}


void j1Player::CleanUp()
{
	LOG("---Player Deleted");

}

void j1Player::Restart()
{
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
				}
			}
		}
	}
	dead = false;
}

void j1Player::ChangeAnimation()
{

}

void j1Player::LoadAnimations()
{
	
}

void j1Player::CameraOnPlayer()
{
	App->render->camera.x = -position.x + App->render->camera.w / 3;
	App->render->camera.y = -position.y + App->render->camera.h / 2;

	if (App->render->camera.x > 0) //left limit
	{
		App->render->camera.x = 0;
	}
	if (App->render->camera.x - App->render->camera.w < -App->map->data.width * App->map->data.tile_width) //right limit
	{
		App->render->camera.x = -App->map->data.width * App->map->data.tile_width + App->render->camera.w;
	}
	if (App->render->camera.y > 0) //top limit
	{
		App->render->camera.y = 0;
	}
	if (App->render->camera.y - App->render->camera.h < -App->map->data.height * App->map->data.tile_height) //down limit
	{
		App->render->camera.y = -App->map->data.height * App->map->data.tile_height + App->render->camera.h;
	}
}
