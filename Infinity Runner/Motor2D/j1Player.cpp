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
	grounded = true;
	dead = false;
	is_jump = false;

	lives = 3;
	speed.y = 0;
	gravity = 2;

	jumpSpeed = 20;
	
	idle.PushBack({ 0,0,32,32 });
	jump.PushBack({ 64,0,32,32 });
	fall.PushBack({ 96,0,32,32 });
	start_slide.PushBack({ 32,0,32,16 });
	slide.PushBack({ 32,0,32,16 });
	finish_slide.PushBack({ 32,0,32,16 });

	//LoadAnimations();
	current_animation = &idle;
	current_animation->speed = 0.5f;

	Collider.x = position.x;
	Collider.y = position.y;
	Collider.w = current_animation->GetCurrentFrame().w;
	Collider.h = current_animation->GetCurrentFrame().h;

	floor = App->render->viewport.h - 70;

	position.x = 100;
	position.y = floor;

	LOG("player created");
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
	bool can_jump = (!jumping && !falling && !sliding && !finish_sliding && !start_sliding && grounded);
	bool can_slide = (!jumping && !falling && grounded && !finish_sliding);

	if (App->scene->controls == j1Scene::Controls::WASD)
	{
		if ((App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) && can_jump)
		{
			jumping = true;
		}
		if ((App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) && can_slide)
		{
			start_sliding = true;
		}
		if (App->input->GetKey(SDL_SCANCODE_S) == KEY_UP && sliding)
		{
			finish_sliding = true;
		}
	}
	else if (App->scene->controls == j1Scene::Controls::ARROWS)
	{
		if ((App->input->GetKey(SDL_SCANCODE_UP) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT) && can_jump)
		{
			jumping = true;
		}
		if ((App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT) && can_slide)
		{
			start_sliding = true;
		}
		if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_UP && sliding)
		{
			start_sliding = false;
			finish_sliding = true;
		}
	}
	else if (App->scene->controls == j1Scene::Controls::UI_BUTTONS)
	{
		if (App->entitycontroller->want_jump && can_jump)
		{
			jumping = true;
		}
		if (App->entitycontroller->want_slide && can_slide)
		{
			start_sliding = true;
		}
		if (App->entitycontroller->want_jump && sliding)
		{
			start_sliding = false;
			finish_sliding = true;
		}
	}
	else if (App->scene->controls == j1Scene::Controls::DRAG_MOUSE)
	{
		
	}
	return true;
}

bool j1Player::Update(float dt)
{
	dy = 0;

	//gravity acting first
	if (!grounded)
	{
		speed.y += gravity;
	}

	if (jumping)
	{
		grounded = false;
		jumping = false;
		is_jump = true;
		LOG("JUMP---------------------------------------------------------------");
		speed.y = -jumpSpeed;
	}

	if (speed.y > 0)
	{
		falling = true;
		is_jump = false;
		LOG("falling");
	}

	if (start_sliding)
	{
		speed.y += gravity;
	}

	if (finish_sliding)
	{
		sliding = false;
		position.y = floor - idle.GetCurrentFrame().h;
	}

	ChangeAnimation();
	dy += speed.y;
	
	return true;
}

bool j1Player::PostUpdate()
{
	position.y += dy;

	PlayerOnFloor();
	PositionCollider();

	if (lives <= 0)
	{
		dead = true;
	}



	//if (dead && !App->scenechange->IsChanging())
	//{
	//	App->scenechange->ChangeMap(App->scene->currentMap, App->scene->fade_time);
	//	App->scene->delay.Start();
	//	App->audio->PlayFx(GAME_OVER);
	//}

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
	if (!dead)
	{
		current_animation = &idle;

		if (is_jump)
		{
			current_animation = &jump;
			LOG("current anim: JUMP-----------------");
		}

		if (falling)
			current_animation = &fall;

		if (start_sliding)
		{
			current_animation = &start_slide;
			LOG("start sliding-----------------------------------------------------------");
			if (current_animation->Finished())
			{
				current_animation = &slide;
				sliding = true;
			}
		}

		if (sliding)
			current_animation = &slide;

		if (finish_sliding)
		{
			current_animation = &finish_slide;
			if (current_animation->Finished())
			{
			LOG("finish sliding------------------------------------------------------------");
				current_animation = &idle;
				finish_sliding = false;
				sliding = false;
			}
		}		
	}
}

void j1Player::LoadAnimations()
{
	this->idle = App->entitycontroller->info->idle;
	this->start_slide = App->entitycontroller->info->start_slide;
	this->slide = App->entitycontroller->info->slide;
	this->start_slide = App->entitycontroller->info->finish_slide;
	this->jump = App->entitycontroller->info->jump;
}

void j1Player::PlayerOnFloor()
{
	if (!grounded)
		LOG("-------------------------------------------------------------------00");
	if (position.y + Collider.h >= floor)
	{
		position.y = floor - Collider.h;
		speed.y = 0;
		falling = false;
		grounded = true;
		LOG("grounded---------------------------------------------------------");
	}
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
				App->audio->PlayFx(DEAD);
			}
		}
		tmp = tmp->next;
	}
}
