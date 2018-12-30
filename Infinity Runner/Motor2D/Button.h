#include "UIElement.h"

class Button : public UIElement
{
public:

	Button();
	Button(iPoint pos,const Button &b,ActionType action, UIElement* parent = nullptr);

	~Button() {}

	void HandleAction();

public:

	UIColor color;
};