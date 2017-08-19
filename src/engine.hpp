/*
 * =====================================================================================
 *
 *       Filename:  engine.cpp
 *
 *    Description:  Game engine, responsible for storing the map and characters (and abstacting everything)
 *
 *        Version:  1.0
 *        Created:  04/03/17 02:07:09 PM
 *       Revision:  none
 *       Compiler:  clang4
 *
 *         Author:  Jacque McGoats (MrMcGoats), 31115470+MrMcGoats@users.noreply.github.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef __ENGINE_HPP
#define __ENGINE_HPP

#include <stdlib.h>
#include <iostream>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <vector>
#include "world.hpp"
#include "characters.hpp"

enum game_state_t
{
	menu, playing, paused
};

class Engine
{

	struct tile_struct
	{
		std::string name;
		ALLEGRO_BITMAP *image;
	};

	struct npc_struct
	{
		Character* c;
		int hitbox; //Hitbox radius, set to 0 for no collision
		int hitbox_char; //Hitbox for colliding with the player
		ALLEGRO_BITMAP *image;
		int num_anim; //Number of animations (this should be at least 4 for any npc that has a walking animation, and at least 5 for any npc with both a walking and death animation)
		int len_anim; //Number of frames in each animation
		int height_anim; //The height of each frame in pixels
		int width_anim; //The width of each frame in pixels

		int current_frame; //The current frame of the animation
		bool locked; //When true, the normal animation won't play
	};

	ALLEGRO_DISPLAY *display;
	ALLEGRO_EVENT_QUEUE *event_queue;

	int default_bg_blue;
	int default_bg_red;
	int default_bg_green;

	int screen_w;
	int screen_h;

	World* map;
	std::vector<tile_struct> tile_types;

	std::vector<npc_struct> npc; //Stores both NPCs and PCs (sorry)

	bool is_running;

	game_state_t state;

	int center_character=-1; //-1 means scroll the map, any other numeber is the ID of the character that the map is to be centered on

	std::vector<ALLEGRO_BITMAP*> menus;

	void initialize(int,int,int,int,int,bool=true);

	public:
		Engine(int h, int w, int bg_red, int bg_green, int bg_blue, bool mouse=true): screen_w(w), screen_h(h), default_bg_red(bg_red), default_bg_green(bg_green), default_bg_blue(bg_blue)
		{ initialize(h,w,bg_red,bg_green,bg_blue,mouse); }
		Engine(int, int, int, bool=true);
		~Engine();
		bool is_game_running() { return this->is_running; }
		void end_game() { this->is_running=false; }

		/* Starts two different loops, one for event handling, and one for drawing in two seprerate threads
		 * @arg Function to handle drawing
		 * @arg Function to handle input
		 * Note: Neither of these functions may have loops that run for the entire game, this is handled by the engine. Pretend everything in the function is in a while(true) loop, since it basically will be
		 */
		void game_loop(void (*)(Engine*), void (*)(Engine*,ALLEGRO_EVENT*));

		//Map related functions
		void make_new_map(World* w=new World()) { this->map=w; }
		void draw_map();
		ALLEGRO_EVENT_QUEUE* get_event_queue() { return (this->event_queue); }
		void add_tile(std::string, std::string);
		void print_map(); //Prints the map to stdout. Use only for testing

		//Functions to get info about or modify tiles
		bool is_tile_lit(int x, int y) { return this->map->is_tile_lit(x,y); }
		bool is_tile_lit(int id) { return this->map->is_tile_lit(id); }
		void light_tile(int id, bool light=true) { this->map->light_tile(id,light); }
		int get_tile_flag(int x, int y) { return this->map->get_tile_flag(x,y); }
		int get_tile_flag(int id) { return this->map->get_tile_flag(id); }
		int get_tile_type(int x, int y) { return this->map->get_tile_type(x,y); }
		int get_tile_type(int id) { return this->map->get_tile_type(id); }
		void set_tile_type(int x, int y, int type) { this->map->set_tile_type(x,y,type); }
		void set_tile_type(int id, int type) { this->map->set_tile_type(id,type); }
		int get_tile_size() { return this->map->get_tile_size(); }
		int get_num_tiles() { return this->map->get_num_tiles(); }
		int get_num_tiles_x() { return this->map->get_num_tiles_x(); }
		int get_num_tiles_y() { return this->map->get_num_tiles_y(); }
		int get_tile_x(int id) { return this->map->get_tile_x(id); }
		int get_tile_y(int id) { return this->map->get_tile_y(id); }
		bool tile_is_drawn(int);

		//Functions for characters
		int add_character(Character*,std::string,int,int,int,int);
		int get_char_x(int i) { return this->npc.at(i).c->get_x(); }
		int get_char_y(int i) { return this->npc.at(i).c->get_y(); }
		int get_char_hitbox(int i) { return this->npc.at(i).hitbox; }
		face_t get_char_face(int i) { return this->npc.at(i).c->get_face(); }
		void set_char_x(int i, int x) { this->npc.at(i).c->set_x(x);
			this->npc.at(i).c->on_move(); }
		void set_char_y(int i, int y) { this->npc.at(i).c->set_y(y); 
			this->npc.at(i).c->on_move(); }
		void center_map(int i) { this->center_character=i; 
		this->npc.at(i).c->set_x(-this->npc.at(i).c->get_x());
		this->npc.at(i).c->set_y(-this->npc.at(i).c->get_y()); } //i is the character id or -1 if you want the map to scroll
		void set_char_hitbox(int i, int hb) { this->npc.at(i).hitbox=hb;
			this->npc.at(i).hitbox_char=hb; }
		void set_char_face(int i, face_t dir) { this->npc.at(i).c->set_face(dir); }
		bool npc_is_drawn(int);
		void ignore_npc(int id) { this->npc.at(id).c->ignore(true); }
		bool npc_is_ignored(int id) { return this->npc.at(id).c->ignore(); }
		bool npc_is_alive(int id) { return this->npc.at(id).c->is_alive(); }
		int npc_heal(int id, int h) { return this->npc.at(id).c->heal(h); }
		void npc_harm(int id, int h) { return this->npc.at(id).c->harm(h); }

		//Load npc spawns from the map
		void load_character_spawns(int, int=-1);
		int get_num_npcs() { return this->npc.size()-1; } //Subtract 1 to remove the player
	
		//Functions to check for collisions	
		bool check_collision_wall(int); //Checks if char with the id has collided with a wall
		bool check_collision_char(int,int); //Checks if one character is colliding with another
		bool check_collision_misc_tile(int,int); //checks if one character is colliding with a specific type of tile

		//Move character, then check for collisions with walls (uses face_t  enum for direction) NOTE: on collisions, the character is not moved
		bool move_char(int,int,face_t,bool=true,bool=true);
		void animate_char(int);
		bool animate_char(int, int); //Run a specific animation. Returns true when the animation finishes

		//Add menu screens
		int add_screen(std::string);
		void draw_screen(int);

		//Game states
		void set_game_state(game_state_t g) { this->state=g; }
		game_state_t get_game_state() { return this->state; }
};

#endif //__ENGINE_HPP
