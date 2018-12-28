#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Map.h"
#include "j1Pathfinding.h"
#include "j1Scene.h"
#include "j1MainMenu.h"
#include "j1SceneChange.h"
#include "j1EntityController.h"
#include "j1Gui.h"
#include "j1Fonts.h"
#include "UI_Element.h"
#include "InteractiveButton.h"
#include "JustSimpleUI.h"

#include "Brofiler\Brofiler.h"
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
	bool ret = false;

	current_track = App->audio->tracks_path.start;
	App->audio->PlayMusic(PATH(App->audio->folder_music.GetString(), current_track->data.GetString()));

	App->audio->AdjustMusicVol(App->main_menu->vol_value);
	App->audio->AdjustSoundVol(App->main_menu->sfx_value);

	pause = false;
	to_end = false;
	App->map->Load(map_names.start->data->GetString());
	currentMap = 0;

	//walkability map
	int w, h;
	uchar* data = NULL;
	if (App->map->CreateWalkabilityMap(w, h, &data))
	{
		App->pathfinding->SetMap(w, h, data);

		LOG("Create walkability");
	}
	RELEASE_ARRAY(data);
	debug_tex = App->tex->Load("maps/pathfinding.png");

	ResetBoxPos();
	ResetIngameUI();
	SpawnEntities();

	//create gui

	//IN-GAME UI ---
	ui_life1 = App->gui->AddUIElement(UI_Element::UI_type::IMAGE, UI_Element::Action::NONE, { 5,3 }, { 29,25 }, nullptr, true);
	ui_life2 = App->gui->AddUIElement(UI_Element::UI_type::IMAGE, UI_Element::Action::NONE, { 37,3 }, { 29,25 }, nullptr, true);
	ui_life3 = App->gui->AddUIElement(UI_Element::UI_type::IMAGE, UI_Element::Action::NONE, { 69,3 }, { 29,25 }, nullptr, true);
	ui_coins = App->gui->AddUIElement(UI_Element::UI_type::IMAGE, UI_Element::Action::NONE, { 200,5 }, { 24,24 }, nullptr, true);
	ui_game_over = App->gui->AddUIElement(UI_Element::UI_type::IMAGE, UI_Element::Action::NONE, { App->win->width / 2 - 313, App->win->height / 2 - 134 }, { 0,0 }, nullptr, false, { false, false });
	ui_game_win = App->gui->AddUIElement(UI_Element::UI_type::IMAGE, UI_Element::Action::NONE, { App->win->width / 2 - 502, App->win->height / 2 - 250 }, { 0,0 }, nullptr, false, { false, false });
	ui_coins_text = App->gui->AddUIElement(UI_Element::UI_type::TEXT, UI_Element::Action::NONE, { 230,10 }, { 0,0 }, nullptr, true, { false, false }, "x0");
	ui_score = App->gui->AddUIElement(UI_Element::UI_type::TEXT, UI_Element::Action::NONE, { App->win->width / 2 - 50, 10 }, { 0,0 }, nullptr, true, { false, false }, "Score: 0");
	ui_timer = App->gui->AddUIElement(UI_Element::UI_type::TEXT, UI_Element::Action::NONE, { 950,10 }, { 0,0 }, nullptr, true, { false, false }, "Timer: 0s");

	ui_coins_text->color = { 255,255,255,255 }; //white
	ui_score->color = { 255,255,255,255 };
	ui_timer->color = { 255,255,255,255 };

	coins_bg = { 193,3,70,30 };
	score_bg = { App->win->width/2 - 57, 5,85,25 };
	timer_bg = { 943,6,70,25 };

	//PAUSE MENU ---
	pause_window = App->gui->AddUIElement(UI_Element::UI_type::WINDOW, UI_Element::Action::NONE, { App->render->viewport.w / 2 - 187, App->render->viewport.h / 2 - 212 }, { 375,425 }, nullptr, false);
	pause_window_text = App->gui->AddUIElement(UI_Element::UI_type::TEXT, UI_Element::Action::NONE, { 0, 40 }, { 0,0 }, pause_window, false, { false, false }, "*PAUSE*");
	pause_window->rect = { 628,0,375,425 };

	//RESUME
	resume_button = App->gui->AddUIElement(UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::RESUME, { 0, 350 }, { 190,48 }, pause_window, false);
	resume_text = App->gui->AddUIElement(UI_Element::UI_type::TEXT, UI_Element::Action::NONE, { 0,0 }, { 0,0 }, resume_button, false, { false, false }, "RESUME");

	//SETTINGS
	vol_slider_bar = App->gui->AddUIElement(UI_Element::UI_type::BACKGROUND, UI_Element::Action::NONE, { 0, 230 }, { 148,18 }, pause_window, false);
	sfx_slider_bar = App->gui->AddUIElement(UI_Element::UI_type::BACKGROUND, UI_Element::Action::NONE, { 0, 300 }, { 148,18 }, pause_window, false);
	vol_slider_bar->rect = { 0,26,158,18 };
	sfx_slider_bar->rect = { 0,44,158,18 };
	volume_text = App->gui->AddUIElement(UI_Element::UI_type::TEXT, UI_Element::Action::NONE, { 0, 200 }, { 0,0 }, pause_window, false, { false, false }, "MUSIC: 0");
	sfx_text = App->gui->AddUIElement(UI_Element::UI_type::TEXT, UI_Element::Action::NONE, { 0, 270 }, { 0,0 }, pause_window, false, { false, false }, "SOUND EFFECTS: 0");
	vol_slider_circle = App->gui->AddUIElement(UI_Element::UI_type::SLIDER, UI_Element::Action::ADJUST_VOL, { App->render->viewport.w / 2 - 79, 368 }, { 26,26 }, nullptr, false, { true, false });
	sfx_slider_circle = App->gui->AddUIElement(UI_Element::UI_type::SLIDER, UI_Element::Action::ADJUST_FX, { App->render->viewport.w / 2 - 79, 438 }, { 26,26 }, nullptr, false, { true, false });

	//SAVE
	save_button = App->gui->AddUIElement(UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::SAVE, { 0, 140 }, { 190,48 }, pause_window, false);
	save_text = App->gui->AddUIElement(UI_Element::UI_type::TEXT, UI_Element::Action::NONE, { 0,0 }, { 0,0 }, save_button, false, { false, false }, "SAVE");

	//MAIN MENU
	main_menu_button = App->gui->AddUIElement(UI_Element::UI_type::PUSHBUTTON, UI_Element::Action::MAIN_MENU, {  0, 80 }, { 190,48 }, pause_window, false);
	main_menu_text = App->gui->AddUIElement(UI_Element::UI_type::TEXT, UI_Element::Action::NONE, { 0,0 }, { 0,0 }, main_menu_button, false, { false, false }, "MAIN MENU");


	//init
	coins = 0;
	score = 0;
	countdown = 300;
	player_lives = 6;
	finish_1 = false;
	finish_2 = false;
	end_game = false;
	timer.Start();
	pause_window_text->color = volume_text->color = sfx_text->color =  save_text->color = main_menu_text->color = resume_text->color = { 0,0,0,255 };

	return true;
}

// Called each loop iteration
bool j1Scene::PreUpdate()
{
	BROFILER_CATEGORY("Scene PreUpdate", Profiler::Color::DarkOrange);

	//LOG("IsChanging: %i", App->scenechange->IsChanging());

	if (pause == true && end_game == false && player_lives > 0)
	{
		resume_button->visible = true;
		resume_text->visible = true;
		main_menu_button->visible = true;
		main_menu_text->visible = true;
		save_button->visible = true;
		save_text->visible = true;
		pause_window->visible = true;
		pause_window_text->visible = true;
		vol_slider_bar->visible = true;
		vol_slider_circle->visible = true;
		sfx_slider_bar->visible = true;
		sfx_slider_circle->visible = true;
		volume_text->visible = true;
		sfx_text->visible = true;
	}
	else
	{
		resume_button->visible = false;
		resume_text->visible = false;
		main_menu_button->visible = false;
		main_menu_text->visible = false;
		save_button->visible = false;
		save_text->visible = false;
		pause_window->visible = false;
		pause_window_text->visible = false;
		vol_slider_bar->visible = false;
		vol_slider_circle->visible = false;
		sfx_slider_bar->visible = false;
		sfx_slider_circle->visible = false;
		volume_text->visible = false;
		sfx_text->visible = false;
	}

	x_limit.x = vol_slider_bar->globalpos.x;
	x_limit.y = vol_slider_bar->globalpos.x + vol_slider_bar->size.x - vol_slider_circle->size.x;

	vol_slider_circle->globalpos.x = App->main_menu->vol_value + x_limit.x;
	sfx_slider_circle->globalpos.x = App->main_menu->sfx_value + x_limit.x;

	return true;
}

// Called each loop iteration
bool j1Scene::Update(float dt)
{
	BROFILER_CATEGORY("Scene Update", Profiler::Color::DarkOrange);

	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN) //Start from first level
	{
		ResetBoxPos();
		ResetIngameUI();
		Load_level(0);
		currentMap = 0;
	}
	else if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN) //Start from beginning of current map
	{
		ResetBoxPos();
		ResetIngameUI();
		App->entitycontroller->Restart();
	}
	else if (App->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN) //Start from second level
	{
		ResetBoxPos();
		ResetIngameUI();
		Load_level(1);
		currentMap = 1;
	}
	else if (App->input->GetKey(SDL_SCANCODE_F4) == KEY_DOWN) //Go to main menu
	{
		App->scenechange->SwitchScene(App->main_menu, App->scene);
	}
	else if (App->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN) //Save game state
	{
		App->SaveGame();
	}
	else if (App->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN) //Load game state
	{
		App->LoadGame();
	}
	else if (App->input->GetKey(SDL_SCANCODE_F8) == KEY_DOWN) //Debug UI
	{
		App->gui->UI_Debug = !App->gui->UI_Debug;
	}
	else if (App->input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN) //View colliders
	{
		App->map->debug = !App->map->debug;
		App->entitycontroller->draw_path = !App->entitycontroller->draw_path;
	}
	else if (App->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN) //Godmode
	{
		godmode = !godmode;
	}
	else if (App->input->GetKey(SDL_SCANCODE_F11) == KEY_DOWN) //Framerate Cap On/Off
	{
		App->fpsCapON = !App->fpsCapON;
	}

	else if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) //Go to main menu after game over or win
	{
		if (player_lives <= 0 || end_game == true)
		{
			App->audio->PauseMusic();
			App->scenechange->SwitchScene(App->main_menu, App->scene);
		}
	}
	//----
	App->map->Draw(dt);
	if (change == false)
	{
		App->entitycontroller->Draw(dt);
	}

	// gui update
	p2List_item<UI_Element*>* item = App->gui->UI_elements.end;
	while (item != nullptr)
	{
		if (item->data->children.start != nullptr) //center children
		{
			if (item->data->type != UI_Element::UI_type::PUSHBUTTON)
			{
				if (item->data->type == UI_Element::UI_type::WINDOW)
				{
					p2List_item<UI_Element*>* tmp = item->data->children.start;
					while (tmp != nullptr)
					{
						tmp->data->Center(true,false); //center X
						tmp = tmp->next;
					}
				}
				else if (item->data->type == UI_Element::UI_type::BACKGROUND)
				{
					p2List_item<UI_Element*>* tmp = item->data->children.start;
					while (tmp != nullptr)
					{
						if (tmp->data->type == UI_Element::UI_type::SLIDER)
						tmp->data->Center(false, true); //center Y
						tmp = tmp->next;
					}
				}
			}
			else
			{
				p2List_item<UI_Element*>* tmp = item->data->children.start;
				while (tmp != nullptr)
				{
					tmp->data->Center(true, true); //center X and Y
					tmp = tmp->next;
				}
			}
		}

		if (item->data->visible == true)
		{
			if (App->gui->CheckMousePos(item->data) == true && item->data->dragging == false && App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) != KEY_REPEAT) //hovering
			{
				item->data->state = UI_Element::State::HOVER;
			}
			if (App->gui->CheckClick(item->data) == true && item->data->state == UI_Element::State::HOVER && item->data->action != UI_Element::Action::NONE) //on-click
			{
				if (item->data->dragable.x == false && item->data->dragable.y == false)
				{
					item->data->state = UI_Element::State::LOGIC;
					if (item->data->locked == false)
					{
						item->data->DoLogic(item->data->action);
						App->audio->PlayFx(CLICK);
					}
					else
					{
						App->audio->PlayFx(LOCKED);
					}
				}
				else
				{
					item->data->state = UI_Element::State::DRAG;
					item->data->DoLogic(item->data->action);
				}
			}
			if (item->data->state == UI_Element::State::DRAG && App->gui->CheckClick(item->data) == true)
			{
				item->data->dragging = true;
				item->data->Drag();

				if (item->data->action == UI_Element::Action::ADJUST_VOL)
				{
					App->main_menu->vol_value = item->data->globalpos.x - x_limit.x;
					if (App->main_menu->vol_value <= -1)
					{
						App->main_menu->vol_value = 0;
					}
					else if (App->main_menu->vol_value >= SDL_MIX_MAXVOLUME + 1)
					{
						App->main_menu->vol_value = SDL_MIX_MAXVOLUME;
					}
					item->data->DoLogic(item->data->action);
				}
				else if (item->data->action == UI_Element::Action::ADJUST_FX)
				{
					App->main_menu->sfx_value = item->data->globalpos.x - x_limit.x;
					if (App->main_menu->sfx_value <= -1)
					{
						App->main_menu->sfx_value = 0;
					}
					else if (App->main_menu->sfx_value >= SDL_MIX_MAXVOLUME + 1)
					{
						App->main_menu->sfx_value = SDL_MIX_MAXVOLUME;
					}
					item->data->DoLogic(item->data->action);
				}

				if (item->data->globalpos.x <= x_limit.x) //left limit
				{
					item->data->globalpos.x = x_limit.x;
				}
				else if (item->data->globalpos.x >= x_limit.y) //right limit
				{
					item->data->globalpos.x = x_limit.y;
				}

				App->gui->UpdateChildren();
			}
			else if (App->gui->CheckMousePos(item->data) == false && item->data->state != UI_Element::State::DRAG)
			{
				item->data->state = UI_Element::State::IDLE; //change to idle
			}
		}
		UpdateState(item->data);
		item = item->prev;
	}
	
	App->render->DrawQuad(coins_bg, 0, 0, 0, 160, true, false);
	App->render->DrawQuad(score_bg, 0, 0, 0, 160, true, false);
	App->render->DrawQuad(timer_bg, 0, 0, 0, 160, true, false);

	UpdateSimpleUI();
	App->gui->Draw();

	if (pause == true && end_game == false && player_lives > 0) //draw settings slider bars and circles
	{
		//bars
		App->render->Blit(App->gui->GetAtlas(), vol_slider_bar->globalpos.x, vol_slider_bar->globalpos.y, &vol_slider_bar->rect, SDL_FLIP_NONE, 0);
		App->render->Blit(App->gui->GetAtlas(), sfx_slider_bar->globalpos.x, sfx_slider_bar->globalpos.y, &sfx_slider_bar->rect, SDL_FLIP_NONE, 0);

		//circles
		App->render->Blit(App->gui->GetAtlas(), vol_slider_circle->globalpos.x, vol_slider_circle->globalpos.y, &vol_slider_circle->rect, SDL_FLIP_NONE, 0);
		App->render->Blit(App->gui->GetAtlas(), sfx_slider_circle->globalpos.x, sfx_slider_circle->globalpos.y, &sfx_slider_circle->rect, SDL_FLIP_NONE, 0);
	}


	return true;
}

// Called each loop iteration
bool j1Scene::PostUpdate()
{
	BROFILER_CATEGORY("Scene PostUpdate", Profiler::Color::DarkOrange);

	bool ret = true;

	if (countdown <= 0) //time up
	{
		player_lives = 0;
		App->audio->PlayFx(GAME_OVER);
	}
	
	if (end_game == true && ui_game_win->visible == false) //win game
	{
		pause = true;
		ui_game_win->visible = true;
		App->audio->PauseMusic();
		App->audio->PlayFx(WIN);
	}

	if (player_lives <= 0 && ui_game_over->visible == false) //game over
	{
		pause = true;
		ui_game_over->visible = true;
		App->audio->PauseMusic();

		pugi::xml_document data; //delete save_game
		data.load_file("save_game.xml");
		data.reset();
		data.save_file("save_game.xml");
	}

	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP)
	{
		p2List_item<UI_Element*>* item = App->gui->UI_elements.start;
		while (item != nullptr)
		{
			if (item->data->dragging == false)
			{
				item->data->state = UI_Element::State::IDLE;
			}
			item = item->next;
		}
	}

	if (change == true) //rotate map and change
	{
		if (App->map->rotate == false)
		{
			if (App->map->rotate_end == true)
			{
				change = false;
			}
			else if (App->map->rotate_back == false && App->map->rotate_end == false)
			{
				SaveAndChange();
			}
		}
	}

	if (to_end == true && App->scenechange->IsChanging() == false)
	{
		if (currentMap < map_names.count() - 1)
		{
			ret = App->scenechange->ChangeMap(++currentMap, fade_time);
		}
		else
		{
			currentMap = 0;
			ret = App->scenechange->ChangeMap(currentMap, fade_time);
		}
		to_end = false;
	}

	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
	{
		if (player_lives > 0 && end_game == false)
		{
			pause = !pause;
			timer.Start();
		}
		else
		{
			ret = false;
		}
	}

	return ret;
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

void j1Scene::UpdateState(UI_Element* data)
{
	switch(data->type)
	{
	case UI_Element::UI_type::PUSHBUTTON: //push button
		switch (data->state)
		{
		case UI_Element::State::IDLE:
			data->rect = { 380,139,190,48 };
			break;

		case UI_Element::State::HOVER:
			data->rect = { 190,139,190,48 };
			break;

		case UI_Element::State::LOGIC:
			data->rect = { 190,90,190,48 };
			break;
		}
		break;

	case UI_Element::UI_type::SLIDER: //slider
		if (data->action == UI_Element::Action::ADJUST_FX)
		{
			switch (data->state)
			{
			case UI_Element::State::IDLE:
			{
				data->rect = { 0,0,26,26 };
			}
			break;

			case UI_Element::State::HOVER:
			{
				data->rect = { 52,0,26,26 };
			}
			break;

			case UI_Element::State::DRAG:
			{
				data->rect = { 104,0,26,26 };
			}
			break;
			}
			break;
		}
		else if (data->action == UI_Element::Action::ADJUST_VOL)
		{
			switch (data->state)
			{
			case UI_Element::State::IDLE:
			{
				data->rect = { 26,0,26,26 };
			}
			break;

			case UI_Element::State::HOVER:
			{
				data->rect = { 78,0,26,26 };
			}
			break;

			case UI_Element::State::DRAG:
			{
				data->rect = { 130,0,26,26 };
			}
			break;
			}
			break;
		}
	}
}

void j1Scene::UpdateSimpleUI()
{
	p2List_item<UI_Element*>* item = App->gui->UI_elements.start;
	while (item != nullptr)
	{
		switch (item->data->type)
		{
		case UI_Element::UI_type::IMAGE: //image

			if (item->data == ui_life1) //Life 1
			{
				if (player_lives >= 2)
				{
					item->data->rect = { 192,0,30,25 }; //full
					break;
				}
				else if (player_lives == 1)
				{
					item->data->rect = { 221,0,30,25 }; //half
					break;
				}
				else
				{
					item->data->rect = { 251,0,30,25 }; //empty
					break;
				}
			}
			else if (item->data == ui_life2) //Life 2
			{
				if (player_lives >= 4)
				{
					item->data->rect = { 192,0,30,25 }; //full
					break;
				}
				else if (player_lives == 3)
				{
					item->data->rect = { 221,0,30,25 }; //half
					break;
				}
				else
				{
					item->data->rect = { 251,0,30,25 }; //empty
					break;
				}
			}
			else if (item->data == ui_life3) //Life 3
			{
				if (player_lives == 6)
				{
					item->data->rect = { 192,0,30,25 }; //full
					break;
				}
				else if (player_lives == 5)
				{
					item->data->rect = { 221,0,30,25 }; //half
					break;
				}
				else
				{
					item->data->rect = { 251,0,30,25 }; //empty
					break;
				}
			}
			else if (item->data == ui_coins) //Coins
			{
				item->data->rect = { 192,25,24,24 };
				break;
			}
			else if (item->data == ui_game_over)
			{
				item->data->rect = { 0,187,626,336 };
				break;
			}
			else if (item->data == ui_game_win)
			{
				item->data->rect = { 0,524,1003,476 };
				break;
			}

		case UI_Element::UI_type::TEXT: //text

			if (item->data == ui_coins_text) //coins number
			{
				sprintf_s(current_coins, "x%u", coins);
				item->data->label = current_coins;
				break;
			}
			else if (item->data == ui_score) //score
			{
				sprintf_s(current_score, "SCORE: %u", score);
				item->data->label = current_score;
				break;
			}
			else if (item->data == ui_timer && pause == false) //timer
			{
				if (countdown <= 60)
				{
					item->data->color = { 255,0,0,255 };
				}
				else
				{
					item->data->color = { 255,255,255,255 };
				}

				if (timer.ReadSec() >= 1)
				{
					timer.Start();
					countdown--;
				}
				sprintf_s(current_time, "TIME: %u", countdown);
				item->data->label = current_time;
				break;
			}
			else if (item->data == volume_text) //volume
			{
				sprintf_s(current_vol, "MUSIC: %d", App->main_menu->vol_value);
				item->data->label = current_vol;
				break;
			}
			else if (item->data == sfx_text) //sfx
			{
				sprintf_s(current_sfx, "SOUND: %d", App->main_menu->sfx_value);
				item->data->label = current_sfx;
				break;
			}
		}
		item = item->next;
	}
}