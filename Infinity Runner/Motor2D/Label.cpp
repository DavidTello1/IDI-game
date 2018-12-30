#include "Label.h"

Label::Label()
{
	initial_pos		 = { 0,0 };
	position		 = { 0,0 };

	state			 = IDLE;

	string			 = nullptr;
	action			 = NO_ACTION;
}

Label::Label(ActionType action, iPoint pos, UIType type, p2SString string, UIElement* parent, bool visible, SDL_Color color) : UIElement(type, parent,visible)
{
	this->initial_pos = pos;
	this->position = pos;
	this->state = IDLE;

	this->rect[IDLE].x = position.x;
	this->rect[IDLE].y = position.y;

	this->string = string;
	this->action = action;
	this->color  = color;

	text = App->font->Print(string.GetString(), this->color, App->font->default);
	App->font->CalcSize(string.GetString(), rect[IDLE].w, rect[IDLE].h, App->font->default);

	this->rect[HOVER] = rect[IDLE];
	this->rect[CLICK_DOWN] = rect[IDLE];
	this->rect[CLICK_UP] = rect[IDLE];
}

bool Label::PreUpdate()
{
	if (!App->pause)
	{
		if (action == GAME_TIMER_SECS)
		{
			char game_time_secs[50];
			if (App->GetTimerReadSec() >= 1)
			{
				App->in_game_timer.sec++;
				App->game_timer.Start();
			}
			sprintf_s(game_time_secs, "%d", App->in_game_timer.sec);

			ChangeText(game_time_secs);
		}
		if (action == GAME_TIMER_MINS)
		{
			char game_time_mins[50];
			if (App->in_game_timer.sec == 60)
			{
				App->in_game_timer.min++;
				App->in_game_timer.sec = 0;
			}
			sprintf_s(game_time_mins, "%d", App->in_game_timer.min);

			ChangeText(game_time_mins);
		}
	}
	if (action == SCORE)
	{
		char curr_score[50];
		sprintf_s(curr_score, "%d", App->entitymanager->score);
		
		ChangeText(curr_score);
	}
	if (action == BLUE_COUNTER)
	{
		char curr_score[50];
		sprintf_s(curr_score, "%d", App->entitymanager->blue_counter);

		ChangeText(curr_score);
	}
	if (action == GREEN_COUNTER)
	{
		char curr_score[50];
		sprintf_s(curr_score, "%d", App->entitymanager->green_counter);

		ChangeText(curr_score);
	}
	if (action == PLAYER_NAME)
	{
		position.x = App->render->camera.x + App->entitymanager->player_ref->position.x + App->entitymanager->player_ref->rect.w / 2 - rect[IDLE].w / 2;
		position.y = App->render->camera.y + App->entitymanager->player_ref->position.y - 20;

		return true;
	}
	if (action == FPS) 
	{ 
		char curr_score[50]; 
		sprintf_s(curr_score, "%.2f", App->GetAvFPS()); 

		ChangeText(curr_score); 
		return true; 
	}
	return true;
}


void Label::Draw(SDL_Texture* sprites)
{
	sprites = text;
	App->render->Blit(text, position.x, position.y,NULL,0);
}

void Label::ChangeText(const char * string)
{
	if (text != nullptr)
	{
		App->tex->UnLoad(this->text);
	}

	this->text = App->font->Print(string, this->color, App->font->default);
	App->font->CalcSize(string, rect[state].w, rect[state].h, App->font->default);

	this->string = string;	
}

void Label::HandleAction() 
{
	if (action != INFO)
	{
		switch (state)
		{
		case HOVER:
			//ChangeText("HOVER");
			break;
		case IDLE:
			//ChangeText("IDLE");
			break;
		case CLICK_DOWN:
			//ChangeText("CLICKASU BRO");
			break;
		}
	}
}
