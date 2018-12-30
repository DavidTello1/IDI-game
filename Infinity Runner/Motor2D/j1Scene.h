#ifndef __j1SCENE_H__
#define __j1SCENE_H__

#include "j1Module.h"
#include "j1App.h"

struct SDL_Texture;
class Entity;

class j1Scene : public j1Module
{
public:

	j1Scene();

	// Destructor
	virtual ~j1Scene();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	void ChangeControls();

	//void UpdateState(UI_Element* data);
	//void UpdateSimpleUI();

public:

	p2List<p2SString*> map_names;
	uint currentMap = 0;
	float fade_time = 0.0f;
	bool pause = false;
	bool godmode = false;

	char current_score[20] = "SCORE: 0000";
	p2List_item<p2SString>* current_track;
	p2List_item<p2SString>* current_fx;

private:

	SDL_Texture*	debug_tex;
	uint			max_obstacles;
	Entity*			last_obstacle;
	uint			spacing;

public:

	uint num_obstacles;
	SDL_Rect coins_bg;
	SDL_Rect score_bg;
	SDL_Rect timer_bg;

	j1Timer timer;
	j1Timer delay;

	//vars saved
	int controls_used;
	uint score;
	uint obstacles_killed;
	Entity* obstacle_dies;

	enum Controls
	{
		WASD,
		ARROWS,
		UI_BUTTONS,
		DRAG_MOUSE
	}controls;
};

#endif // __j1SCENE_H__