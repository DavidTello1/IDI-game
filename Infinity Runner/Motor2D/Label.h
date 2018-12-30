#include "UIElement.h"
#include "j1Textures.h"
#include <string>

class Label :public UIElement
{
public:

	Label();
	Label(ActionType action, iPoint pos, UIType type, p2SString string, UIElement* parent, bool visible, SDL_Color color);

	bool PreUpdate();

	void Draw(SDL_Texture * sprites);

	void ChangeText(const char* string);

	void HandleAction();

public:

	SDL_Texture* text;
	SDL_Color	 color;
};