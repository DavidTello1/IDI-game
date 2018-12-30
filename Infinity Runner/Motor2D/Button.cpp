#include "Button.h"

Button::Button()
{
	initial_pos = { 0,0 };
	position    = { 0,0 };

	state	    = IDLE;
			    
	type	    = UIType::BUTTON;
	action      = NO_ACTION;
	parent      = nullptr;
	callback    = nullptr;
	visible     = true;
	color       = GREY;
	movable     = false;
	is_moving   = false;
}

Button::Button(iPoint pos,const Button & b,ActionType action, UIElement* parent) : UIElement(b.type, parent, b.visible)
{
	this->initial_pos = pos;
	this->position = pos;

	this->state = IDLE;
	this->action = action;

	this->callback = b.callback;

	this->rect[IDLE]		= b.rect[IDLE];
	this->rect[HOVER]		= b.rect[HOVER];
	this->rect[CLICK_DOWN]	= b.rect[CLICK_DOWN];
	this->rect[CLICK_UP]	= b.rect[CLICK_UP];

	this->color = b.color;
}

void Button::HandleAction()
{
	switch (action)
	{
	case JUMP:
		App->entitycontroller->want_jump = true;
		break;
	case SLIDE:
		App->entitycontroller->want_slide = true;
		break;
	}
}
