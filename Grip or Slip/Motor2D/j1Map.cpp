#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Window.h"
#include "j1Map.h"
#include "j1Scene.h"
#include "Brofiler\Brofiler.h"
#include <cmath>


j1Map::j1Map() : j1Module(), map_loaded(false)
{
	name.create("map");
}

// Destructor
j1Map::~j1Map()
{}

// Called before render is available
bool j1Map::Awake(pugi::xml_node& config)
{
	LOG("Loading Map Parser");
	bool ret = true;

	folder.create(config.child("folder").child_value());

	return ret;
}

void j1Map::Draw(float dt)
{
	BROFILER_CATEGORY("Map Draw", Profiler::Color::Crimson);

	if (map_loaded == false)
		return;

	if (rotate == true || rotate_back == true || rotated == true) //rotate
	{
		RotateMaps(dt);
	}

	else //draw normal
	{
		for (uint lay = 0; lay < data.layers.count(); lay++)
		{
			for (uint set = 0; set < data.tilesets.count(); set++)
			{
				for (int y = 0; y < data.height; ++y)
				{
					for (int x = 0; x < data.width; ++x)
					{
						int tile_id = data.layers[lay]->Get(x, y);
						if (tile_id > 0)
						{
							TileSet* tileset = GetTilesetFromTileId(tile_id);
							SDL_Rect r = tileset->GetTileRect(tile_id);
							iPoint pos = MapToWorld(x, y);

							if (data.layers[lay]->name == "Background")
							{
								App->render->Blit(tileset->texture, pos.x, pos.y, &r, SDL_FLIP_NONE, data.layers[lay]->parallaxSpeed);
							}
							else
							{
								App->render->Blit(tileset->texture, pos.x, pos.y, &r, SDL_FLIP_NONE, data.layers[lay]->parallaxSpeed);
							}
						}
					}
				}
			}
		}
		if (debug == true) //debug draw
		{
			SDL_Rect collisions;
			for (p2List_item<ObjectsGroup*>* object = App->map->data.objLayers.start; object; object = object->next)
			{
				if (object->data->name == ("Collision"))
				{
					for (p2List_item<ObjectsData*>* objectdata = object->data->objects.start; objectdata; objectdata = objectdata->next)
					{
						collisions.x = objectdata->data->x;
						collisions.y = objectdata->data->y;
						collisions.w = objectdata->data->width;
						collisions.h = objectdata->data->height;
						if (objectdata->data->name == "Floor") //green
						{
							App->render->DrawQuad(collisions, 0, 255, 0, 50);
						}
						else if (objectdata->data->name == "Spikes") //red
						{
							App->render->DrawQuad(collisions, 255, 0, 0, 50);
						}
						else if (objectdata->data->name == "Wall") //yellow
						{
							App->render->DrawQuad(collisions, 255, 255, 0, 50);
						}
						else if (objectdata->data->name == "Grid" && objectdata->data->type == "Static") //blue
						{
							App->render->DrawQuad(collisions, 0, 0, 255, 50);
						}
						else if (objectdata->data->name == "Ceiling") //black
						{
							App->render->DrawQuad(collisions, 0, 0, 0, 50);
						}
					}
				}
			}
		}
	}
}

int Properties::Get(const char* value, int default_value) const
{
	p2List_item<Property*>* item = list.start;

	while (item)
	{
		if (item->data->name == value)
			return item->data->value;
		item = item->next;
	}

	return default_value;
}

TileSet* j1Map::GetTilesetFromTileId(int id) const
{
	// Tileset based on a tile id
	p2List_item<TileSet*>* item = data.tilesets.start;
	TileSet* set = item->data;

	while (item)
	{
		if (id < item->data->firstgid)
		{
			set = item->prev->data;
			break;
		}
		set = item->data;
		item = item->next;
	}

	return set;
}

iPoint j1Map::MapToWorld(int x, int y) const
{
	iPoint ret;

	if(data.type == MAPTYPE_ORTHOGONAL)
	{
		ret.x = x * data.tile_width;
		ret.y = y * data.tile_height;
	}
	else if(data.type == MAPTYPE_ISOMETRIC)
	{
		ret.x = (x - y) * (data.tile_width * 0.5f);
		ret.y = (x + y) * (data.tile_height * 0.5f);
	}
	else
	{
		LOG("Unknown map type");
		ret.x = x; ret.y = y;
	}

	return ret;
}

iPoint j1Map::WorldToMap(int x, int y) const
{
	iPoint ret(0,0);

	if(data.type == MAPTYPE_ORTHOGONAL)
	{
		ret.x = x / data.tile_width;
		ret.y = y / data.tile_height;
	}
	else if(data.type == MAPTYPE_ISOMETRIC)
	{
		
		float half_width = data.tile_width * 0.5f;
		float half_height = data.tile_height * 0.5f;
		ret.x = int( (x / half_width + y / half_height) / 2);
		ret.y = int( (y / half_height - (x / half_width)) / 2);
	}
	else
	{
		LOG("Unknown map type");
		ret.x = x; ret.y = y;
	}

	return ret;
}

SDL_Rect TileSet::GetTileRect(int id) const
{
	int relative_id = id - firstgid;
	SDL_Rect rect;
	rect.w = tile_width;
	rect.h = tile_height;
	rect.x = margin + ((rect.w + spacing) * (relative_id % num_tiles_width));
	rect.y = margin + ((rect.h + spacing) * (relative_id / num_tiles_width));
	return rect;
}

// Called before quitting
bool j1Map::CleanUp()
{
	LOG("Unloading map");

	// Remove all tilesets
	p2List_item<TileSet*>* item;
	item = data.tilesets.start;
	while(item != NULL)
	{
		SDL_DestroyTexture(item->data->texture);
		RELEASE(item->data);
		item = item->next;
	}
	data.tilesets.clear();

	//Remove all object layers
	p2List_item<ObjectsGroup*>* item3;
	item3 = data.objLayers.start;
	while (item3 != NULL)
	{
		RELEASE(item3->data);
		item3 = item3->next;
	}
	data.objLayers.clear();

	// Remove all layers
	p2List_item<MapLayer*>* item2;
	item2 = data.layers.start;
	while(item2 != NULL)
	{
		RELEASE(item2->data);
		item2 = item2->next;
	}
	data.layers.clear();

	// Clean up the pugui tree
	map_file.reset();

	return true;
}

// Load new map
bool j1Map::Load(const char* file_name)
{
	bool ret = true;
	p2SString tmp("%s%s", folder.GetString(), file_name);

	pugi::xml_parse_result result = map_file.load_file(tmp.GetString());

	if(result == NULL)
	{
		LOG("Could not load map xml file %s. pugi error: %s", file_name, result.description());
		ret = false;
	}

	// Load general info ----------------------------------------------
	if(ret == true)
	{
		ret = LoadMap();
	}

	// Load all tilesets info ----------------------------------------------
	pugi::xml_node tileset;
	for(tileset = map_file.child("map").child("tileset"); tileset && ret; tileset = tileset.next_sibling("tileset"))
	{
		TileSet* set = new TileSet();

		if(ret == true)
		{
			ret = LoadTilesetDetails(tileset, set);
		}

		if(ret == true)
		{
			ret = LoadTilesetImage(tileset, set);
		}

		data.tilesets.add(set);
	}

	// Load layer info ----------------------------------------------
	pugi::xml_node layer;
	for(layer = map_file.child("map").child("layer"); layer && ret; layer = layer.next_sibling("layer"))
	{
		MapLayer* lay = new MapLayer();

		ret = LoadLayer(layer, lay);

		if(ret == true)
			data.layers.add(lay);
	}

	//Load objects info ----------------------------------------------
	pugi::xml_node group;
	for (group = map_file.child("map").child("objectgroup"); group && ret; group = group.next_sibling("objectgroup"))
	{
		ObjectsGroup* set = new ObjectsGroup();

		if (ret == true)
		{
			ret = LoadObjectLayers(group, set);
		}
		data.objLayers.add(set);
	}


	if(ret == true)
	{
		LOG("Successfully parsed map XML file: %s", file_name);
		LOG("width: %d height: %d", data.width, data.height);
		LOG("tile_width: %d tile_height: %d", data.tile_width, data.tile_height);

		p2List_item<TileSet*>* item = data.tilesets.start;
		while(item != NULL)
		{
			TileSet* s = item->data;
			LOG("Tileset ----");
			LOG("name: %s firstgid: %d", s->name.GetString(), s->firstgid);
			LOG("tile width: %d tile height: %d", s->tile_width, s->tile_height);
			LOG("spacing: %d margin: %d", s->spacing, s->margin);
			item = item->next;
		}

		p2List_item<MapLayer*>* item_layer = data.layers.start;
		while(item_layer != NULL)
		{
			MapLayer* l = item_layer->data;
			LOG("Layer ----");
			LOG("name: %s", l->name.GetString());
			LOG("tile width: %d tile height: %d", l->width, l->height);
			item_layer = item_layer->next;
		}

		p2List_item<ObjectsGroup*>* obj_layer = data.objLayers.start;
		while (obj_layer != NULL)
		{
			ObjectsGroup* o = obj_layer->data;
			LOG("Group ----");
			LOG("name: %s", o->name.GetString());

			obj_layer = obj_layer->next;
		}

	}

	map_loaded = ret;

	return ret;
}

// Load map general properties
bool j1Map::LoadMap()
{
	bool ret = true;
	pugi::xml_node map = map_file.child("map");

	if(map == NULL)
	{
		LOG("Error parsing map xml file: Cannot find 'map' tag.");
		ret = false;
	}
	else
	{
		data.width = map.attribute("width").as_int();
		data.height = map.attribute("height").as_int();
		data.tile_width = map.attribute("tilewidth").as_int();
		data.tile_height = map.attribute("tileheight").as_int();
		p2SString bg_color(map.attribute("backgroundcolor").as_string());

		data.background_color.r = 0;
		data.background_color.g = 0;
		data.background_color.b = 0;
		data.background_color.a = 0;

		if(bg_color.Length() > 0)
		{
			p2SString red, green, blue;
			bg_color.SubString(1, 2, red);
			bg_color.SubString(3, 4, green);
			bg_color.SubString(5, 6, blue);

			int v = 0;

			sscanf_s(red.GetString(), "%x", &v);
			if(v >= 0 && v <= 255) data.background_color.r = v;

			sscanf_s(green.GetString(), "%x", &v);
			if(v >= 0 && v <= 255) data.background_color.g = v;

			sscanf_s(blue.GetString(), "%x", &v);
			if(v >= 0 && v <= 255) data.background_color.b = v;
		}

		p2SString orientation(map.attribute("orientation").as_string());

		if(orientation == "orthogonal")
		{
			data.type = MAPTYPE_ORTHOGONAL;
		}
		else if(orientation == "isometric")
		{
			data.type = MAPTYPE_ISOMETRIC;
		}
		else if(orientation == "staggered")
		{
			data.type = MAPTYPE_STAGGERED;
		}
		else
		{
			data.type = MAPTYPE_UNKNOWN;
		}
	}

	return ret;
}

bool j1Map::LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set)
{
	bool ret = true;
	set->name.create(tileset_node.attribute("name").as_string());
	set->firstgid = tileset_node.attribute("firstgid").as_int();
	set->tile_width = tileset_node.attribute("tilewidth").as_int();
	set->tile_height = tileset_node.attribute("tileheight").as_int();
	set->margin = tileset_node.attribute("margin").as_int();
	set->spacing = tileset_node.attribute("spacing").as_int();
	pugi::xml_node offset = tileset_node.child("tileoffset");

	if(offset != NULL)
	{
		set->offset_x = offset.attribute("x").as_int();
		set->offset_y = offset.attribute("y").as_int();
	}
	else
	{
		set->offset_x = 0;
		set->offset_y = 0;
	}

	return ret;
}

bool j1Map::LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set)
{
	bool ret = true;
	pugi::xml_node image = tileset_node.child("image");

	if(image == NULL)
	{
		LOG("Error parsing tileset xml file: Cannot find 'image' tag.");
		ret = false;
	}
	else
	{
		set->texture = App->tex->Load(PATH(folder.GetString(), image.attribute("source").as_string()));
		int w, h;
		SDL_QueryTexture(set->texture, NULL, NULL, &w, &h);
		set->tex_width = image.attribute("width").as_int();

		if(set->tex_width <= 0)
		{
			set->tex_width = w;
		}

		set->tex_height = image.attribute("height").as_int();

		if(set->tex_height <= 0)
		{
			set->tex_height = h;
		}

		set->num_tiles_width = set->tex_width / set->tile_width;
		set->num_tiles_height = set->tex_height / set->tile_height;
	}

	return ret;
}

bool j1Map::LoadLayer(pugi::xml_node& node, MapLayer* layer)
{
	bool ret = true;

	layer->name = node.attribute("name").as_string();
	layer->width = node.attribute("width").as_uint();
	layer->height = node.attribute("height").as_uint();
	LoadProperties(node, layer->properties);
	layer->data = new uint[layer->width * layer->height];
	layer->parallaxSpeed = node.child("properties").child("property").attribute("value").as_float(0.0f);

	pugi::xml_node layer_data = node.child("data");

	if(layer_data == NULL)
	{
		LOG("Error parsing map xml file: Cannot find 'layer/data' tag.");
		ret = false;
		RELEASE(layer);
	}
	else
	{
		layer->data = new uint[layer->width*layer->height];
		memset(layer->data, 0, layer->width*layer->height);

		int i = 0;
		for(pugi::xml_node tile = layer_data.child("tile"); tile; tile = tile.next_sibling("tile"))
		{
			layer->data[i++] = tile.attribute("gid").as_int(0);
		}
	}

	return ret;
}

bool j1Map::LoadObjectLayers(pugi::xml_node & node, ObjectsGroup * group)
{
	bool ret = true;

	group->name = node.attribute("name").as_string();

	for (pugi::xml_node& object = node.child("object"); object && ret; object = object.next_sibling("object"))
	{
		ObjectsData* data = new ObjectsData;

		data->height = object.attribute("height").as_uint();
		data->width = object.attribute("width").as_uint();
		data->x = object.attribute("x").as_uint();
		data->y = object.attribute("y").as_uint();
		data->name = object.attribute("name").as_string();
		data->type = object.attribute("type").as_string();

		group->objects.add(data);
	}
	return ret;
}

// Load a group of properties from a node and fill a list with it
bool j1Map::LoadProperties(pugi::xml_node& node, Properties& properties)
{
	bool ret = false;

	pugi::xml_node data = node.child("properties");

	if (data != NULL)
	{
		pugi::xml_node properties_node;

		for (properties_node = data.child("property"); properties_node != NULL; properties_node = properties_node.next_sibling("property"))
		{
			Properties::Property* property = new Properties::Property();

			property->name = properties_node.attribute("name").as_string();
			property->value = properties_node.attribute("value").as_int();

			properties.list.add(property);
		}
	}
	return ret;
}

bool j1Map::SwitchMaps(p2SString* new_map)
{
	CleanUp();
	Load(new_map->GetString());
	App->scene->to_end = false;

	return true;
}

bool j1Map::SwitchMaps2(p2SString* new_map)
{
	CleanUp();
	Load(new_map->GetString());

	return true;
}

void j1Map::RotateMaps(float dt)
{
	BROFILER_CATEGORY("RotateMaps", Profiler::Color::Purple);

	if (rotate == true) //rotate
	{
		if (angle >= 90)
		{
			rotate = false;
			rotated = true;
		}
		else
		{
			for (uint lay = 0; lay < data.layers.count(); lay++)
			{
				for (uint set = 0; set < data.tilesets.count(); set++)
				{
					for (int y = 0; y < data.height; ++y)
					{
						for (int x = 0; x < data.width; ++x)
						{
							int tile_id = data.layers[lay]->Get(x, y);
							if (tile_id > 0)
							{
								TileSet* tileset = GetTilesetFromTileId(tile_id);

								SDL_Rect r = tileset->GetTileRect(tile_id);
								iPoint pos = MapToWorld(x, y);
								iPoint new_pos;

								new_pos.x = pos.x * cos(angle* 3.14159265359 / 180) + pos.y * sin(angle* 3.14159265359 / 180);
								new_pos.y = pos.y * cos(angle* 3.14159265359 / 180) - pos.x * sin(angle* 3.14159265359 / 180);
								App->render->Blit(tileset->texture, new_pos.x, new_pos.y, &r, SDL_FLIP_NONE, 1.0f, -angle);
							}
						}
					}
				}
			}
			angle += ceilf(50*dt);
		}
	}
	else if (rotated == true)
	{
		rotated = false;
		angle = 0.0;
		rotate_back = true;
	}
	else if (rotate_back == true) //rotate backwards
	{
		if (angle >= 90.0)
		{
			rotate_back = false;
			rotate_end = true;
		}
		else
		{
			for (uint lay = 0; lay < data.layers.count(); lay++)
			{
				for (uint set = 0; set < data.tilesets.count(); set++)
				{
					for (int y = 0; y < data.height; ++y)
					{
						for (int x = 0; x < data.width; ++x)
						{
							int tile_id = data.layers[lay]->Get(x, y);
							if (tile_id > 0)
							{
								TileSet* tileset = GetTilesetFromTileId(tile_id);
								SDL_Rect r = tileset->GetTileRect(tile_id);
								iPoint pos = MapToWorld(-(y + 1), x);
								iPoint new_pos;

								new_pos.x = pos.x * cos(angle * PI / 180) + pos.y * sin(angle * PI / 180);
								new_pos.y = pos.y * cos(angle * PI / 180) - pos.x * sin(angle * PI / 180);
								App->render->Blit(tileset->texture, new_pos.x, new_pos.y - data.tile_height, &r, SDL_FLIP_NONE, 1.0f, -angle + 90);
							}
						}
					}
				}
			}
			angle += ceilf(50 * dt);
		}
	}
}

bool j1Map::CreateWalkabilityMap(int& width, int& height, uchar** buffer) const
{
	bool ret = false;
	p2List_item<MapLayer*>* item;
	item = data.layers.start;
	for (item = data.layers.start; item != NULL; item = item->next)
	{
		MapLayer* layer = item->data;
		if (layer->properties.Get("Navigation") == 0)
			continue;
		uchar* map = new uchar[layer->width*layer->height];
		memset(map, 1, layer->width*layer->height);
		for (int y = 0; y < data.height; ++y)
		{
			for (int x = 0; x < data.width; ++x)
			{
				int i = (y*layer->width) + x;
				int tile_id = layer->Get(x, y);
				TileSet* tileset = (tile_id > 0) ? GetTilesetFromTileId(tile_id) : NULL;
				if (tileset != NULL)
				{
					map[i] = (tile_id - tileset->firstgid) > 0 ? 0 : 1;
					
				}
			}
		}
		*buffer = map;
		width = data.width;
		height = data.height;
		ret = true;
		break;
	}
	return ret;
}