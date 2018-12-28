#include "j1EntityController.h"
#include "j1App.h"
#include "j1Audio.h"
#include "j1Render.h"
#include "j1Pathfinding.h"
#include "j1Scene.h"
#include "j1SceneChange.h"
#include "j1Entity.h"
#include "j1Player.h"
#include "j1Grid.h"
#include "j1FlyingEnemy.h"
#include "j1LandEnemy.h"
#include "j1Box.h"
#include "j1Coins.h"
#include "j1Textures.h"

#include "Brofiler\Brofiler.h"
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
	bool ret = true;
	folder.create(config.child("folder").child_value());
	texture_path = config.child("sprite_sheet").attribute("source").as_string();

	return ret;
}

bool j1EntityController::Start()
{
	bool ret = true;
	texture = App->tex->Load(PATH(folder.GetString(), texture_path.GetString()));
	debug_tex = App->tex->Load("maps/pathfinding.png");

	return ret;
}

bool j1EntityController::Update(float dt)
{
	BROFILER_CATEGORY("EntityController Update", Profiler::Color::Blue);

	bool ret = true;

	if (App->map->debug)
	{
		DebugDraw();
	}

	if (App->scene->change == false && App->scene->pause == false)
	{
		EnemyColliderCheck();

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
	BROFILER_CATEGORY("EntityController PostUpdate", Profiler::Color::Blue);

	bool ret = true;

	if (App->scene->pause == false)
	{
		p2List_item<Entity*>* tmp = Entities.start;
		while (tmp != nullptr)
		{
			tmp->data->PostUpdate();
			tmp = tmp->next;
		}
	}
	
	return ret;
}

bool j1EntityController::CleanUp()
{
	DeleteEntities();
	App->tex->UnLoad(texture);
	App->tex->UnLoad(debug_tex);

	return true;
}

bool j1EntityController::Save(pugi::xml_node& file) const
{
	bool ret = true;
	if (App->scene->currentMap == 0)
	{
		pugi::xml_node param = file.append_child("map_1");
		p2List_item<Entity*>* tmp = Entities.start;
		while (tmp != nullptr)
		{
			tmp->data->Save(param);
			tmp = tmp->next;
		}
	}
	else if (App->scene->currentMap == 1)
	{
		file = file.append_child("map_2");
		p2List_item<Entity*>* tmp = Entities.start;
		while (tmp != nullptr)
		{
			tmp->data->Save(file);
			tmp = tmp->next;
		}
	}
	return ret;
}

bool j1EntityController::CopySave(pugi::xml_node& file)
{
	bool ret = true;

	p2List_item<Entity*>* tmp = Entities.start;
	while (tmp != nullptr)
	{
		tmp->data->Save(file);
		tmp = tmp->next;
	}

	return ret;
}

bool j1EntityController::AppendSave(pugi::xml_node& source, pugi::xml_node& destiny)
{
	bool ret = true;

	if (App->scene->currentMap == 1)
	{
		if (destiny.child("map_1") != NULL) //map1 data already exists (not to date)
		{
			for (pugi::xml_node tmp = destiny.child("map_1"); tmp; tmp = tmp.next_sibling()) //remove map1 data
			{
				destiny.remove_child(tmp.name());
				for (pugi::xml_node tmp2 = tmp.first_child(); tmp2; tmp2 = tmp2.next_sibling())
				{
					destiny.remove_child(tmp2.name());

					for (pugi::xml_attribute attr = tmp2.first_attribute(); attr; attr = attr.next_attribute())
					{
						destiny.remove_attribute(attr.name());
					}
				}
			}
		}
		if (destiny.child("map_2") != NULL) //map2 data already exists
		{
			destiny = destiny.insert_child_before("map_1", destiny.child("map_2"));  //append map1 before map2
		}
		else
		{
			destiny = destiny.append_child("map_1"); //if there is no map2 data append map1
		}
	}
	else if (App->scene->currentMap == 0)
	{
		if (destiny.child("map_2") != NULL) //map2 data already exists (not to date)
		{
			for (pugi::xml_node tmp = destiny.child("map_2"); tmp; tmp = tmp.next_sibling()) //remove map2 data
			{
				destiny.remove_child(tmp.name());
				for (pugi::xml_node tmp2 = tmp.first_child(); tmp2; tmp2 = tmp2.next_sibling())
				{
					destiny.remove_child(tmp2.name());

					for (pugi::xml_attribute attr = tmp2.first_attribute(); attr; attr = attr.next_attribute())
					{
						destiny.remove_attribute(attr.name());
					}
				}
			}
		}
		if (destiny.child("map_1") != NULL) //map1 data already exists
		{
			destiny = destiny.insert_child_after("map_2", destiny.child("map_1"));  //append map2 after map1
		}
		else
		{
			destiny = destiny.append_child("map_2"); //if there is no map1 data append map2
		}
	}

	pugi::xml_node destiny_source = destiny;
	for (pugi::xml_node tmp = source.first_child(); tmp; tmp = tmp.next_sibling()) //traverse copysave.xml (entity type)
	{
		destiny = destiny_source.append_child(tmp.name()); //append child to savegame.xml

		for (pugi::xml_node tmp2 = tmp.first_child(); tmp2; tmp2 = tmp2.next_sibling()) //traverse copysave.xml (variable names)
		{
			destiny.append_child(tmp2.name()); //append child to savegame.xml

			for (pugi::xml_attribute attr = tmp2.first_attribute(); attr; attr = attr.next_attribute()) //traverse copysave.xml (attributes)
			{
				destiny.child(tmp2.name()).append_attribute(attr.name()) = attr.value(); //append attribute to save_game.xml
			}
		}
	}

	return ret;
}

bool j1EntityController::Load(pugi::xml_node& file)
{
	bool ret = true;

	DeleteEntities();
	App->scene->SpawnEntities();

	if (App->scene->currentMap == 0)
	{
		file = file.child("map_1");
	}
	else if (App->scene->currentMap == 1)
	{
		file = file.child("map_2");
	}

	p2List_item<Entity*>* tmp = Entities.start;
	pugi::xml_node box = file.child("box");
	pugi::xml_node grid = file.child("grid");
	pugi::xml_node flying_enemy = file.child("flying_enemy");
	pugi::xml_node land_enemy = file.child("land_enemy");
	pugi::xml_node coin = file.child("coin");

	while (tmp != nullptr)
	{
		if (tmp->data->type == Entity::entityType::BOX)
		{
			tmp->data->Load(box);
			box = box.next_sibling("box");
		}
		else if (tmp->data->type == Entity::entityType::GRID)
		{
			tmp->data->Load(grid);
			grid = grid.next_sibling("grid");
		}
		else if (tmp->data->type == Entity::entityType::FLYING_ENEMY)
		{
			tmp->data->Load(flying_enemy);
			flying_enemy = flying_enemy.next_sibling("flying_enemy");
		}
		else if (tmp->data->type == Entity::entityType::LAND_ENEMY)
		{
			tmp->data->Load(land_enemy);
			land_enemy = land_enemy.next_sibling("land_enemy");
		}
		else if (tmp->data->type == Entity::entityType::COIN)
		{
			tmp->data->Load(coin);
			coin = coin.next_sibling("coin");
		}
		else if (tmp->data->type == Entity::entityType::PLAYER)
		{
			tmp->data->Load(file.child("player"));
		}
		tmp = tmp->next;
	}
	return ret;
}

bool j1EntityController::Restart()
{
	bool ret = true;

	DeleteEnemies();
	App->scene->SpawnEnemies();
	p2List_item<Entity*>* tmp = Entities.end;
	while (tmp != nullptr)
	{
		if (tmp->data->type == Entity::entityType::PLAYER)
		{
			tmp->data->Restart();
			break;
		}
		tmp = tmp->prev;
	}

	return ret;
}

void j1EntityController::DeleteEnemies()
{
	p2List_item<Entity*>* tmp = Entities.end;
	while (tmp != nullptr)
	{
		if (tmp->data->type != Entity::entityType::PLAYER)
		{
			Entities.del(tmp);
			RELEASE(tmp->data);
		}
		tmp = tmp->prev;
	}
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

bool j1EntityController::Draw(float dt)
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
			tmp->data->Draw(dt);
		}
		tmp = tmp->next;
	}
	if (tmp2 != nullptr)
	{
		tmp2->data->Draw(dt); //draw player last
	}

	return ret;
}

bool j1EntityController::DebugDraw()
{
	p2List_item<Entity*>* tmp = Entities.start;
	SDL_Rect col;
	SDL_Rect col2;
	while (tmp != nullptr)
	{
		col.x = tmp->data->Collider.x;
		col.y = tmp->data->Collider.y;
		col.h = tmp->data->Collider.h;
		col.w = tmp->data->Collider.w;
		App->render->DrawQuad(col, 0, 0, 255, 50); //blue

		if (tmp->data->type == Entity::entityType::FLYING_ENEMY || tmp->data->type == Entity::entityType::LAND_ENEMY)
		{
			if (tmp->data->dead == false)
			{
				col2.x = tmp->data->SightCollider.x;
				col2.y = tmp->data->SightCollider.y;
				col2.h = tmp->data->SightCollider.h;
				col2.w = tmp->data->SightCollider.w;
				App->render->DrawQuad(col2, 255, 0, 0, 50); //red
			}
		}
		tmp = tmp->next;
	}

	return true;
}

Entity* j1EntityController::AddEntity(Entity::entityType type, iPoint position, iPoint Size, p2SString Type, p2SString side)
{
	Entity* tmp = nullptr;

	switch (type)
	{
	case Entity::entityType::BOX:
		tmp = new j1Box(position, Size, Type, side);
		break;

	case Entity::entityType::FLYING_ENEMY:
		tmp = new FlyingEnemy(position);
		break;

	case Entity::entityType::LAND_ENEMY:
		tmp = new LandEnemy(position);
		break;

	case Entity::entityType::GRID:
		tmp = new j1Grid(position, Size, Type);
		break;

	case Entity::entityType::PLAYER:
		tmp = new j1Player();
		break;

	case Entity::entityType::COIN:
		tmp = new j1Coins(position, Size);
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


void j1EntityController::EnemyColliderCheck()
{
	p2List_item<Entity*>* player = nullptr;
	for (p2List_item<Entity*>* i = Entities.start; i != nullptr; i = i->next)
	{
		if (i->data->type == Entity::entityType::PLAYER)
		{
			player = i;
			break;
		}
	}
	p2List_item<Entity*>* tmp = Entities.start;
	while (tmp != nullptr)
	{
		if (tmp->data->type == Entity::entityType::LAND_ENEMY || tmp->data->type == Entity::entityType::FLYING_ENEMY)
		{
			if (tmp->data->dead == false)
			{

				if (SDL_HasIntersection(&tmp->data->SightCollider, &player->data->Collider))
				{
					tmp->data->chasing_player = true;
				}
				else
				{
					tmp->data->chasing_player = false;
				}
				if (SDL_HasIntersection(&tmp->data->Collider, &player->data->Collider) && !App->scenechange->IsChanging())
				{
					if (player->data->attack)
					{
						if (tmp->data->type == Entity::entityType::FLYING_ENEMY && tmp->data->dead == false)
						{
							tmp->data->dead = true;
							App->scene->score += 500;
						}
						else if (tmp->data->hurt == false)
						{
							tmp->data->lives--;
							tmp->data->hurt = true;
						}
					}
					else
					{
						if (!App->scene->godmode)
						{
							player->data->dead = true;;
						}
					}
				}
			}
		}
		else if (tmp->data->type == Entity::entityType::COIN)
		{
			if (SDL_HasIntersection(&tmp->data->Collider, &player->data->Collider))
			{
				if (tmp->data->dead == false)
				{
					tmp->data->dead = true;
					App->audio->PlayFx(COIN);
					App->scene->coins++;
					if (App->scene->coins == 10)
					{
						App->scene->score += 1000;
						App->audio->PlayFx(COIN);
					}
					App->scene->score += 100;
				}
			}
		}
		tmp = tmp->next;
	}
}

void j1EntityController::PlayerRestart()
{
	p2List_item<Entity*>* tmp = Entities.end;
	while (tmp != nullptr)
	{
		if (tmp->data->type == Entity::entityType::PLAYER)
		{
			tmp->data->Restart();
			break;
		}
		tmp = tmp->prev;
	}
}