#ifndef __j1SceneChange_H__
#define __j1SceneChange_H__

#include "j1Module.h"
#include "j1Timer.h"
#include "SDL\include\SDL_rect.h"

enum ListOfMapNames;

class j1SceneChange : public j1Module
{
public:

	j1SceneChange();

	//Destructor
	virtual ~j1SceneChange();

	//Called before render is avalible
	bool Awake(pugi::xml_node&);

	//Called before the first frame
	bool Start();

	//Called each loop iteration
	bool Update(float dt);

	//Change Map
	bool ChangeMap(int newMap, float time);

	//returns true if changing
	bool IsChanging() const;

public:
	bool fading = false;

private:
	enum fade_step
	{
		none,
		fade_to_black,
		fade_from_black
	} 
	current_step = fade_step::none;
	
	int nextMap;
	uint start_time = 0;
	uint total_time = 0;
	SDL_Rect screen;
};

#endif // __j1SceneChange_H__