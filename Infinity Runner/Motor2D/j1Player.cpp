#include "j1App.h"
#include "j1Player.h"
#include "j1Audio.h"
#include "j1Render.h"
#include "j1Map.h"
#include "j1Window.h"
#include "j1Textures.h"
#include "j1Input.h"
#include "j1Scene.h"
#include "j1EntityController.h"
#include "p2Log.h"

j1Player::j1Player() : Entity(entityType::PLAYER)
{
	jumping = false;
	sliding = false;
	falling = false;
	grounded = true;
	dead = false;
	is_jump = false;

	speed.y = 0;
	gravity = 1.2f;
	jumpSpeed = 25;
	
	idle.PushBack({ 0,0,64,64 });
	jump.PushBack({ 128,0,64,64 });
	fall.PushBack({ 64,64,64,64 });
	die.PushBack({ 64,0,64,64 });


	slide.PushBack({ 128,0,64,32 });
	slide.PushBack({ 128,0,64,32 });
	slide.PushBack({ 128,0,64,32 });
	slide.loop = false;
	slide.speed = 0.03f;

	attacking.PushBack({ 192,0,64,64 });
	attacking.PushBack({ 192,0,64,64 });
	attacking.PushBack({ 192,0,64,64 });
	attacking.loop = false;
	attacking.speed = 0.05f;

	//LoadAnimations();
	current_animation = &idle;
	current_animation->speed = 0.5f;

	Collider.x = position.x;
	Collider.y = position.y;
	Collider.w = current_animation->GetCurrentFrame().w;
	Collider.h = current_animation->GetCurrentFrame().h;

	floor = App->win->height - 78;

	position.x = 150;
	position.y = floor;

	LOG("player created");
}

j1Player::~j1Player()
{
}

bool j1Player::PreUpdate()
{
	bool can_jump = (!jumping && !falling && !sliding && grounded);
	bool can_slide = (!jumping && !falling && grounded);

	if (App->scene->controls == j1Scene::Controls::WASD)
	{
		if ((App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) && can_jump)
		{
			App->audio->PlayFx(JUMP);
			jumping = true;
		}
		if ((App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) && can_slide)
		{
			sliding = true;
			attack = false;
		}
		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_DOWN && !sliding)
		{
			attack = true;
		}
		if (App->input->GetKey(SDL_SCANCODE_S) == KEY_UP || slide.Finished())
		{
			position.y = floor - idle.GetCurrentFrame().h;
			sliding = false;
			slide.Reset();
			slide.ResetLoops();
		}
		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_UP || attacking.Finished())
		{
			attack = false;
			attacking.Reset();
			attacking.ResetLoops();
		}

	}
	else if (App->scene->controls == j1Scene::Controls::ARROWS)
	{
		if ((App->input->GetKey(SDL_SCANCODE_UP) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT) && can_jump)
		{
			App->audio->PlayFx(JUMP);
			jumping = true;
		}
		if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_DOWN && can_slide)
		{
			sliding = true;
			attack = false;
		}
		if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && !sliding)
		{
			attack = true;
		}
		if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_UP || slide.Finished())
		{
			position.y = floor - idle.GetCurrentFrame().h;
			sliding = false;
			slide.Reset();
			slide.ResetLoops();
			sliding = false;
		}
		if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_UP || attacking.Finished())
		{
			attack = false;
			attacking.Reset();
			attacking.ResetLoops();
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
			sliding = true;
		}
		if (App->entitycontroller->want_jump && sliding)
		{
			sliding = false;
		}
	}
	else if (App->scene->controls == j1Scene::Controls::DRAG_MOUSE)
	{
		
	}
	return true;
}

bool j1Player::Update(float dt)
{
	Collider_Overlay();
	dy = 0;

	//gravity acting first
	if (!grounded)
	{
		speed.y += gravity;
	}

	if (sliding)
	{
		position.y = floor - slide.GetCurrentFrame().h;
	}

	if (jumping)
	{
		grounded = false;
		jumping = false;
		is_jump = true;
		speed.y = -jumpSpeed;
	}

	if (speed.y > 0)
	{
		falling = true;
		is_jump = false;
	}

	ChangeAnimation();
	PositionCollider();
	dy += speed.y;
	
	return true;
}

bool j1Player::PostUpdate()
{
	position.y += dy;
	PlayerOnFloor();
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
			current_animation = &jump;

		if (falling)
			current_animation = &fall;

		if (sliding)
			current_animation = &slide;

		if (attack)
			current_animation = &attacking;
	}
	else
	{
		current_animation = &die;
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
		if (SDL_HasIntersection(&Collider, &tmp->data->Collider) && tmp->data->type != entityType::PLAYER)
		{
			if (tmp->data->type == Entity::entityType::BOX)
			{
				if (attack == true)
				{
					tmp->data->dead = true;
					App->audio->PlayFx(DEAD);
				}
				else
				{
					dead = true;
					App->scene->player_dead = true;
					App->scene->obstacle_dies = tmp->data;
				}
			}
			else
			{
				dead = true;
				App->scene->player_dead = true;
				App->scene->obstacle_dies = tmp->data;
			}
		}
		tmp = tmp->next;
	}
}
