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
#include "Brofiler\Brofiler.h"


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
	BROFILER_CATEGORY("Player Update", Profiler::Color::Black);

	if (!dead)
	{
		PositionCollider();
		Collider_Overlay();
		final_speed = { 0,0 };

		if (App->input->GetKey(SDL_SCANCODE_N) == KEY_DOWN) //change map
		{
			if (App->scene->change == false && gripped == false && attack == false && jumping == false)
			{
				pugi::xml_document save_data;
				pugi::xml_parse_result result = save_data.load_file("save_game.xml");
				if (result == false)
				{
					App->SaveGame();
				}

				App->map->angle = 0.0;
				App->map->rotate = true;
				App->map->rotate_end = false;
				App->scene->change = true;
			}
		}

		if (App->input->GetKey(SDL_SCANCODE_M) == KEY_DOWN) //attack
		{
			if (attack == false && attack_able == true && gripped == false)
			{
				attack = true;
				attack_able = false;
				grid = false;
				jumping = false;
			}
			if (App->scene->godmode == true)
			{
				attack_able = true;
			}
		}

		if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) //jump
		{
			if (attack == false && App->scene->godmode == false && box_moving == false && jump_able == true)
			{
				if (jumping == false)
				{
					jumping = true;
					grounded = false;
					sliding = false;
					grid = false;
					landed = false;
					attack_able = true;
					jump_able = false;
					jumpSpeed = speed.y;
				}
			}
		}

		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) //left
		{
			if (attack == false  || App->scene->godmode == true)
			{
				flip = false;
				if (gripped == false)
				{
					if (grid == true)
					{
						grid_moving = true;
						final_speed.x -= speed.x / 2;
					}
					else
					{
						left = true;
						final_speed.x -= speed.x;
						if (wall_left == true && grounded == false)
						{
							sliding = true;
						}
					}
				}
			}
		}

		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) //right
		{
			if (attack == false && gripped == false || App->scene->godmode == true)
			{
				flip = true;

				if (gripped == false)
				{
					if (grid == true) {
						grid_moving = true;
						final_speed.x += speed.x / 2;
					}
					else
					{
						right = true;
						final_speed.x += speed.x;
						if (wall_right == true && grounded == false)
						{
							sliding = true;
						}
					}
				}
			}
		}

		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) //up
		{
			if (grid == true && ceiling == false && top_grid == false)
			{
				grid_moving = true;
				final_speed.y -= speed.x / 2;
			}
			else if (App->scene->godmode == true)
			{
				final_speed.y -= speed.x;
			}
		}

		if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) //down
		{
			if (grid == true)
			{
				grid_moving = true;
				final_speed.y += speed.x / 2;
			}
			else if (App->scene->godmode == true)
			{
				final_speed.y += speed.x;
			}
		}

		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN) //grip
		{
			if (grid_collision == true && grid == false)
			{
				gripping = true;
				grid = true;
				jumping = false;
				attack_able = true;
			}
			else if (box_collision == true && gripped == false)
			{
				gripping = true;
				gripped = true;
				jumping = false;
				attack = false;
				attack_able = false;
			}
		}

		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_UP) //left release
		{
			if (sliding == true)
			{
				jump_able = true;
			}
			left = false;
			sliding = false;
			grid_moving = false;
		}
		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_UP) //right release
		{
			if (sliding == true)
			{
				jump_able = true;
			}
			right = false;
			sliding = false;
			grid_moving = false;
		}
		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_UP) //up release
		{
			grid_moving = false;
		}
		if (App->input->GetKey(SDL_SCANCODE_S) == KEY_UP) //down release
		{
			grid_moving = false;
		}


		//----------------	
		if (attack == true) //attack
		{
			if (flip)
			{
				final_speed.x += speed.x;
			}
			else
			{
				final_speed.x -= speed.x;
			}
			jumpSpeed = 0;
			final_speed.y = 0;
		}

		if (grounded == true) //grounded
		{
			jumping = false;
			jump_able = true;
			sliding = false;
			attack_able = true;
		}

		if (box_collision == false) //box collision
		{
			gripped = false;
			box_moving = false;
		}

		if (gripped == true) //box gripped
		{
			gravity_active = false;
			jump_able = true;
			grounded = false;
			sliding = false;
			landed = false;

			if (box_moving == true)
			{
				position = box_position;
			}
			else
			{
				attack_able = true;
			}
		}

		if (grid_collision == false) //grid collision
		{
			grid = false;
		}

		if (grid_collision == false && box_collision == false)
		{
			gravity_active = true;
		}

		if (grid == true) //grid
		{
			gravity_active = false;
			jump_able = true;
			grounded = false;
			sliding = false;
			landed = false;

			if (!is_static)
			{
				if (vertical == true)
				{
					current_speed.y += grid_speed.y*dt;
					if (current_speed.y >= 1)
					{
						if (flip_ver == true) //hide_up
						{
							if (grid_direction == 1)
							{
								position.y++;
							}
							else if (grid_direction == -1 && ceiling == false)
							{
								position.y--;
							}
						}
						else if (flip_ver == false) //hide_down
						{
							if (grid_direction == 1)
							{
								position.y--;
							}
							else if (grid_direction == -1)
							{
								position.y++;
							}
						}
						current_speed.y--;
					}
				}
				if (vertical == false)
				{
					current_speed.x += grid_speed.x*dt;
					if (current_speed.x >= 1)
					{
						if (flip_hor == true) //hide_left
						{
							if (grid_direction == 1)
							{
								position.x++;
							}
							else if (grid_direction == -1 && wall_left == false)
							{
								position.x--;
							}
						}
						else if (flip_hor == false) //hide_right
						{
							if (grid_direction == 1)
							{
								position.x--;
							}
							else if (grid_direction == -1 && wall_right == false)
							{
								position.x++;
							}
						}
						current_speed.x--;
					}
				}
			}
		}

		if (jumping == true && App->scene->godmode == false) //jumping
		{
			gravity_active = false;
			jumpSpeed -= gravity / 5;
			if (jumpSpeed <= -gravity)
			{
				jumpSpeed = -gravity;
			}
			final_speed.y -= jumpSpeed;
		}

		if (sliding == true && left == true && wall_left == false ||
			sliding == true && right == true && wall_right == false)
		{
			sliding = false;
		}

		if (sliding == true) //sliding
		{
			grounded = false;
			grid = false;
			jumping = false;
		}

		if (wall_left == true && App->scene->godmode == false) //wall left
		{
			if (final_speed.x < 0)
			{
				final_speed.x = 0;
			}
			wall_left = false;
		}

		if (wall_right == true && App->scene->godmode == false) //wall right
		{
			if (final_speed.x > 0)
			{
				final_speed.x = 0;
			}
			wall_right = false;
		}

		if (ceiling == true) //ceiling
		{
			jumpSpeed = 0;
			if (final_speed.y < 0)
			{
				final_speed.y = 0;
			}
			ceiling = false;
		}

		if (gravity_active == true) //gravity
		{
			if (App->scene->godmode == false)
			{
				if (grounded == false && grid == false && attack == false)
				{
					if (sliding == true)
					{
						final_speed.y += gravity / 3;
					}
					if (sliding == false)
					{
						final_speed.y += gravity;
					}
				}
			}
		}

		//limits
		if (position.x + ceilf(final_speed.x*dt) <= 0 || position.x + ceilf(final_speed.x*dt) > (App->map->data.width - 1)*App->map->data.tile_width) //x axis
		{
			final_speed.x = 0;
		}
		if (position.y + ceilf(final_speed.y*dt) <= 0 || position.y + ceilf(final_speed.y*dt) > (App->map->data.height - 1)*App->map->data.tile_height) //y axis
		{
			final_speed.y = 0;
		}

		if (final_speed.x > 0) //going right
		{
			position.x += ceilf(final_speed.x*dt);
		}
		else if (final_speed.x < 0) //going left
		{
			position.x -= ceilf(abs(final_speed.x*dt));
		}

		if (final_speed.y > 0) //going down
		{
			position.y += ceilf(final_speed.y*dt);
		}
		else if (final_speed.y < 0) //going up
		{
			position.y -= ceilf(abs(final_speed.y*dt));
		}


		//Animations and vars reset
		if (landing == true && landing_anim.Finished())
		{
			landing_anim.Reset();
			landing_anim.ResetLoops();
			landed = true;
			landing = false;
		}
		if (gripping == true && grip_anim.Finished())
		{
			grip_anim.Reset();
			grip_anim.ResetLoops();
			gripping = false;
		}
		if (attacking.Finished())
		{
			attacking.Reset();
			attacking.ResetLoops();
			attack = false;
			gravity_active = true;
		}

		ChangeAnimation();

		grounded = false;
		grid_collision = false;
		box_collision = false;
		top_grid = false;
	}
	else
	{
		ChangeAnimation();
		left = right = attack = jumping = grid = box_moving = sliding = false;
	}
	CameraOnPlayer();

	return true;
}

bool j1Player::PostUpdate()
{
	BROFILER_CATEGORY("Player PostUpdate", Profiler::Color::Black);

	PositionCollider();

	if (dead && !App->scenechange->IsChanging())
	{
		App->scene->player_lives--;
		if (App->scene->player_lives > 0)
		{
			App->audio->PlayFx(DEAD);
			App->scene->ResetBoxPos();
			App->scene->ResetEnemies();
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

	player.append_child("size").append_attribute("width") = size.x;
	player.child("size").append_attribute("height") = size.y;
	player.append_child("position").append_attribute("x") = position.x;
	player.child("position").append_attribute("y") = position.y;
	player.append_child("gravity").append_attribute("value") = gravity;
	player.append_child("speed").append_attribute("x") = speed.x;
	player.child("speed").append_attribute("y") = speed.y;
	player.append_child("grounded").append_attribute("value") = grounded;
	player.append_child("sliding").append_attribute("value") = sliding;
	player.append_child("jumping").append_attribute("value") = jumping;
	player.append_child("grid").append_attribute("value") = grid;
	player.append_child("gripped").append_attribute("value") = gripped;
	player.append_child("gravity_active").append_attribute("value") = gravity_active;

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
	if (!dead)
	{
		if (gravity_active)
		{
			Current_Animation = &falling_anim;
		}
		if (grounded && landed)
		{
			if (!left && !right)
			{
				Current_Animation = &idle;
			}
			else
			{
				Current_Animation = &move_anim;
			}
		}
		if (jumping)
		{
			if (jumpSpeed > 0)
			{
				Current_Animation = &jump_anim;
			}
			else
			{
				Current_Animation = &falling_anim;
			}
		}
		if (grid && !gripping)
		{
			if (!grid_moving)
			{
				Current_Animation = &grip_idle;
			}
			else
			{
				Current_Animation = &grip_move;
			}
		}
		if (box_moving && !gripping)
		{
			Current_Animation = &grip_idle;
		}
		if (sliding)
		{
			Current_Animation = &slide_anim;
		}
		if (landing)
		{
			Current_Animation = &landing_anim;
		}
		if (gripping)
		{
			Current_Animation = &grip_anim;
		}
		if (attack)
		{
			Current_Animation = &attacking;
		}
	}
	else
	{
		Current_Animation = &dying;
	}
}

void j1Player::LoadAnimations()
{
	idle.PushBack({ 0, 0, size.x, size.y });
	idle.PushBack({ 32, 0, size.x, size.y });
	idle.PushBack({ 64, 0, size.x, size.y });
	idle.PushBack({ 96, 0, size.x, size.y });
	idle.loop = true;
	idle.speed = 6.0f;

	move_anim.PushBack({ 0, 32, size.x, size.y });
	move_anim.PushBack({ 32, 32, size.x, size.y });
	move_anim.PushBack({ 64, 32, size.x, size.y });
	move_anim.PushBack({ 96, 32, size.x, size.y });
	move_anim.PushBack({ 128, 32, size.x, size.y });
	move_anim.PushBack({ 160, 32, size.x, size.y });
	move_anim.PushBack({ 192, 32, size.x, size.y });
	move_anim.PushBack({ 224, 32, size.x, size.y });
	move_anim.loop = true;
	move_anim.speed = 8.0f;

	jump_anim.PushBack({ 224, 64, size.x, size.y });
	jump_anim.PushBack({ 192, 64, size.x, size.y });
	jump_anim.loop = true;
	jump_anim.speed = 6.0f;

	falling_anim.PushBack({ 160, 64, size.x, size.y });
	falling_anim.PushBack({ 128, 64, size.x, size.y });
	falling_anim.loop = false;
	falling_anim.speed = 6.0f;

	landing_anim.PushBack({ 96, 64, size.x, size.y });
	landing_anim.PushBack({ 64, 64, size.x, size.y });
	landing_anim.PushBack({ 32, 64, size.x, size.y });
	landing_anim.PushBack({ 0, 64, size.x, size.y });
	landing_anim.loop = false;
	landing_anim.speed = 6.0f;

	grip_idle.PushBack({ 160, 0, size.x, size.y });

	grip_anim.PushBack({ 128, 64, size.x, size.y });
	grip_anim.PushBack({ 224, 96, size.x, size.y });
	grip_anim.PushBack({ 128, 0, size.x, size.y });
	grip_anim.PushBack({ 192, 0, size.x, size.y });
	grip_anim.loop = false;
	grip_anim.speed = 12.5f;

	grip_move.PushBack({ 224, 0, size.x, size.y });
	grip_move.PushBack({ 192, 0, size.x, size.y });
	grip_move.PushBack({ 160, 0, size.x, size.y });
	grip_move.PushBack({ 160, 96, size.x, size.y });
	grip_move.PushBack({ 192, 96, size.x, size.y });
	grip_move.loop = true;
	grip_move.speed = 8.0f;

	slide_anim.PushBack({ 128, 96, size.x, size.y });

	dying.PushBack({ 64, 96, size.x, size.y });

	attacking.PushBack({ 0, 160, size.x, size.y });
	attacking.PushBack({ 384, 160, size.x, size.y });
	attacking.PushBack({ 320, 160, size.x, size.y });
	attacking.PushBack({ 256, 160, size.x, size.y });
	attacking.PushBack({ 224, 160, size.x, size.y });
	attacking.PushBack({ 192, 160, size.x, size.y });
	attacking.PushBack({ 128, 160, size.x, size.y });
	attacking.PushBack({ 64, 160, size.x, size.y });
	attacking.PushBack({ 0, 160, size.x, size.y });
	attacking.loop = false;
	attacking.speed = 15.0f;
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
