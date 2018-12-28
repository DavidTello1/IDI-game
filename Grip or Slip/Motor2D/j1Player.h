#ifndef __J1PLAYER_H__
#define __J1PLAYER_H__

#include "p2List.h"
#include "p2Point.h"
#include "j1Module.h"
#include "j1Entity.h"

struct SDL_Texture;
struct ObjectsData;

class j1Player : public Entity
{
public:

	j1Player();

	// Destructor
	~j1Player();

	// Called before render is available
	bool Awake(pugi::xml_node& config);

	//Called before the first frame
	bool Start();

	bool Update(float dt);
	bool PostUpdate();

	// Called each loop iteration

	void Load(pugi::xml_node& file);
	void Save(pugi::xml_node& file) const;

	// Called before quitting
	void CleanUp();

	// Positions the player in the start of the current level
	 void Restart();

	void ChangeAnimation();
	void LoadAnimations();
	void CameraOnPlayer();

public: 

	Animation	idle;
	Animation	move_anim;
	Animation	jump_anim;
	Animation	falling_anim;
	Animation	grip_idle;
	Animation	grip_move;
	Animation	slide_anim;
	Animation	landing_anim;
	Animation	grip_anim;
	Animation	dying;
	Animation	start_attacking;
	Animation	attacking;
	Animation	end_attacking;
};

#endif // !__J1PLAYER_H__