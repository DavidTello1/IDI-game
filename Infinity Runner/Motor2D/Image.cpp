#include "Image.h"

Image::Image()
{
	position	= { 0,0 };
	initial_pos = { 0,0 };

	state		= IDLE;
	type		= UIType::IMAGE;
	action		= NO_ACTION;
	parent		= nullptr;
	callback	= nullptr;
	visible		= true;
	movable		= false;
	is_moving	= false;

	current_animation = nullptr;
}

Image::Image(ActionType action,iPoint pos, SDL_Rect rect, Image* img,UIType type, UIElement* parent, bool visible) : UIElement(type, parent, visible)

{
	this->position = pos;
	this->initial_pos = pos;

	this->rect[IDLE] = rect;
	this->rect[HOVER] = rect;
	this->rect[CLICK_DOWN] = rect;
	this->rect[CLICK_UP] = rect;

	this->state = IDLE;
	this->action = action;

	if (img != nullptr)
	{
		this->idle = img->idle;
		this->blue_shine = img->blue_shine;
		this->green_shine = img->green_shine;
		this->heart_blink = img->heart_blink;
		this->current_animation = &idle;
	}
}

void Image::Draw(SDL_Texture * sprites)
{
	int phi;
	int temp;
	switch (this->action)
	{
	case INFO:

		App->render->Blit(sprites, position.x, position.y, &current_animation->GetCurrentFrame() ,0);
		break;

	case LIFE_SYSTEM:

		phi = 40;
		temp = position.x;

		if (App->entitymanager->GetAnimTimer() >= 1.5f)
			current_animation = &idle;

		if (App->entitymanager->aux_score >= App->entitymanager->score_powUp)
		{
			App->entitymanager->player_ref->lifes++;
			App->entitymanager->aux_score = App->entitymanager->aux_score - App->entitymanager->score_powUp;
			App->entitymanager->ChangeUIAnimation(&App->entitymanager->heart);
		}

		for (int i = 0; i < App->entitymanager->player_ref->lifes; i++)
		{
			App->render->Blit(sprites, position.x, position.y, &current_animation->GetCurrentFrame(), 0);
			position.x += phi;
		}

		position.x = temp;
		break;
	case LAST_DEATH:

		App->render->Blit(sprites, position.x, position.y, &rect[state], 1);
		break;
	case DYNAMIC_INFO:
		
		App->render->Blit(sprites, position.x, position.y, &current_animation->GetCurrentFrame(), 0);
		if (App->entitymanager->GetAnimTimer() >= 1.0f)
			current_animation = &idle;

		break;
	default:
		App->render->Blit(sprites, position.x, position.y, &rect[state], 0);
		break;
	}
}

