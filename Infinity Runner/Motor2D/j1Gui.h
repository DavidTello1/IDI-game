#ifndef __j1GUI_H__
#define __j1GUI_H__

#include "UIElement.h"
#include "Image.h"
#include "Button.h"
#include "Label.h"
#include "j1App.h"
#include "j1EntityController.h"


class j1Gui : public j1Module
{
public:

	j1Gui();

	// Destructor
	virtual ~j1Gui();

	// Called when before render is available
	bool Awake(pugi::xml_node&);

	// Call before first frame
	bool Start();

	//void CheckContinue();

	void PrepareMainMenuGui();

	void PrepareInGameGui();

	void SetWindowsInvisible();

	void SetWindowsVisible();

	void CalculateElementsPosition();

	// Called before all Updates
	bool PreUpdate();

	bool Update(float dt);

	// Called before quitting
	bool CleanUp();


	// Gui creation functions
	//UIElement* CreateElement(UIType type, iPoint pos, SDL_Rect rect, p2SString string = nullptr, ActionType action = NO_ACTION, j1Module* callback = nullptr, UIElement* parent = nullptr, bool visible = true);
	UIElement* CreateElement(UIType type, iPoint pos, SDL_Rect rect, Image* img = nullptr, p2SString string = nullptr, ActionType action = NO_ACTION, j1Module* callback = nullptr, UIElement* parent = nullptr, bool visible = true, SDL_Color color = { 0,0,0,0 });
	UIElement * CreateButton(iPoint pos, const Button & b, ActionType action, j1Module * callback, UIElement * parent);
	
	bool HandleInput(UIElement* element);

	const SDL_Texture* GetAtlas() const;

	Image*			in_game_gui;
	Image*			main_menu_ui;
	Image*			in_game_pause_ui;
	Image*			settings_gui;

	Image*			main_menu_window;
	Image*			in_game_window;
	Image*			credits_ui;
	Image*			settings_window;
	Image*			help_window;

	Image*			saving_point;
	Image*			game_over;
	Image*			last_death;
	Image			white_window;

	Image			blue_diamond;
	Image			green_diamond;
	Image			heart;

	Image*			green_ref;
	Image*			blue_ref;
	Image*			heart_ref;

	Button*			continue_button;
	Button*			credits_button;
	Button*			credits_to_menu;
	Button*			help_button;
	Button*			help_to_menu;
	Label*			score;
	Label*			timersec;
	Label*			fps;

	j1Timer			anim_timer;
	SDL_Color		white;
	SDL_Color		black;

	p2List<UIElement*>	windows;

private:

	p2List<UIElement*>  elements;

	SDL_Texture*		atlas;
	SDL_Texture*		blue;
	SDL_Texture*		red;
	SDL_Texture*		green;
	SDL_Texture*		yellow;
	SDL_Texture*		grey;

	p2SString			atlas_file_name;
	p2SString			blue_file_name;
	p2SString			red_file_name;
	p2SString			green_file_name;
	p2SString			yellow_file_name;
	p2SString			grey_file_name;


	Button				blue_button;
	Button				red_button;
	Button				green_button;
	Button				yellow_button;
	Button				grey_button;
	Button				slider_pointer_button;
	Button				slider_button;

	Image				game_over_image;
	Image				last_death_image;
	Image				help_image;
	Image				saving_point_image;

	//bool				moving_element;
	iPoint				last_element_motion;
	int					moving_speed;
	iPoint				mouseClick;
	iPoint				startDraging;


};

#endif // __j1GUI_H__