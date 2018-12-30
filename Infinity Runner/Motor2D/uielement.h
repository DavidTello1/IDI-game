#ifndef __UIELEMENT_H__
#define __UIELEMENT_H__

#include "j1Module.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Fonts.h"
#include "j1EntityController.h"


enum UIColor {
	BLUE,
	RED,
	YELLOW,
	GREY,
	GREEN
};
enum UIType 
{ 
	BUTTON,
	LABEL, 
	IMAGE, 
	NONE 
};

enum UIState
{
	HOVER,
	CLICK_DOWN,
	CLICK_UP,
	IDLE
};

enum ActionType
{
	START,
	CONTINUE,
	MAIN_MENU,
	SETTINGS,
	CHANGE_VOLUME,
	CHANGE_VOLUME_FX,
	EXIT_GAME,
	CREDITS,
	WEBSITE,
	PAUSE,
	SCORE,
	LIFE_SYSTEM,
	GAME_TIMER_SECS,
	GAME_TIMER_MINS,
	PLAYER_NAME,
	INFO,
	DYNAMIC_INFO,
	BLUE_COUNTER,
	GREEN_COUNTER,
	LAST_DEATH,
	FPS,
	HELP,
	JUMP,
	SLIDE,

	NO_ACTION
};

class UIElement
{
public:

	UIElement() {}
	UIElement(UIType type, UIElement* parent, bool visible) { 
		this->type = type; 
		this->parent = parent;
		this->visible = false;
		movable = false;
		is_moving = false;
	}

	virtual bool PreUpdate() { return true; }

	virtual bool Update() {
		
		if (parent != nullptr)
		{
			if (action != PLAYER_NAME)
			{
				position.x = parent->position.x + initial_pos.x;
				position.y = parent->position.y + initial_pos.y;
			}
		}
		return true; 
	}

	virtual void Draw(SDL_Texture* sprites = nullptr)
	{
		App->render->Blit(sprites, position.x, position.y, &rect[state]);
	}

	int CenterX(int offset = 0) {
		if (parent != nullptr)
			initial_pos.x = ((parent->rect[IDLE].w /2) - rect[IDLE].w/2) + offset;
		return initial_pos.x;
	}
	int CenterY(int offset = 0) {
		if (parent != nullptr)
			initial_pos.y = ((parent->rect[IDLE].h / 2) - rect[IDLE].h / 2) + offset;
		return initial_pos.y;
	}

	void Center(int offsetX = 0, int offsetY = 0) {
		if (parent != nullptr) {
			initial_pos.x = ((parent->rect[IDLE].w / 2) - rect[IDLE].w / 2) + offsetX;
			initial_pos.y = ((parent->rect[IDLE].h / 2) - rect[IDLE].h / 2) + offsetY;
		}
	}

	void SetInvisible() {
		p2List_item<UIElement*>* item = sons.start;
		visible = false;
		for (item; item != nullptr; item = item->next) {
			item->data->SetInvisible();
		}
	}

	void SetVisible() {
		p2List_item<UIElement*>* item = sons.start;
		visible = true;
		for (item; item != nullptr; item = item->next) {
			item->data->SetVisible();
		}
	}

	virtual void HandleAction() {}

	void DrawRect() {
		SDL_SetRenderDrawColor(App->render->renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
		//SDL_RenderDrawLine(App->render->renderer, camera_motion.x, camera_motion.y, final.x, final.y);
		SDL_Rect temp = rect[state];
		temp.x = position.x;
		temp.y = position.y;
		SDL_RenderDrawRect(App->render->renderer, &temp);
	}

public:

	SDL_Rect	rect[IDLE + 1];

	iPoint		position;
	iPoint		initial_pos;
	UIType		type;
	UIState		state;
	ActionType	action;

	p2SString	string;

	UIElement*	parent;

	j1Module*	callback;

	bool		visible;
	bool		movable;
	bool		is_moving;

	p2List<UIElement*>	sons;
};
#endif //__UIELEMENT_H__
