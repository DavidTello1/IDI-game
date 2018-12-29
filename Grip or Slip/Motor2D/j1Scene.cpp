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


bool j1Scene::Load(pugi::xml_node& data)
{
	LOG("IsChanging: %i", App->scenechange->IsChanging());

	player_lives = data.child("player_lives").attribute("value").as_int();
	coins = data.child("coins").attribute("value").as_uint();
	score = data.child("score").attribute("value").as_uint();
	countdown = data.child("countdown").attribute("value").as_uint();
	box_1_side = data.child("box_1_side").attribute("left").as_bool();
	box_2_side = data.child("box_2_side").attribute("left").as_bool();
	box_3_side = data.child("box_3_side").attribute("left").as_bool();
	box_4_side = data.child("box_4_side").attribute("left").as_bool();

	if (currentMap != data.child("currentMap").attribute("num").as_int())
	{
		LOG("Calling switch maps");
		currentMap = data.child("currentMap").attribute("num").as_int();
		App->map->SwitchMaps(map_names[data.child("currentMap").attribute("num").as_int()]);
	}
	
	return true;
}

bool j1Scene::Save(pugi::xml_node& data) const
{
	data.append_child("currentMap").append_attribute("num") = currentMap;
	data.append_child("player_lives").append_attribute("value") = player_lives;
	data.append_child("coins").append_attribute("value") = coins;
	data.append_child("score").append_attribute("value") = score;
	data.append_child("countdown").append_attribute("value") = countdown;
	data.append_child("box_1_side").append_attribute("left") = box_1_side;
	data.append_child("box_2_side").append_attribute("left") = box_2_side;
	data.append_child("box_3_side").append_attribute("left") = box_3_side;
	data.append_child("box_4_side").append_attribute("left") = box_4_side;

	return true;
}

bool j1Scene::Load_level(int map)
{
	App->entitycontroller->DeleteEntities();
	App->map->SwitchMaps(map_names[map]);
	SpawnEntities();

	return true;
}

void j1Scene::SpawnEntities()
{
	App->entitycontroller->DeleteEntities();
	SpawnEnemies();

	pugi::xml_document	config_file;
	pugi::xml_node		config;

	config = App->LoadConfig(config_file);

	Entity* player = App->entitycontroller->AddEntity(Entity::entityType::PLAYER, { 0,0 }, { 0,0 });
	player->Awake(config.child(App->entitycontroller->name.GetString()));
	player->Start();
}

void j1Scene::ResetEnemies()
{
	p2List_item<Entity*>* tmp = App->entitycontroller->Entities.start;
	while (tmp != nullptr)
	{
		if (tmp->data->type == Entity::entityType::FLYING_ENEMY || tmp->data->type == Entity::entityType::LAND_ENEMY && tmp->data->dead == false)
		{
			tmp->data->position = tmp->data->initial_pos;
		}
		tmp = tmp->next;
	}
}


void j1Scene::SpawnEnemies()
{
	for (p2List_item<ObjectsGroup*>* object = App->map->data.objLayers.start; object; object = object->next)
	{
		if (object->data->name == ("Enemies"))
		{
			for (p2List_item<ObjectsData*>* objectdata = object->data->objects.start; objectdata; objectdata = objectdata->next)
			{
				if (objectdata->data->name == "FlyingEnemy")
				{
					App->entitycontroller->AddEntity(Entity::entityType::FLYING_ENEMY, { objectdata->data->x, objectdata->data->y }, { 0,0 });
				}

				else if (objectdata->data->name == "LandEnemy")
				{
					App->entitycontroller->AddEntity(Entity::entityType::LAND_ENEMY, { objectdata->data->x, objectdata->data->y }, { 0,0 });
				}
			}
		}
		if (object->data->name == ("Collision"))
		{
			for (p2List_item<ObjectsData*>* objectdata = object->data->objects.start; objectdata; objectdata = objectdata->next)
			{
				if (objectdata->data->name == "Grid" && objectdata->data->type != "Static") 
				{
					App->entitycontroller->AddEntity(Entity::entityType::GRID, { objectdata->data->x, objectdata->data->y }, { objectdata->data->width, objectdata->data->height }, objectdata->data->type);
				}
				else if (objectdata->data->name == "Box1" || objectdata->data->name == "Box2" || objectdata->data->name == "Box3" || objectdata->data->name == "Box4")
				{
					App->entitycontroller->AddEntity(Entity::entityType::BOX, { objectdata->data->x, objectdata->data->y }, { objectdata->data->width, objectdata->data->height }, objectdata->data->name, objectdata->data->type);
				}
				else if (objectdata->data->name == "Coin")
				{
					App->entitycontroller->AddEntity(Entity::entityType::COIN, { objectdata->data->x, objectdata->data->y }, { objectdata->data->width, objectdata->data->height });
				}
			}
		}
	}
}

void j1Scene::SaveAndChange()
{
	pugi::xml_document data;  //savegame.xml data
	pugi::xml_node root;
	pugi::xml_parse_result result = data.load_file("save_game.xml");
	data.child("game_state").child("scene").child("currentMap").attribute("num").set_value(currentMap);
	data.child("game_state").child("scene").child("coins").attribute("value").set_value(coins);
	data.child("game_state").child("scene").child("score").attribute("value").set_value(score);
	data.child("game_state").child("scene").child("countdown").attribute("value").set_value(countdown);
	data.save_file("save_game.xml");

	root = data.child("game_state").child("entitycontroller");

	pugi::xml_document copy_data; //copysave.xml data
	pugi::xml_node file;
	file = copy_data.append_child("game_state");

	App->entitycontroller->CopySave(file); //write currentmap data to copysave.xml
	copy_data.save_file("copysave.xml");

	if (currentMap == 0) //change to map2
	{
		currentMap = 1; //switch to and load map2
		App->entitycontroller->DeleteEntities();
		App->map->SwitchMaps2(map_names[1]);
		SpawnEntities();

		if (root.child("map_2").child("player") != NULL) // if map2 data exists loads it from savegame.xml
		{
			App->entitycontroller->Load(root);
			root = data.child("game_state").child("entitycontroller");
		}
	}
	else if (currentMap == 1) //change to map1
	{
		currentMap = 0; //switch to and load map1
		App->entitycontroller->DeleteEntities();
		App->map->SwitchMaps2(map_names[0]);
		SpawnEntities();

		if (root.child("map_1").child("player") != NULL) //if map1 data exists loads it from savegame.xml
		{
			App->entitycontroller->Load(root);
			root = data.child("game_state").child("entitycontroller");
		}
	}

	file = copy_data.child("game_state");
	App->entitycontroller->AppendSave(file, root); //write copysave.xml data to savegame.xml
	data.save_file("save_game.xml");
	copy_data.reset();
	data.reset();
}

void j1Scene::ResetBoxPos()
{
 	box_1_side = false;
	box_2_side = false;
	box_3_side = true;
	box_4_side = false;
}

void j1Scene::ResetIngameUI()
{
	score = 0;
	coins = 0;
	countdown = 300;
	player_lives = 6;
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