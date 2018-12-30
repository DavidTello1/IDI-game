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

	//Called each loop iteration
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();

	// Called before quitting
	void CleanUp();

	// Positions the player in the start of the current level
	void Restart();

	void ChangeAnimation();
	void LoadAnimations();
	void PlayerOnFloor();

	void Collider_Overlay();

public:

	Animation	idle;
	Animation	jump;
	Animation	fall;
	Animation	start_slide;
	Animation	slide;
	Animation	finish_slide;

	int			floor;

	int			dx;
	int			dy;

	bool		jumping;
	bool		start_sliding;
	bool		sliding;
	bool		finish_sliding;
	bool		attack;
	bool		falling;
	bool		grounded;

	bool		is_jump;
	bool		is_fall;

	int gravity;
	int lives;
	int jumpSpeed;

};
#endif // !__J1PLAYER_H__