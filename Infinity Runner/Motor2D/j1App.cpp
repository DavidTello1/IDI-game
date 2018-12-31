#include <iostream> 
#include <sstream> 
#include <cstdio>

#include "p2Defs.h"
#include "p2Log.h"

#include "j1Window.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Scene.h"
#include "j1Map.h"
#include "j1EntityController.h"
#include "j1Fonts.h"
#include "j1App.h"

// Constructor
j1App::j1App(int argc, char* args[]) : argc(argc), args(args)
{
	PERF_START(ptimer);

	input = new j1Input();
	win = new j1Window();
	render = new j1Render();
	tex = new j1Textures();
	audio = new j1Audio();
	scene = new j1Scene();
	map = new j1Map();
	entitycontroller = new j1EntityController();
	font = new j1Fonts();

	// Ordered for awake / Start / Update
	// Reverse order of CleanUp
	AddModule(input);
	AddModule(win);
	AddModule(tex);
	AddModule(audio);
	AddModule(map);
	AddModule(entitycontroller);
	AddModule(scene);
	AddModule(font);

	// render last to swap buffer
	AddModule(render);

	PERF_PEEK(ptimer);
}

// Destructor
j1App::~j1App()
{
	// release modules
	p2List_item<j1Module*>* item = modules.end;

	while(item != NULL)
	{
		RELEASE(item->data);
		item = item->prev;
	}

	modules.clear();
}

void j1App::AddModule(j1Module* module, bool check_active)
{
	module->Init(check_active);
	modules.add(module);
}

// Called before render is available
bool j1App::Awake()
{
	PERF_START(ptimer);

	pugi::xml_document	config_file;
	pugi::xml_node		config;
	pugi::xml_node		app_config;

	bool ret = false;

	config = LoadConfig(config_file);
	save_game =  "save_game_0.xml";

	if(config.empty() == false)
	{
		// self-config
		ret = true;
		app_config = config.child("app");
		title.create(app_config.child("title").child_value());
		organization.create(app_config.child("organization").child_value());
		framerate_cap = config.child("app").attribute("framerate_cap").as_int(-1);
		vsyncON = config.child("renderer").child("vsync").attribute("value").as_bool();

		if (framerate_cap > 0)
		{
			ms_capped = 1000 / framerate_cap;
		}
	}

	if(ret == true)
	{
		p2List_item<j1Module*>* item;
		item = modules.start;

		while(item != NULL && ret == true)
		{
			ret = item->data->Awake(config.child(item->data->name.GetString()));
			if (!ret)
			{
				LOG("name: %s", item->data->name.GetString());
			}
			item = item->next;
		}
	}

	PERF_PEEK(ptimer);

	return ret;
}

// Called before the first frame
bool j1App::Start()
{
	PERF_START(ptimer);

	bool ret = true;
	p2List_item<j1Module*>* item;
	item = modules.start;

	while(item != NULL && ret == true)
	{
		if (item->data->active)
		{
			ret = item->data->Start();
		}
		item = item->next;
	}

	PERF_PEEK(ptimer);

	return ret;
}

// Called each loop iteration
bool j1App::Update()
{
	bool ret = true;
	PrepareUpdate();

	if(input->GetWindowEvent(WE_QUIT) == true)
		ret = false;

	if(ret == true)
		ret = PreUpdate();

	if(ret == true)
		ret = DoUpdate();

	if(ret == true)
		ret = PostUpdate();

	FinishUpdate();
	return ret;
}

// ---------------------------------------------
pugi::xml_node j1App::LoadConfig(pugi::xml_document& config_file) const
{
	pugi::xml_node ret;
	pugi::xml_parse_result result = config_file.load_file("config.xml");

	if (result == NULL)
		LOG("Could not load map xml file config.xml. pugi error: %s", result.description());
	else
		ret = config_file.child("config");

	return ret;
}

// ---------------------------------------------
void j1App::PrepareUpdate()
{
	frame_count++;
	last_sec_frame_count++;

	ptimer.Start();
}

// ---------------------------------------------
void j1App::FinishUpdate()
{
	// Framerate calculations
	if (last_sec_frame_time.Read() > 1000)
	{
		last_sec_frame_time.Start();
		prev_last_sec_frame_count = last_sec_frame_count;
		last_sec_frame_count = 0;
	}

	avg_fps = float(frame_count) / startup_time.ReadSec();
	seconds_since_startup = startup_time.ReadSec();
	uint32 current_ms_frame = ptimer.ReadMs();
	last_frame_ms = current_ms_frame;

	if (fpsCapON)
	{
		if (ms_capped > 0 && last_frame_ms < ms_capped)
		{
			j1PerfTimer timer;
			SDL_Delay(ms_capped - last_frame_ms);
			//LOG("We waited for %d milliseconds and got back in %f", ms_capped - last_frame_ms, timer.ReadMs());
		}
	}

	framerate = 1000.0f / ptimer.ReadMs();
	dt = 1.0f / framerate;

	p2SString title("FPS: %i | Av.FPS: %.2f | MsLastFrame: %02u ms | Last dt: %.5f | FPS_Cap: %i | Vsync: %i",
		prev_last_sec_frame_count, avg_fps, last_frame_ms, dt,  fpsCapON, vsyncON);
	App->win->SetTitle(title.GetString());
}

// Call modules before each loop iteration
bool j1App::PreUpdate()
{
	bool ret = true;
	p2List_item<j1Module*>* item;
	item = modules.start;
	j1Module* pModule = NULL;

	for(item = modules.start; item != NULL && ret == true; item = item->next)
	{
		pModule = item->data;

		if(pModule->active == false) {
			continue;
		}

		ret = item->data->PreUpdate();
	}

	return ret;
}

// Call modules on each loop iteration
bool j1App::DoUpdate()
{
	bool ret = true;
	p2List_item<j1Module*>* item;
	item = modules.start;
	j1Module* pModule = NULL;

	for(item = modules.start; item != NULL && ret == true; item = item->next)
	{
		pModule = item->data;

		if(pModule->active == false) {
			continue;
		}

		ret = item->data->Update(dt);
	}

	return ret;
}

// Call modules after each loop iteration
bool j1App::PostUpdate()
{
	bool ret = true;
	p2List_item<j1Module*>* item;
	j1Module* pModule = NULL;

	for(item = modules.start; item != NULL && ret == true; item = item->next)
	{
		pModule = item->data;

		if(pModule->active == false) {
			continue;
		}

		ret = item->data->PostUpdate();
	}

	return ret;
}

// Called before quitting
bool j1App::CleanUp()
{
	bool ret = true;
	p2List_item<j1Module*>* item;
	item = modules.end;

	while(item != NULL && ret == true)
	{
		if (item->data->active)
		{
			ret = item->data->CleanUp();
		}
		item = item->prev;
	}

	return ret;
}

// ---------------------------------------
int j1App::GetArgc() const
{
	return argc;
}

// ---------------------------------------
const char* j1App::GetArgv(int index) const
{
	if(index < argc)
		return args[index];
	else
		return NULL;
}

// ---------------------------------------
const char* j1App::GetTitle() const
{
	return title.GetString();
}

// ---------------------------------------
const char* j1App::GetOrganization() const
{
	return organization.GetString();
}

bool j1App::SavegameNow() const
{
	bool ret = true;
	
	pugi::xml_document data;
	pugi::xml_node root;

	pugi::xml_document save_data;
	pugi::xml_parse_result result = save_data.load_file(save_game.GetString());

	if (result)
	{
		int i = 0;
		for(pugi::xml_document save; pugi::xml_parse_result result2 = save.load_file(save_game.GetString()); ++i)
		{
			sprintf_s(App->scene->save_file, "save_game_%u.xml", i);
			save_game = App->scene->save_file;
		}
	}
	else
	{
		App->scene->num_saves = 0;
	}

	root = data.append_child("Automatic_Variables_Saved");
	root.append_child("controls_used").append_attribute("type") = App->scene->controls_type;
	root.append_child("score").append_attribute("value") = App->scene->score;
	root.append_child("boxes_killed").append_attribute("value") = App->scene->boxes_killed;
	root.append_child("total_boxes").append_attribute("value") = App->scene->num_boxes;
	root.append_child("obstacle_dies").append_attribute("type") = App->scene->obstacle_type;
	root.child("obstacle_dies").append_attribute("ground") = App->scene->obstacle_dies->ground;

	data.save_file(save_game.GetString());
	return ret;
}