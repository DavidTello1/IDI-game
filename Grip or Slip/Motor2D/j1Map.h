#ifndef __j1MAP_H__
#define __j1MAP_H__

#include "PugiXml/src/pugixml.hpp"
#include "p2List.h"
#include "p2Point.h"
#include "j1Module.h"
#include "SDL\include\SDL_rect.h"

struct SDL_Texture;

struct Properties
{
	struct Property
	{
		p2SString name;
		int value;
	};

	~Properties()
	{
		p2List_item<Property*>* item;
		item = list.start;

		while (item != NULL)
		{
			RELEASE(item->data);
			item = item->next;
		}

		list.clear();
	}

	int Get(const char* name, int default_value = 0) const;

	p2List<Property*>	list;
};

// ----------------------------------------------------
struct MapLayer
{
	p2SString	name;
	uint		width;
	uint		height;
	uint*		data;
	Properties	properties;
	float		parallaxSpeed;

	MapLayer() : data(NULL)
	{}

	~MapLayer()
	{
		RELEASE(data);
	}

	inline uint Get(int x, int y) const
	{
		return data[(y*width) + x];
	}
};

struct ObjectsData
{
	p2SString	name;	
	p2SString	type;
	int			x;
	int			y;
	int		width;
	int		height;

};

struct ObjectsGroup
{
	p2SString				name;
	p2List<ObjectsData*>	objects;

	~ObjectsGroup()
	{
		p2List_item<ObjectsData*>* item;
		item = objects.start;

		while (item != NULL)
		{
			RELEASE(item->data);
			item = item->next;
		}

		objects.clear();
	}
};


// ----------------------------------------------------
struct TileSet
{
	SDL_Rect GetTileRect(int id) const;

	p2SString			name;
	int					firstgid;
	int					margin;
	int					spacing;
	int					tile_width;
	int					tile_height;
	SDL_Texture*		texture;
	int					tex_width;
	int					tex_height;
	int					num_tiles_width;
	int					num_tiles_height;
	int					offset_x;
	int					offset_y;
};

enum MapTypes
{
	MAPTYPE_UNKNOWN = 0,
	MAPTYPE_ORTHOGONAL,
	MAPTYPE_ISOMETRIC,
	MAPTYPE_STAGGERED
};
// ----------------------------------------------------
struct MapData
{
	int						width;
	int						height;
	int						tile_width;
	int						tile_height;
	SDL_Color				background_color;
	MapTypes				type;
	p2List<TileSet*>		tilesets;
	p2List<MapLayer*>		layers;
	p2List<ObjectsGroup*>	objLayers;
};

// ----------------------------------------------------
class j1Map : public j1Module
{
public:

	j1Map();

	// Destructor
	virtual ~j1Map();

	// Called before render is available
	bool Awake(pugi::xml_node& conf);

	// Called each loop iteration
	void Draw(float dt);

	// Called before quitting
	bool CleanUp();

	// Load new map
	bool Load(const char* path);

	//Unloads the current map and loads a new one 
	bool SwitchMaps(p2SString* new_map);

	//same as switchmaps but without scene->to_end
	bool SwitchMaps2(p2SString* new_map);

	iPoint MapToWorld(int x, int y) const;
	iPoint WorldToMap(int x, int y) const;

	void RotateMaps(float dt);
	bool CreateWalkabilityMap(int& width, int& height, uchar** buffer) const;

private:

	bool LoadMap();
	bool LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set);
	bool LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set);
	bool LoadLayer(pugi::xml_node& node, MapLayer* layer);
	bool LoadProperties(pugi::xml_node& node, Properties& properties);
	bool LoadObjectLayers(pugi::xml_node& node, ObjectsGroup* group);

	TileSet* GetTilesetFromTileId(int id) const;

public:

	MapData				data;
	bool				debug = false;
	bool				rotate = false;
	bool				rotate_back = false;
	bool				rotated = false;
	bool				rotate_end = true;
	double				angle = 0.0;
private:

	pugi::xml_document	map_file;
	p2SString			folder;
	bool				map_loaded;
};

#endif // __j1MAP_H__