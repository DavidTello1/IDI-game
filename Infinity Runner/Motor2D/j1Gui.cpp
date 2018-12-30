#include "p2Defs.h"
#include "p2Log.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Fonts.h"
#include "j1Input.h"
#include "j1Gui.h"
#include "j1Window.h"
#include "j1Audio.h"

j1Gui::j1Gui() : j1Module()
{
	name.create("gui");
	last_element_motion = { 0,0 };
}

// Destructor
j1Gui::~j1Gui()
{}

// Called before render is available
bool j1Gui::Awake(pugi::xml_node& conf)
{
	LOG("Loading GUI");
	bool ret = true;

	atlas_file_name = conf.child("atlas").attribute("file").as_string("");
	blue_file_name = conf.child("blue").attribute("file").as_string("");
	red_file_name = conf.child("red").attribute("file").as_string();
	green_file_name = conf.child("green").attribute("file").as_string();
	yellow_file_name = conf.child("yellow").attribute("file").as_string();
	grey_file_name = conf.child("grey").attribute("file").as_string();
	
	moving_speed = conf.attribute("moving_elements_speed").as_int();

	white.r = conf.child("atlas").child("white").attribute("r").as_uint();
	white.g = conf.child("atlas").child("white").attribute("g").as_uint();
	white.b = conf.child("atlas").child("white").attribute("b").as_uint();
	white.a = conf.child("atlas").child("white").attribute("a").as_uint();

	black.r = conf.child("atlas").child("black").attribute("r").as_uint();
	black.g = conf.child("atlas").child("black").attribute("g").as_uint();
	black.g = conf.child("atlas").child("black").attribute("b").as_uint();
	black.a = conf.child("atlas").child("black").attribute("a").as_uint();

	//Creating game over image
	pugi::xml_node n = conf.child("atlas").child("gameOver");
	for (SDL_Rect &r : game_over_image.rect)
	{
		r.x = n.attribute("x").as_int();
		r.y = n.attribute("y").as_int();
		r.w = n.attribute("width").as_int();
		r.h = n.attribute("height").as_int();
		game_over_image.idle.PushBack(r);
		if (n.next_sibling("gameOver"))
			n = n.next_sibling("gameOver");
	}

	//Creating help image
	n = conf.child("atlas").child("help");
	for (SDL_Rect &r : help_image.rect)
	{
		r.x = n.attribute("x").as_int();
		r.y = n.attribute("y").as_int();
		r.w = n.attribute("width").as_int();
		r.h = n.attribute("height").as_int();
		if (n.next_sibling("help"))
			n = n.next_sibling("help");
	}

	//Creating saving point image
	n = conf.child("atlas").child("save");
	for (SDL_Rect &r : saving_point_image.rect)
	{
		r.x = n.attribute("x").as_int();
		r.y = n.attribute("y").as_int();
		r.w = n.attribute("width").as_int();
		r.h = n.attribute("height").as_int();
		saving_point_image.idle.PushBack(r);
		if (n.next_sibling("save"))
			n = n.next_sibling("save");
	}

	//Creating last death image
	n = conf.child("atlas").child("lastDeath");
	for (SDL_Rect &r : last_death_image.rect)
	{
		r.x = n.attribute("x").as_int();
		r.y = n.attribute("y").as_int();
		r.w = n.attribute("width").as_int();
		r.h = n.attribute("height").as_int();
		last_death_image.idle.PushBack(r);
		if (n.next_sibling("lastDeath"))
			n = n.next_sibling("lastDeath");
	}

	//Creating a blue button
	n = conf.child("blue").child("button");
	for (SDL_Rect &r : blue_button.rect)
	{
		r.x = n.attribute("x").as_int();
		r.y = n.attribute("y").as_int();
		r.w = n.attribute("width").as_int();
		r.h = n.attribute("height").as_int();
		n = n.next_sibling("button");
	}
	blue_button.color	 = BLUE;


	// Creating a red button
	n = conf.child("red").child("button");
	for (SDL_Rect &r : red_button.rect)
	{
		r.x = n.attribute("x").as_int();
		r.y = n.attribute("y").as_int();
		r.w = n.attribute("width").as_int();
		r.h = n.attribute("height").as_int();
		n = n.next_sibling("button");
	}
	red_button.color	= RED;


	// Creating a green button
	n = conf.child("green").child("button");
	for (SDL_Rect &r : green_button.rect)
	{
		r.x = n.attribute("x").as_int();
		r.y = n.attribute("y").as_int();
		r.w = n.attribute("width").as_int();
		r.h = n.attribute("height").as_int();
		n = n.next_sibling("button");
	}
	green_button.color = GREEN;


	//Creating slider pointer image (Green)
	n = conf.child("green").child("slider_pointer");
	for (SDL_Rect &r : slider_pointer_button.rect)
	{
		r.x = n.attribute("x").as_int();
		r.y = n.attribute("y").as_int();
		r.w = n.attribute("width").as_int();
		r.h = n.attribute("height").as_int();
		if (n.next_sibling("slider_pointer"))
			n = n.next_sibling("slider_pointer");
	}
	slider_pointer_button.color = GREEN;


	//Creating slider image
	n = conf.child("green").child("slider");
	for (SDL_Rect &r : slider_button.rect)
	{
		r.x = n.attribute("x").as_int();
		r.y = n.attribute("y").as_int();
		r.w = n.attribute("width").as_int();
		r.h = n.attribute("height").as_int();
		if (n.next_sibling("slider"))
			n = n.next_sibling("slider");
	}
	slider_button.color = GREEN;


	// Creating a yellow button
	n = conf.child("yellow").child("button");
	for (SDL_Rect &r : yellow_button.rect)
	{
		r.x = n.attribute("x").as_int();
		r.y = n.attribute("y").as_int();
		r.w = n.attribute("width").as_int();
		r.h = n.attribute("height").as_int();
		n = n.next_sibling("button");
	}
	yellow_button.color = YELLOW;


	// Creating a grey button
	n = conf.child("grey").child("button");
	for (SDL_Rect &r : grey_button.rect)
	{
		r.x = n.attribute("x").as_int();
		r.y = n.attribute("y").as_int();
		r.w = n.attribute("width").as_int();
		r.h = n.attribute("height").as_int();
		if (n.next_sibling("button"))
			n = n.next_sibling("button");
	}

	//pause window menu
	n = conf.child("atlas").child("window");
	for (SDL_Rect &r : white_window.rect)
	{
		r.x = n.attribute("x").as_int();
		r.y = n.attribute("y").as_int();
		r.w = n.attribute("width").as_int();
		r.h = n.attribute("height").as_int();
		if (n.next_sibling("window"))
			n = n.next_sibling("window");
	}

	// Blue diamond
	n = conf.child("atlas").child("blue_diamond");
	for (SDL_Rect &r : blue_diamond.rect)
	{
		r.x = n.attribute("x").as_int();
		r.y = n.attribute("y").as_int();
		r.w = n.attribute("width").as_int();
		r.h = n.attribute("height").as_int();
		blue_diamond.idle.PushBack(r);
		if (n.next_sibling("blue_diamond"))
			n = n.next_sibling("blue_diamond");

	}

	// Green diamond
	n = conf.child("atlas").child("green_diamond");
	for (SDL_Rect &r : green_diamond.rect)
	{
		r.x = n.attribute("x").as_int();
		r.y = n.attribute("y").as_int();
		r.w = n.attribute("width").as_int();
		r.h = n.attribute("height").as_int();
		green_diamond.idle.PushBack(r);
		if (n.next_sibling("green_diamond"))
			n = n.next_sibling("green_diamond");
	}

	// Heart
	n = conf.child("atlas").child("heart");
	for (SDL_Rect &r : heart.rect)
	{
		r.x = n.attribute("x").as_int();
		r.y = n.attribute("y").as_int();
		r.w = n.attribute("width").as_int();
		r.h = n.attribute("height").as_int();
		heart.idle.PushBack(r);
		if (n.next_sibling("heart"))
			n = n.next_sibling("heart");
	}

	//READING ANIMATIONS
	//BLUE SHINE
	SDL_Rect r;
	n = conf.child("UIanimations").child("blue_shine");
	for (n; n; n = n.next_sibling("blue_shine"))
	{
		r.x = n.attribute("x").as_int();
		r.y = n.attribute("y").as_int();
		r.w = n.attribute("width").as_int();
		r.h = n.attribute("height").as_int();
		blue_diamond.blue_shine.PushBack(r);
	}
	blue_diamond.blue_shine.loop = true;

	//GREEN SHINE
	n = conf.child("UIanimations").child("green_shine");
	for (n; n; n = n.next_sibling("green_shine"))
	{
		r.x = n.attribute("x").as_int();
		r.y = n.attribute("y").as_int();
		r.w = n.attribute("width").as_int();
		r.h = n.attribute("height").as_int();
		green_diamond.green_shine.PushBack(r);
	}
	green_diamond.green_shine.loop = true;

	//HEART BLINK
	n = conf.child("UIanimations").child("heart_blink");
	for (n; n; n = n.next_sibling("heart_blink"))
	{
		r.x = n.attribute("x").as_int();
		r.y = n.attribute("y").as_int();
		r.w = n.attribute("width").as_int();
		r.h = n.attribute("height").as_int();
		heart.heart_blink.PushBack(r);
	}
	heart.heart_blink.loop = true;

	blue_diamond.blue_shine.speed = green_diamond.green_shine.speed = conf.child("UIanimations").attribute("animSpeed").as_float();
	heart.heart_blink.speed = blue_diamond.blue_shine.speed / 5;

	return ret;
}

// Called before the first frame
bool j1Gui::Start()
{
	atlas	= App->tex->Load(atlas_file_name.GetString());
	blue	= App->tex->Load(blue_file_name.GetString());
	red		= App->tex->Load(red_file_name.GetString());
	green	= App->tex->Load(green_file_name.GetString());
	yellow	= App->tex->Load(yellow_file_name.GetString());
	grey	= App->tex->Load(grey_file_name.GetString());

	SDL_RenderGetViewport(App->render->renderer, &App->render->viewport);
	

//	////////////////////////////////////// MAIN MENU //////////////////////////////////////
//
//	//main_menu_window = (Image*)CreateElement(IMAGE, iPoint(0, 0), SDL_Rect({ 1000,1000,App->render->viewport.w,App->render->viewport.h }), nullptr, nullptr, NO_ACTION, (j1Module*)App, nullptr, true);
//	//windows.add(main_menu_window);
//
//	//main_menu_ui = (Image*)CreateElement(IMAGE, iPoint(App->render->viewport.w / 2 - 250, App->render->viewport.h / 2 - 400), SDL_Rect({ 1000,1000,500,800 }), nullptr, nullptr, NO_ACTION, (j1Module*)App, main_menu_window, true);
//
//	//Button* start_button	= (Button*)CreateButton({ 0,0 },					 blue_button,	START,		nullptr, main_menu_ui);
//	//continue_button			= (Button*)CreateButton({ 0,0 },					 grey_button,	CONTINUE,	nullptr, main_menu_ui);
//	//Button* settings_button = (Button*)CreateButton({ 0,0 },					 green_button,	SETTINGS,	nullptr, main_menu_ui);
//	//Button* exit_button		= (Button*)CreateButton({ 0,0 },					 red_button,	EXIT_GAME,	nullptr, main_menu_ui);
//	//credits_button			= (Button*)CreateButton(-main_menu_ui->position + 50,red_button,	CREDITS,	nullptr, main_menu_ui);
//	//help_button				= (Button*)CreateButton({ 0,0 },					 red_button,	HELP,		nullptr, main_menu_ui);
//	//
//	//start_button	->Center(0, -140);
//	//continue_button	->Center(0,-70);
//	//settings_button	->Center();
//	//help_button		->Center(0, 70);
//	//exit_button		->Center(0, 160);
//
//
//
//
//	//Label* lstart_button	= (Label*)CreateElement(LABEL, iPoint(0, 0), { 0,0,0,0 }, nullptr, "PLAY GAME", NO_ACTION, nullptr, start_button	, false, black);
//	//Label* lcontinue_button = (Label*)CreateElement(LABEL, iPoint(0, 0), { 0,0,0,0 }, nullptr, "CONTINUE",	NO_ACTION, nullptr, continue_button	, false, black);
//	//Label* lsettings_button = (Label*)CreateElement(LABEL, iPoint(0, 0), { 0,0,0,0 }, nullptr, "SETTINGS",	NO_ACTION, nullptr, settings_button	, false, black);
//	//Label* lexit_button		= (Label*)CreateElement(LABEL, iPoint(0, 0), { 0,0,0,0 }, nullptr, "EXIT GAME", NO_ACTION, nullptr, exit_button		, false, black);
//	//Label* lcredits_button	= (Label*)CreateElement(LABEL, iPoint(0, 0), { 0,0,0,0 }, nullptr, "CREDITS",	NO_ACTION, nullptr, credits_button	, false, black);
//	//Label* lhelp_button		= (Label*)CreateElement(LABEL, iPoint(0, 0), { 0,0,0,0 }, nullptr, "HELP",		NO_ACTION, nullptr, help_button		, false, black);
//
//	//lstart_button	->Center();
//	//lcontinue_button->Center();
//	//lsettings_button->Center();
//	//lexit_button	->Center();
//	//lcredits_button	->Center();
//	//lhelp_button	->Center();
//
//	////Credits window
//
//	//credits_ui = (Image*)CreateElement(IMAGE, iPoint(App->render->viewport.w / 2 - 250, App->render->viewport.h / 2 - 400), SDL_Rect({ 1000,1000,500,800 }), nullptr, nullptr, NO_ACTION, (j1Module*)App, main_menu_window, false);
//	//
//	//credits_to_menu			= (Button*)CreateButton(-main_menu_ui->position + 50, red_button,   CREDITS, nullptr, credits_ui);
//	//Button* website_button	= (Button*)CreateButton({ 50,50 },					  green_button, WEBSITE, nullptr, credits_ui);
//
//	//Label* lwebsite_button	= (Label*)CreateElement(LABEL, iPoint(0, 0), { 0,0,0,0 }, nullptr, "Go to website",									NO_ACTION, nullptr, website_button,			  false, black);
//	//Label* lcredits_to_menu = (Label*)CreateElement(LABEL, iPoint(0, 0), { 0,0,0,0 }, nullptr, "RETURN",										NO_ACTION, nullptr, credits_to_menu,		  false, black);
//	//Label* lcredits			= (Label*)CreateElement(LABEL, iPoint(0, 0), { 0,0,0,0 }, nullptr, "Game developed by Oscar Pons and Oriol Sabate", NO_ACTION, nullptr, credits_ui,				  false, white);
//	//Label* llicense			= (Label*)CreateElement(LABEL, iPoint(0, 0), { 0,0,0,0 }, nullptr, "MIT License Copyright(c)(2018) (Oscar Pons and Oriol Sabate)", NO_ACTION, nullptr, credits_ui,false, white);
//	//
//
//	//	
//
//	//website_button->Center(0, 90);
//	//lwebsite_button->Center();
//	//lcredits_to_menu->Center();
//	//lcredits->Center(0,-30);
//	//llicense->Center(0,40);
//
//	//////////////////////////////////////// END MAIN MENU //////////////////////////////////////
//
//	////////////////////////////////////////HELP WINDOW////////////////////////////////////////
//
//
//	//help_window = (Image*)CreateElement(IMAGE, iPoint(App->render->viewport.w / 2 - 250, App->render->viewport.h / 2 - 400), SDL_Rect({ 1000,1000,500,800 }), nullptr, nullptr, NO_ACTION,nullptr, main_menu_window, true);
//	////windows.add(help_window);
//
//	//Image* help = (Image*)CreateElement(IMAGE, iPoint(0, 0), help_image.rect[IDLE], &help_image, nullptr, NO_ACTION, nullptr, help_window, false);	
//	//help_to_menu = (Button*)CreateButton({ 0,0 }, red_button, HELP, nullptr, help_window);
//
//	//Label* lhelptomenu = (Label*)CreateElement(LABEL, iPoint(0, 0), { 0,0,0,0 }, nullptr, "RETURN", NO_ACTION, nullptr, help_to_menu, false, black);
//
//	//help->Center(0,-70);
//	//help_to_menu->Center(0, 130);
//	//lhelptomenu->Center();
//
//	///////////////////////////////////////END HELP////////////////////////////////////////////
//
//	//////////////////////////////////////// IN GAME UI //////////////////////////////////////
//
//	//in_game_window	  = (Image*)CreateElement(IMAGE, iPoint(0, 0), SDL_Rect({ 1000,1000,App->render->viewport.w,App->render->viewport.h }), nullptr, nullptr, NO_ACTION, nullptr, nullptr);
//	//windows.add(in_game_window);
//
//	//in_game_gui = (Image*)CreateElement(IMAGE, iPoint(0, 0), SDL_Rect({ 1000,1000,App->render->viewport.w,App->render->viewport.h }), nullptr, nullptr, NO_ACTION, nullptr, in_game_window);
//
//	//score  = (Label*)CreateElement(LABEL, iPoint(in_game_gui->rect[IDLE].w - 200, 35), { 0,0,0,0 }, nullptr, "SCORE :",  INFO,  nullptr, in_game_gui, false, white);
//	//Label* lscore = (Label*)CreateElement(LABEL, iPoint(0, 0), { 0,0,0,0 }, nullptr, "", SCORE, nullptr, score, false, white);
//
//	//lscore->Center(50, 0);
//
//	//heart_ref = (Image*)CreateElement(IMAGE, iPoint(10, 10),  heart.rect[IDLE],			&heart,			nullptr, LIFE_SYSTEM,  nullptr, in_game_gui);
//	//green_ref = (Image*)CreateElement(IMAGE, iPoint(-200, -7), green_diamond.rect[IDLE], &green_diamond, nullptr, DYNAMIC_INFO, nullptr, score);
//	//blue_ref  = (Image*)CreateElement(IMAGE, iPoint(-300, -7), blue_diamond.rect[IDLE],  &blue_diamond,	 nullptr, DYNAMIC_INFO, nullptr, score);
//
//	//Label* lgreen_ref		= (Label*)CreateElement(LABEL, iPoint(0, 0), { 0,0,0,0 }, nullptr, "X", INFO,			nullptr, green_ref ,false ,white);
//	//Label* lblue_ref		= (Label*)CreateElement(LABEL, iPoint(0, 0), { 0,0,0,0 }, nullptr, "X", INFO,			nullptr, blue_ref  ,false ,white);
//	//Label* green_ref_count	= (Label*)CreateElement(LABEL, iPoint(0, 0), { 0,0,0,0 }, nullptr, "",  GREEN_COUNTER,	nullptr, lgreen_ref,false ,white);
//	//Label* blue_ref_count	= (Label*)CreateElement(LABEL, iPoint(0, 0), { 0,0,0,0 }, nullptr, "",  BLUE_COUNTER,	nullptr, lblue_ref ,false ,white);
//
//	//lgreen_ref->Center(30, 5);
//	//lblue_ref->Center(30, 5);
//	//green_ref_count->Center(20, 0);
//	//blue_ref_count->Center(20, 0);	
//
//	//timersec			= (Label*)CreateElement(LABEL, iPoint(0, 15), { 0,0,0,0 }, nullptr, ":", INFO,			  nullptr, in_game_gui, false, white);
//	//Label* time_minutes = (Label*)CreateElement(LABEL, iPoint(0, 0),  { 0,0,0,0 }, nullptr, "",  GAME_TIMER_MINS, nullptr, timersec   , false, white);
//	//Label* time_seconds = (Label*)CreateElement(LABEL, iPoint(0, 0),  { 0,0,0,0 }, nullptr, "",  GAME_TIMER_SECS, nullptr, timersec   , false, white);
//
//	//timersec	->CenterX();
//	//time_minutes->Center(-30, 0);		
//	//time_seconds->Center(25, 0);
//
//
//	//////////////////////////////////////// END IN GAME UI //////////////////////////////////////
//
//	//////////////////////////////////////// MISC //////////////////////////////////////
//	//
//	////Label* pl_name = (Label*)CreateElement(LABEL, iPoint(App->entitymanager->player_ref->position.x + App->entitymanager->player_ref->rect.w / 2, App->entitymanager->player_ref->position.y - 50), { 0,0,0,0 },nullptr, "--Kikime--", PLAYER_NAME, nullptr, in_game_gui);
//	//
//	//last_death = (Image*)CreateElement(IMAGE, iPoint(-20, -20), last_death_image.rect[IDLE],&last_death_image,nullptr, LAST_DEATH, nullptr, nullptr, true);
//	//
//	//saving_point = (Image*)CreateElement(IMAGE, iPoint(-20, -20), saving_point_image.rect[IDLE], &saving_point_image, nullptr, LAST_DEATH, nullptr, nullptr, true);
//
//	//fps = (Label*)CreateElement(LABEL, iPoint(20, 50), { 0,0,0,0 }, nullptr, "0", FPS, nullptr, in_game_gui, false, white);
//
//	//game_over  = (Image*)CreateElement(IMAGE, iPoint(App->render->viewport.w / 2 - game_over_image.rect[IDLE].w / 2, App->render->viewport.h / 2 - game_over_image.rect[IDLE].h / 2), game_over_image.rect[IDLE], &game_over_image, nullptr, NO_ACTION, nullptr,in_game_window, false);
//
//
//	//////////////////////////////////////// END MISC //////////////////////////////////////
//
//	//////////////////////////////////////// PAUSE MENU //////////////////////////////////////
//
//	//in_game_pause_ui = (Image*)CreateElement(IMAGE, iPoint(App->render->viewport.w / 2 - white_window.rect[IDLE].w/2, App->render->viewport.h / 2 - white_window.rect[IDLE].h/2), white_window.rect[IDLE], &white_window, nullptr, NO_ACTION, nullptr,in_game_window);
//
//	//Label* title_pause = (Label*)CreateElement(LABEL, iPoint(0, 15), { 0,0,0,0 },nullptr, "PAUSE MENU", NO_ACTION, nullptr, in_game_pause_ui, false, black);
//	//title_pause->CenterX();
//
//	//Button* in_game_resume		= (Button*)CreateButton({ 0,0 }, blue_button,  PAUSE,	  nullptr, in_game_pause_ui);
//	//Button* in_game_settings	= (Button*)CreateButton({ 0,0 }, green_button, SETTINGS,  nullptr, in_game_pause_ui);
//	//Button* in_game_main_menu	= (Button*)CreateButton({ 0,0 }, red_button,   MAIN_MENU, nullptr, in_game_pause_ui);
//	//Button* in_game_exit		= (Button*)CreateButton({ 0,0 }, red_button,   EXIT_GAME, nullptr, in_game_pause_ui);
//	//
//	//in_game_resume		->Center(0, -100);
//	//in_game_main_menu	->Center(0, -30);
//	//in_game_settings	->Center(0, 40);
//	//in_game_exit		->Center(0, 110);
//
//	//Label* lin_game_resume		= (Label*)CreateElement(LABEL, iPoint(0, 0), { 0,0,0,0 },nullptr, "RESUME GAME", NO_ACTION, nullptr, in_game_resume   ,false ,black);
//	//Label* lin_game_settings	= (Label*)CreateElement(LABEL, iPoint(0, 0), { 0,0,0,0 },nullptr, "SETTINGS",	 NO_ACTION, nullptr, in_game_settings ,false ,black);
//	//Label* lin_game_main_menu	= (Label*)CreateElement(LABEL, iPoint(0, 0), { 0,0,0,0 },nullptr, "MAIN MENU",	 NO_ACTION, nullptr, in_game_main_menu,false ,black);
//	//Label* lin_game_exit		= (Label*)CreateElement(LABEL, iPoint(0, 0), { 0,0,0,0 },nullptr, "EXIT GAME",	 NO_ACTION, nullptr, in_game_exit	  ,false ,black);
//
//	//lin_game_resume->Center();
//	//lin_game_settings->Center();
//	//lin_game_main_menu->Center();
//	//lin_game_exit->Center();
//
//	//////////////////////////////////////// END PAUSE MENU //////////////////////////////////////
//
//	//	////////////////////////////////////// SETTINGS //////////////////////////////////////
//
//	//settings_window = (Image*)CreateElement(IMAGE, { 0,0}, SDL_Rect({ 1000,1000,App->render->viewport.w,App->render->viewport.h }), nullptr, nullptr, NO_ACTION, nullptr, nullptr, false);
//	//windows.add(settings_window);
//
//	//settings_gui = (Image*)CreateElement(IMAGE, { App->render->viewport.w / 2 - 250, App->render->viewport.h / 2 - 350 }, SDL_Rect({ 1000,1000,500,800 }), nullptr, nullptr, NO_ACTION, nullptr, settings_window, false);
//
//	//Button* settings_to_main = (Button*)CreateButton({ 0,0 }, red_button, SETTINGS, nullptr, settings_gui);
//	//settings_to_main->Center(0, 70);
//
//	//Label* lsettings_to_main = (Label*)CreateElement(LABEL, iPoint(0, 0), { 0,0,0,0 }, nullptr, "RETURN", NO_ACTION, nullptr, settings_to_main, false, black);
//	//lsettings_to_main->Center();
//
//	//// MUSIC
//	//Button* music_volume_button = (Button*)CreateButton({ 0,-125 }, green_button, NO_ACTION, nullptr, settings_to_main);
//	//Button* music_slider = (Button*)CreateButton({ 0,music_volume_button->rect[IDLE].h + 20 }, slider_button, NO_ACTION, nullptr, music_volume_button);
//	//Button* music_slider_button = (Button*)CreateButton({ SDL_MIX_MAXVOLUME,0 }, slider_pointer_button, CHANGE_VOLUME, nullptr, music_slider);
//
//	//music_volume_button->CenterX();
//	//music_slider->CenterX();
//	//music_slider_button->movable = true;
//
//	//Label* lmusic_volume_button = (Label*)CreateElement(LABEL, iPoint(0, 0), { 0,0,0,0 }, nullptr, "MUSIC VOLUME", NO_ACTION, nullptr, music_volume_button, false, black);
//	//lmusic_volume_button->Center();
//
//
//	////FX Volume
//	//Button* fx_volume_button = (Button*)CreateButton({ 0,-250 }, green_button, NO_ACTION, nullptr, settings_to_main);
//	//Button* fx_slider = (Button*)CreateButton({ 0,fx_volume_button->rect[IDLE].h + 20 }, slider_button, NO_ACTION, nullptr, fx_volume_button);
//	//Button* fx_slider_button = (Button*)CreateButton({ SDL_MIX_MAXVOLUME,0 }, slider_pointer_button, CHANGE_VOLUME_FX, nullptr, fx_slider);
//
//	//fx_volume_button->CenterX();
//	//fx_slider->CenterX();
//	//fx_slider_button->movable = true;
//
//	//Label* fx_lvolume_button = (Label*)CreateElement(LABEL, iPoint(0, 0), { 0,0,0,0 }, nullptr, "FX VOLUME", NO_ACTION, nullptr, fx_volume_button, false, black);
//	//fx_lvolume_button->Center();
//
//	//////////////////////////////////////// END SETTINGS //////////////////////////////////////
//
//
//	//// The very first load of the game, preparing main menu
//	//PrepareMainMenuGui();
//
//	///* Note
//	//To input text check the keyobard state then make a bool matrix or something similar then update if changed the specific key true
//	//*/
//	//last_death->SetVisible();
//	//saving_point->SetVisible();
//	//CheckContinue();
	return true;
}
//void j1Gui::CheckContinue()
//{
//	if (App->ExistsSaveGame()) {
//		continue_button->color = YELLOW;
//		int i = 0;
//		for (SDL_Rect &r : continue_button->rect)
//		{
//			r.x = yellow_button.rect[i].x;
//			r.y = yellow_button.rect[i].y;
//			r.w = yellow_button.rect[i].w;
//			r.h = yellow_button.rect[i].h;
//			i++;
//		}
//	}
//	else {
//		App->gui->continue_button->color = GREY;
//		int i = 0;
//		for (SDL_Rect &r : continue_button->rect)
//		{
//			r.x = grey_button.rect[i].x;
//			r.y = grey_button.rect[i].y;
//			r.w = grey_button.rect[i].w;
//			r.h = grey_button.rect[i].h;
//			i++;
//		}
//		saving_point->position = { -50,-50 };
//	}
//}
void j1Gui::PrepareMainMenuGui() {
	SetWindowsInvisible();
	main_menu_window->SetVisible();
	credits_ui->SetInvisible();
	settings_window->SetInvisible();
	help_window->SetInvisible();
}

void j1Gui::PrepareInGameGui() {
	SetWindowsInvisible();
	in_game_window->SetVisible();
	in_game_pause_ui->SetInvisible();
	game_over->SetInvisible();
}

void j1Gui::SetWindowsInvisible() {
	p2List_item<UIElement*>* item = windows.start;
	for (item; item != nullptr; item = item->next) {
		item->data->SetInvisible();
	}
}

void j1Gui::SetWindowsVisible() {
	p2List_item<UIElement*>* item = windows.start;
	for (item; item != nullptr; item = item->next) {
		item->data->SetVisible();
	}
}

void j1Gui::CalculateElementsPosition() {

	main_menu_ui->Center();
	credits_ui->Center();
	settings_gui->Center();
	in_game_pause_ui->Center();
	timersec->CenterX();
	credits_button->initial_pos = -credits_button->parent->initial_pos + 50;
	credits_to_menu->initial_pos = -credits_to_menu->parent->initial_pos + 50;
	score->initial_pos.x = in_game_gui->rect[IDLE].w - 140;
	game_over->Center();
	help_window->Center();
}

// Update all guis
bool j1Gui::PreUpdate()
{  
	bool has_changed = false;
	p2List_item<UIElement*>* item = elements.end;	

	while (item != nullptr)
	{
		//if (App->debug->show_fps == false && in_game_gui->visible)
		//{
		//	fps->SetInvisible();
		//}
		//else if(App->debug->show_fps == true && in_game_gui->visible)
		//{
		//	fps->SetVisible();
		//}

		if (item->data->visible) {

			item->data->PreUpdate();
			has_changed = HandleInput(item->data);
			if (has_changed) break;
		}

		item = item->prev;
	}
	return true;
}


// Called after all Updates
bool j1Gui::Update(float dt)
{

	p2List_item<UIElement*>* item = elements.start;
	SDL_Texture* sprites = atlas;
	while (item != NULL)
	{
		if (item->data->visible) {

			item->data->Update();

			if (item->data->type == BUTTON) {
				Button* b = (Button*)item->data;
				switch (b->color)
				{
				case BLUE:
					sprites = blue;
					break;
				case RED:
					sprites = red;
					break;
				case YELLOW:
					sprites = yellow;
					break;
				case GREY:
					sprites = grey;
					break;
				case GREEN:
					sprites = green;
					break;
				default:
					sprites = atlas;
					break;
				}
			}

			item->data->Draw(sprites);
			//if (App->debug->show_debug_ui) item->data->DrawRect();
		}
		item = item->next;
	}
	return true;
}

// Called before quitting
bool j1Gui::CleanUp()
{
	LOG("Freeing GUI");

	p2List_item<UIElement*>* item = elements.start;

	while (item != nullptr)
	{
		if (item->data->type == LABEL) {
			Label* l = (Label*)item->data;
			if (l->text != nullptr)
				App->tex->UnLoad(l->text);
		}
		delete item->data;

		item = item->next;
	}
	
	return true;
}

UIElement* j1Gui::CreateButton(iPoint pos,const Button &b, ActionType action, j1Module* callback, UIElement* parent)
{
	Button* elem = new Button(pos, b,action,parent);
	if (parent != nullptr) parent->sons.add(elem);
	elements.add(elem);

	return elem;
}

UIElement* j1Gui::CreateElement(UIType type, iPoint pos, SDL_Rect rect, Image* img, p2SString string, ActionType action, j1Module* callback, UIElement* parent, bool visible, SDL_Color color)
{
	UIElement* elem = nullptr;

	switch (type)
	{
	case IMAGE:
      
		elem = new Image(action,pos, rect, img, type, parent,visible);
		break;
	case LABEL:

		elem = new Label(action, pos, type, string, parent, visible, color);
		break;
	default:

		LOG("COULD NOT IDENTIFY UI ELEMENT TYPE");
		break;
	}

	if (parent != nullptr) parent->sons.add(elem);
	elements.add(elem);

	return elem;
}

bool j1Gui::HandleInput(UIElement* element)
{
	bool ret = true;
	iPoint mouse;
	App->input->GetMousePosition(mouse.x, mouse.y);

	bool is_inside = (mouse.x >= element->position.x && mouse.x <= element->position.x + element->rect[element->state].w &&
					  mouse.y >= element->position.y && mouse.y <= element->position.y + element->rect[element->state].h);

	bool is_changing = false;
	UIState prev_state = element->state;
	
	if (element->state != CLICK_DOWN &&	is_inside && App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) != KEY_DOWN)
	{
		element->state = HOVER;
		//LOG("hover");
	}
	else if((element->state == HOVER || element->state == CLICK_DOWN) && is_inside && (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT || App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN))
	{
		element->state = CLICK_DOWN;
		App->input->GetMousePosition(mouseClick.x, mouseClick.y);
		//App->input->GetMousePosition(startDraging.x, startDraging.y);
		startDraging = element->initial_pos;
		element->is_moving = true;
		//LOG("click");
	}
	else if (is_inside && App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP)
	{
		element->state = CLICK_UP;
		element->is_moving = false;
		//LOG("click up");
	}
	else
	{
		element->state = IDLE;
		//LOG("idle");
	}

    is_changing = (prev_state != element->state);
	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP) {
		element->is_moving = false;
	}
	switch (element->type)
	{
	case BUTTON:
		if(element->state == CLICK_UP)
			element->HandleAction();
		break;
	case LABEL:
		if (is_changing)
			element->HandleAction();
		break;
	case IMAGE:
		break;
	}

	//if (element->is_moving && element->movable) 
	//{
	//	if (element->action == CHANGE_VOLUME || element->action == CHANGE_VOLUME_FX) {
	//		element->state = HOVER;
	//		element->initial_pos.x = mouse.x - mouseClick.x + startDraging.x;
	//
	//		if (element->initial_pos.x <= - 1) element->initial_pos.x = 0;
	//		else if (element->initial_pos.x >= SDL_MIX_MAXVOLUME+ 1) element->initial_pos.x = SDL_MIX_MAXVOLUME;
	//
	//		if (element->action == CHANGE_VOLUME) App->ChangeMusicVolume(element->initial_pos.x);
	//		else App->ChangeFXVolume(element->initial_pos.x );
	//
	//		App->audio->current_volume = element->initial_pos.x;
	//	}
	//	else element->initial_pos = mouse - mouseClick + startDraging;
	//}
	//
	//if (element->type == BUTTON && element->state == CLICK_DOWN && element->visible)
	//	App->audio->PlayFx(click);

	//LOG("FIND :::: %d", App->audio->fx_name.find("hello"));

	return is_changing && element->type != LABEL;
}

// const getter for atlas
const SDL_Texture* j1Gui::GetAtlas() const
{
	return atlas;
}