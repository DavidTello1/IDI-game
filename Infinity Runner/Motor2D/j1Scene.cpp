#include "p2Defs.h"
#include "p2Log.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Map.h"
#include "j1Scene.h"
#include "j1EntityController.h"
#include "j1Fonts.h"
#include "j1Entity.h"
#include "j1Audio.h"

#include <stdio.h>
#include <time.h>
#include "SDL_mixer\include\SDL_mixer.h"

j1Scene::j1Scene() : j1Module()
{
	name.create("scene");
}

// Destructor
j1Scene::~j1Scene()
{}

// Called before render is available
bool j1Scene::Awake(pugi::xml_node& config)
{
	bool ret = true;

	LOG("Loading Scene");

	for (pugi::xml_node map = config.child("map_name"); map; map = map.next_sibling("map_name"))
	{
		p2SString* data = new p2SString;

		data->create(map.attribute("name").as_string());
		map_names.add(data);
	}

	controls = WASD;
	max_obstacles = config.child("max_obstacles").attribute("value").as_uint();
	spacing = config.child("spacing").attribute("value").as_uint();
	pause = false;
	player_dead = false;

	return ret;
}

// Called before the first frame
bool j1Scene::Start()
{
	if (Mix_PausedMusic() > 0)
	{
		Mix_ResumeMusic();
		App->audio->AdjustMusicVol(20);
	}
	App->audio->PlayMusic("audio/music/Endless_runner.ogg");
	App->audio->AdjustMusicVol(50);
	App->audio->AdjustSoundVol(30);

	game_over_tex = App->tex->Load("textures/game_over.png");
	App->map->Load(map_names.start->data->GetString());

	App->entitycontroller->AddEntity(Entity::entityType::PLAYER, true, { 0,0 });
	last_obstacle = App->entitycontroller->AddEntity(Entity::entityType::WALL, true, { App->win->width, App->win->height });
	num_obstacles = 1;
	num_boxes = 0;
	score = 0;

	return true;
}

// Called each loop iteration
bool j1Scene::PreUpdate()
{
	ChangeControls();

	if (num_obstacles < max_obstacles)
	{
		if (last_obstacle != nullptr && last_obstacle->position.x <= App->win->width - spacing)
		{
			srand(time(NULL));
			int entity = rand() % 3;
			bool grounded = ((rand() % 10 + 1) % 2 == 0);

			if (entity == 0)
			{
				last_obstacle = App->entitycontroller->AddEntity(Entity::entityType::WALL, grounded, { App->win->width, App->win->height });
			}
			else if (entity == 1)
			{
				last_obstacle = App->entitycontroller->AddEntity(Entity::entityType::BOX, grounded, { App->win->width, App->win->height });
			}
			else if (entity == 2)
			{
				last_obstacle = App->entitycontroller->AddEntity(Entity::entityType::SAW, grounded, { App->win->width, App->win->height });
			}
			num_obstacles++;
		}
	}
	return true;
}

// Called each loop iteration
bool j1Scene::Update(float dt)
{
	App->map->Draw();
	App->entitycontroller->Draw();

	if (pause)
	{
		App->render->Blit(game_over_tex, App->win->width / 2 - 313, App->win->height / 2 - 250);
		if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
		{
			player_dead = false;
			App->scene->Restart();
			pause = false;
		}
	}
	return true;
}

// Called each loop iteration
bool j1Scene::PostUpdate()
{
	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
	{
		return false;
	}

	if (!pause)
	{
		if (player_dead)
		{
			pause = true;
			App->audio->PauseMusic();
			App->audio->PlayFx(GAME_OVER);

			if (obstacle_dies->type == 1) { obstacle_type = "WALL"; }
			else if (obstacle_dies->type == 2) { obstacle_type = "BOX"; }
			else if (obstacle_dies->type == 3) { obstacle_type = "SAW"; }

			if (score > 0)
			{
				App->SavegameNow();
			}
		}
	}

	//score
	App->tex->UnLoad(score_tex);
	sprintf_s(current_score, "SCORE: %u", score);
	score_tex = App->font->Print(current_score, { 0,0,0,255 });
	App->font->CalcSize(current_score, score_size.x, score_size.y);
	App->render->Blit(score_tex, App->win->width / 2 - score_size.x / 2, 20);

	//controls
	App->tex->UnLoad(controls_tex);
	if (controls == 0) { controls_type = "WS / D"; }
	else if (controls == 1) { controls_type = "Arrows / Space"; }
	else if (controls == 2) { controls_type = "Mouse Click / Right Click"; }
	else if (controls == 3) { controls_type = "Mouse Drag / Right Click"; }
	sprintf_s(current_controls, "CONTROLS: %s", controls_type);
	controls_tex = App->font->Print(current_controls, { 0,0,0,255 });
	App->font->CalcSize(current_controls, controls_size.x, controls_size.y);
	App->render->Blit(controls_tex, 25, 20);

	return true;
}

// Called before quitting
bool j1Scene::CleanUp()
{
	LOG("Freeing scene");	
	App->tex->UnLoad(score_tex);
	App->tex->UnLoad(controls_tex);
	App->tex->UnLoad(game_over_tex);

	return true;
}

void j1Scene::ChangeControls()
{
	if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
	{
		controls = WASD;
		App->audio->PlayFx(CLICK);
	}
	if (App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)
	{
		controls = ARROWS;
		App->audio->PlayFx(CLICK);
	}
	if (App->input->GetKey(SDL_SCANCODE_3) == KEY_DOWN)
	{
		controls = MOUSE_CLICK;
		App->audio->PlayFx(CLICK);
	}
	if (App->input->GetKey(SDL_SCANCODE_4) == KEY_DOWN)
	{
		controls = DRAG_MOUSE;
		App->audio->PlayFx(CLICK);
	}
}

void j1Scene::Restart()
{
	App->tex->UnLoad(game_over_tex);
	App->entitycontroller->DeleteEntities();
	App->map->CleanUp();
	App->map->scroll = 0;
	App->map->scroll2 = 0;
	App->scene->spacing = 265;
	Start();
}

//void j1Scene::UpdateState(UI_Element* data)
//{
//	switch(data->type)
//	{
//	case UI_Element::UI_type::PUSHBUTTON: //push button
//		switch (data->state)
//		{
//		case UI_Element::State::IDLE:
//			data->rect = { 380,139,190,48 };
//			break;
//
//		case UI_Element::State::HOVER:
//			data->rect = { 190,139,190,48 };
//			break;
//
//		case UI_Element::State::LOGIC:
//			data->rect = { 190,90,190,48 };
//			break;
//		}
//		break;
//
//	case UI_Element::UI_type::SLIDER: //slider
//		if (data->action == UI_Element::Action::ADJUST_FX)
//		{
//			switch (data->state)
//			{
//			case UI_Element::State::IDLE:
//			{
//				data->rect = { 0,0,26,26 };
//			}
//			break;
//
//			case UI_Element::State::HOVER:
//			{
//				data->rect = { 52,0,26,26 };
//			}
//			break;
//
//			case UI_Element::State::DRAG:
//			{
//				data->rect = { 104,0,26,26 };
//			}
//			break;
//			}
//			break;
//		}
//		else if (data->action == UI_Element::Action::ADJUST_VOL)
//		{
//			switch (data->state)
//			{
//			case UI_Element::State::IDLE:
//			{
//				data->rect = { 26,0,26,26 };
//			}
//			break;
//
//			case UI_Element::State::HOVER:
//			{
//				data->rect = { 78,0,26,26 };
//			}
//			break;
//
//			case UI_Element::State::DRAG:
//			{
//				data->rect = { 130,0,26,26 };
//			}
//			break;
//			}
//			break;
//		}
//	}
//}
//
//void j1Scene::UpdateSimpleUI()
//{
//	p2List_item<UI_Element*>* item = App->gui->UI_elements.start;
//	while (item != nullptr)
//	{
//		switch (item->data->type)
//		{
//		case UI_Element::UI_type::IMAGE: //image
//
//			if (item->data == ui_life1) //Life 1
//			{
//				if (player_lives >= 2)
//				{
//					item->data->rect = { 192,0,30,25 }; //full
//					break;
//				}
//				else if (player_lives == 1)
//				{
//					item->data->rect = { 221,0,30,25 }; //half
//					break;
//				}
//				else
//				{
//					item->data->rect = { 251,0,30,25 }; //empty
//					break;
//				}
//			}
//			else if (item->data == ui_life2) //Life 2
//			{
//				if (player_lives >= 4)
//				{
//					item->data->rect = { 192,0,30,25 }; //full
//					break;
//				}
//				else if (player_lives == 3)
//				{
//					item->data->rect = { 221,0,30,25 }; //half
//					break;
//				}
//				else
//				{
//					item->data->rect = { 251,0,30,25 }; //empty
//					break;
//				}
//			}
//			else if (item->data == ui_life3) //Life 3
//			{
//				if (player_lives == 6)
//				{
//					item->data->rect = { 192,0,30,25 }; //full
//					break;
//				}
//				else if (player_lives == 5)
//				{
//					item->data->rect = { 221,0,30,25 }; //half
//					break;
//				}
//				else
//				{
//					item->data->rect = { 251,0,30,25 }; //empty
//					break;
//				}
//			}
//			else if (item->data == ui_coins) //Coins
//			{
//				item->data->rect = { 192,25,24,24 };
//				break;
//			}
//			else if (item->data == ui_game_over)
//			{
//				item->data->rect = { 0,187,626,336 };
//				break;
//			}
//			else if (item->data == ui_game_win)
//			{
//				item->data->rect = { 0,524,1003,476 };
//				break;
//			}
//
//		case UI_Element::UI_type::TEXT: //text
//
//			if (item->data == ui_coins_text) //coins number
//			{
//				sprintf_s(current_coins, "x%u", coins);
//				item->data->label = current_coins;
//				break;
//			}
//			else if (item->data == ui_score) //score
//			{
//				sprintf_s(current_score, "SCORE: %u", score);
//				item->data->label = current_score;
//				break;
//			}
//			else if (item->data == ui_timer && pause == false) //timer
//			{
//				if (countdown <= 60)
//				{
//					item->data->color = { 255,0,0,255 };
//				}
//				else
//				{
//					item->data->color = { 255,255,255,255 };
//				}
//
//				if (timer.ReadSec() >= 1)
//				{
//					timer.Start();
//					countdown--;
//				}
//				sprintf_s(current_time, "TIME: %u", countdown);
//				item->data->label = current_time;
//				break;
//			}
//			else if (item->data == volume_text) //volume
//			{
//				sprintf_s(current_vol, "MUSIC: %d", App->main_menu->vol_value);
//				item->data->label = current_vol;
//				break;
//			}
//			else if (item->data == sfx_text) //sfx
//			{
//				sprintf_s(current_sfx, "SOUND: %d", App->main_menu->sfx_value);
//				item->data->label = current_sfx;
//				break;
//			}
//		}
//		item = item->next;
//	}
//}