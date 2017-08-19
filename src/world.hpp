/*
 * =====================================================================================
 *
 *       Filename:  world.hpp
 *
 *    Description:  Stores data about the map
 *
 *        Version:  1.0
 *        Created:  04/03/17 04:58:49 PM
 *       Revision:  none
 *       Compiler:  clang4
 *
 *         Author:  Jacque McGoats (MrMcGoats), 31115470+MrMcGoats@users.noreply.github.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef __WORLD_HPP
#define __WORLD_HPP

#include <stdlib.h>
#include <vector>
#include <array>
#include <iostream>

#include "characters.hpp"

//Tile flags (not using an enum so that any int is a valid flag, but the engine will handle any flags defined here, the rest will be handeled manually
#define __TILE_FLAG_EMPTY 0       //Tile has nothing on it
#define __TILE_FLAG_HAS_PLAYER 1  //Tile has the player on it
#define __TILE_FLAG_HAS_NPC 2     //Tile has an NPC on it
#define __TILE_FLAG_HAS_BOTH 3    //Tile has both a player and an NPC on it (the two collided)

class World
{
	struct tile
	{
		int x; //x coord of the top right of the tile
		int y; //y coord of the top right of the tile
		int flag;
		int type; //The type of tile (tells engine what to draw for this tile, 0 means do not draw the tile, 1 means draw a black square, 2 means draw a white square, and every other number above 2 can mean anything else)
		bool is_lit; //true if the tile is illuminated
	};

	std::vector<tile> map;

	int tile_size;
	int map_h;
	int map_w;
	bool all_lit;
	
	World::tile* find_tile_at(int, int); //find a tile at the given point
	World::tile* find_tile_id(int); //find tile by position in array

	public:
		World(int=10,int=10,int=32,bool=false);
		bool is_tile_lit(int x, int y) { return find_tile_at(x,y)->is_lit; }
		bool is_tile_lit(int id) { return find_tile_id(id)->is_lit; }
		void light_tile(int id, bool light=true) { find_tile_id(id)->is_lit=light; }
		int get_tile_flag(int x, int y) { return find_tile_at(x,y)->flag; }
		int get_tile_flag(int id) { return find_tile_id(id)->flag; }
		void set_tile_flag(int id, int flag) { find_tile_id(id)->flag=flag; }
		int get_tile_type(int x, int y) { return find_tile_at(x,y)->type; }
		int get_tile_type(int id) { return find_tile_id(id)->type; }
		void set_tile_type(int x, int y, int type) { find_tile_at(x,y)->type=type; }
		void set_tile_type(int id, int type) { find_tile_id(id)->type=type; }
		int get_tile_size() { return this->tile_size; }
		int get_num_tiles() { return this->map.size(); }
		int get_num_tiles_x() { return this->map_w; }
		int get_num_tiles_y() { return this->map_h; }
		int get_tile_x(int id) { return find_tile_id(id)->x; }
		int get_tile_y(int id) { return find_tile_id(id)->y; }
		virtual std::vector<int> get_walkable_types() { return std::vector<int>(); } //Should be redefined in a child class, returns list of tile types that are walkable
		virtual std::vector<int> get_unwalkable_types() { return std::vector<int>(); } //Should be redefined in a child class, returns list of tile types that are not walkable
		virtual std::vector<Character*> get_characters() { return std::vector<Character*>(); } //Should be redefined in a child class, returns list of npcs to be spawned
		virtual std::vector<std::string> get_character_sprites() { return std::vector<std::string>(); }
		virtual std::vector<std::array<int,2>> get_character_dim() { return std::vector<std::array<int,2>>(); }
		virtual std::vector<std::array<int,2>> get_character_anim() { return std::vector<std::array<int,2>>(); }

};

#endif //__WORLD_HPP
