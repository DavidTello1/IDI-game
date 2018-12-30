#include "UIElement.h"

class Image :public UIElement
{
public:

	Image();
	Image(ActionType action, iPoint pos, SDL_Rect rect, Image* img, UIType type, UIElement* parent, bool visible);

	~Image() {}

	void Draw(SDL_Texture* sprites);

public:
	Animation * current_animation;
	Animation   idle;
	Animation	blue_shine;
	Animation	green_shine;
	Animation	heart_blink;
};
