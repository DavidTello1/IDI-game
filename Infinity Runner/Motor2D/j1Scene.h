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
	void Restart();

	//void UpdateState(UI_Element* data);
	//void UpdateSimpleUI();

private:
	uint			max_obstacles;
	Entity*			last_obstacle;

public:

	p2List<p2SString*> map_names;
	bool player_dead;
	bool pause;

	p2List_item<p2SString>* current_track;
	p2List_item<p2SString>* current_fx;

	uint spacing;
	uint num_obstacles;

	char save_file[20] = "save_game_0.xml";
	uint num_saves;

	char current_score[20] = "SCORE: 0000";
	SDL_Texture* score_tex;
	iPoint score_size;

	char current_controls[80] = "CONTROLS: ";
	SDL_Texture* controls_tex;
	iPoint controls_size;
	const char* controls_type;


	bool game_over;
	SDL_Texture* game_over_tex;
	SDL_Rect* game_over_rect;

	j1Timer timer;
	j1Timer delay;

	//vars saved
	int controls_used;
	uint score;
	uint boxes_killed;
	uint num_boxes;
	Entity* obstacle_dies;
	const char* obstacle_type;

	enum Controls
	{
		WASD,
		ARROWS,
		MOUSE_CLICK,
		DRAG_MOUSE
	}controls;
};

#endif // __j1SCENE_H__