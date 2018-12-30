#include "p2Defs.h"
#include "p2Log.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Map.h"
#include "j1Scene.h"
#include "j1SceneChange.h"
#include "j1EntityController.h"
#include "j1Fonts.h"

#include <stdio.h>

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

	fade_time = config.child("fade_time").attribute("value").as_float();

	for (pugi::xml_node map = config.child("map_name"); map; map = map.next_sibling("map_name"))
	{
		p2SString* data = new p2SString;

		data->create(map.attribute("name").as_string());
		map_names.add(data);
	}

	return ret;
}

// Called before the first frame
bool j1Scene::Start()
{
	App->map->Load(map_names.start->data->GetString());

	return true;
}

// Called each loop iteration
bool j1Scene::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool j1Scene::Update(float dt)
{
	App->map->Draw(dt);
	App->entitycontroller->Draw(dt);

	return true;
}

// Called each loop iteration
bool j1Scene::PostUpdate()
{
	
	return true;
}

// Called before quitting
bool j1Scene::CleanUp()
{
	LOG("Freeing scene");

	App->tex->UnLoad(debug_tex);
	
	return true;
}

void j1Scene::SpawnEnemies()
{

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