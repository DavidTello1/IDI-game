#ifndef __J1BOX_H__
#define __J1BOX_H__
#include "j1Entity.h"

class j1Box : public Entity
{
public:
	j1Box();
	j1Box(iPoint position, iPoint Size, p2SString Type, p2SString side);
	~j1Box();

	bool Update(float dt);
	void CleanUp();

	void Load(pugi::xml_node& file);
	void Save(pugi::xml_node& file) const;

	void UpdateSide();
	void CheckSide();

public:
	Animation idle;
	Animation gripped;
};

#endif