/*
 * =====================================================================================
 *
 *       Filename:  world.cpp
 *
 *    Description:  Stores data about the map
 *
 *        Version:  1.0
 *        Created:  04/03/17 05:11:46 PM
 *       Revision:  none
 *       Compiler:  clang4
 *
 *         Author:  Jacque McGoats (MrMcGoats), 31115470+MrMcGoats@users.noreply.github.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <vector>
#include "world.hpp"

#include <iostream>

World::tile* World::find_tile_at(int x, int y)
{
	for(int i=0; i<this->map.size(); i++)
	{
		if(map.at(i).x==x && map.at(i).y==y)
			return &(this->map.at(i));
	}

	return new tile;
	return NULL; //TODO: Make all the fucntions that call this function check for NULL so the program doesn't crash
}

World::tile* World::find_tile_id(int id)
{
	if(this->map.size()>=id && id>=0)
		return &(this->map.at(id));

	return new tile;
	return NULL; //TODO: Make all the functions that call this function check for NULL so the program doesn't crash
}

World::World(int world_h, int world_w, int size, bool lit):
	tile_size(size), map_h(world_h), map_w(world_w), all_lit(lit)
{
	tile t;
	t.flag=__TILE_FLAG_EMPTY;
	t.is_lit=this->all_lit;
	t.type=0;
	t.x=0;
	t.y=0;
	for(int i=size; i<=(world_h*size); i+=size) //Make an empty map
	{
		for(int j=size; j<=(world_w*size); j+=size) //I have no fucking idea why j has to = size. As far as I can tell it should be =0. If you ask me, you can just go fuck yourself you overly curious piece of shit. -Love, Jacob
		{
			t.x=j-size;
			this->map.push_back(t);
		}
		t.y=i;
	}
}
