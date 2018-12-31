#include "j1EntityController.h"
#include "j1App.h"
#include "j1Audio.h"
#include "j1Render.h"
#include "j1Scene.h"
#include "j1Entity.h"
#include "j1Player.h"
#include "Obstacles.h"
#include "j1Textures.h"

#include "PugiXml/src/pugixml.hpp"

j1EntityController::j1EntityController()
{
	name.create("entitycontroller");
}

j1EntityController::~j1EntityController()
{
}

bool j1EntityController::Awake(pugi::xml_node &config)
{
	folder.create(config.child("folder").child_value());
	texture_path = config.child("sprite_sheet").attribute("source").as_string();

	want_jump = false;
	want_slide = false;
	
	return true;
}

bool j1EntityController::Start()
{
	player = App->tex->Load("textures/spritesheet32.png");
	texture = App->tex->Load(PATH(folder.GetString(), texture_path.GetString()));

	return true;
}

bool j1EntityController::PreUpdate()
{
	if (!App->scene->pause)
	{
		p2List_item<Entity*>* tmp = Entities.start;
		while (tmp != nullptr)
		{
			tmp->data->PreUpdate();
			tmp = tmp->next;
		}
	}
	return true;
}

bool j1EntityController::Update(float dt)
{
	bool ret = true;

	if (!App->scene->pause)
	{
		p2List_item<Entity*>* tmp = Entities.start;
		while (tmp != nullptr)
		{
			ret = tmp->data->Update(dt);
			tmp = tmp->next;
		}
	}
	return ret;
}

bool j1EntityController::PostUpdate()
{
	if (!App->scene->pause)
	{
		p2List_item<Entity*>* tmp = Entities.start;
		while (tmp != nullptr)
		{
			tmp->data->PostUpdate();
			tmp = tmp->next;
		}
	}
	return true;
}

bool j1EntityController::CleanUp()
{
	DeleteEntities();
	App->tex->UnLoad(texture);
	App->tex->UnLoad(player);
	return true;
}

void j1EntityController::DeleteEntities()
{
	p2List_item<Entity*>* tmp = Entities.start;
	while (tmp != nullptr)
	{
		RELEASE(tmp->data);
		tmp = tmp->next;
	}
	Entities.clear();
}

bool j1EntityController::Draw()
{
	bool ret = true;
	p2List_item<Entity*>* tmp = Entities.start;
	p2List_item<Entity*>* tmp2 = nullptr;

	while (tmp != nullptr)
	{
		if (tmp->data->type == Entity::entityType::PLAYER)
		{
			tmp2 = tmp;
		}
		else
		{
			tmp->data->Draw();
		}
		tmp = tmp->next;
	}

	if (tmp2 != nullptr)
	{
		tmp2->data->Draw(); //draw player last
	}

	return ret;
}

Entity* j1EntityController::AddEntity(Entity::entityType type, bool ground, iPoint pos)
{
	Entity* tmp = nullptr;

	switch (type)
	{
	case Entity::entityType::BOX:
		tmp = new Obstacles(type, ground, pos);
		break;

	case Entity::entityType::WALL:
		tmp = new Obstacles(type, ground, pos);
		break;

	case Entity::entityType::SAW:
		tmp = new Obstacles(type, ground, pos);
		break;


	case Entity::entityType::PLAYER:
		tmp = new j1Player();
		break;
	}

	if (tmp)
		Entities.add(tmp);

	return tmp;
}

bool j1EntityController::DeleteEntity(Entity * entity)
{
	entity->CleanUp();
	Entities.del(Entities.At(Entities.find(entity)));
	return true;
}