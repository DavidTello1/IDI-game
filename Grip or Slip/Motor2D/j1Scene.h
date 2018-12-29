#ifndef __j1SCENE_H__
#define __j1SCENE_H__

#include "j1Module.h"
#include "j1App.h"

struct SDL_Texture;

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

	bool Load(pugi::xml_node& data);
	bool Save(pugi::xml_node& data) const;

	bool Load_level(int map);
	void SpawnEnemies();
	void SpawnEntities();
	void SaveAndChange();
	void ResetBoxPos();
	void ResetIngameUI();
	void ResetEnemies();

	//void UpdateState(UI_Element* data);
	//void UpdateSimpleUI();

public:

	p2List<p2SString*> map_names;
	uint currentMap = 0;
	float fade_time = 0.0f;
	bool to_end = false;
	bool change = false;
	bool pause = false;

	bool godmode = false;

	bool box_1_side = false;
	bool box_2_side = false;
	bool box_3_side = false;
	bool box_4_side = false;

	uint coins = 0;
	uint score = 0;
	uint countdown = 0;

	int player_lives = 0;
	char current_score[20] = "SCORE: 0000";
	char current_coins[10] = "x0";
	char current_time[20] = "TIME: 000";
	char current_vol[20] = "MUSIC: 0";
	char current_sfx[20] = "SOUND: 0";


	bool finish_1 = false;
	bool finish_2 = false;
	bool end_game = false;

	iPoint x_limit;
	p2List_item<p2SString>* current_track;
	p2List_item<p2SString>* current_fx;

private:

	SDL_Texture* debug_tex;
	bool copy = false;

public:

	SDL_Rect coins_bg;
	SDL_Rect score_bg;
	SDL_Rect timer_bg;

	j1Timer timer;
	j1Timer delay;
};

#endif // __j1SCENE_H__