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
 *       Compiler:  gcc
 *
 *         Author:  Jacque McGoats (MrMcGoats), 31115470+MrMcGoats@users.noreply.github.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <array>
#include <allegro5/allegro.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include "engine.hpp"
#include "world.hpp"
#include "characters.hpp"

void Engine::initialize(int h, int w, int bg_red, int bg_green, int bg_blue, bool mouse)
{
	//Init window
	this->display=NULL;
	this->event_queue=NULL;

	if(!al_init())
	{
		al_show_native_message_box(NULL, NULL, NULL, "Failed to initialize allegro", NULL, NULL);
		throw;
	}

	this->display=al_create_display(this->screen_h, this->screen_w);

	if(!display)
	{
		al_show_native_message_box(NULL, NULL, NULL, "Failed to initializ display", NULL, NULL);
		throw;
	}

	if(!al_init_image_addon())
	{
		al_show_native_message_box(display, "Error", "Error", "Failed to initialize al_init_image_addon!", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		return;
	}

	al_init_primitives_addon();

	al_clear_to_color(al_map_rgb(bg_red, bg_green, bg_blue));
	al_flip_display();

	//Init input
	al_install_keyboard();
	al_install_mouse();

	this->event_queue=al_create_event_queue();

	al_register_event_source(event_queue, al_get_keyboard_event_source());
	if(mouse) al_register_event_source(event_queue, al_get_mouse_event_source());
	al_register_event_source(event_queue, al_get_display_event_source(this->display));

	this->is_running=true;
}

Engine::Engine(int bg_red, int bg_green, int bg_blue, bool mouse): default_bg_red(bg_red), default_bg_green(bg_green), default_bg_blue(bg_blue)
{
	ALLEGRO_DISPLAY_MODE disp_data;

	al_get_display_mode(al_get_num_display_modes() - 1, &disp_data);
	al_set_new_display_flags(ALLEGRO_FULLSCREEN);
	this->screen_h=disp_data.height;
	this->screen_w=disp_data.width;


	this->initialize(disp_data.height,disp_data.width,bg_red,bg_green,bg_blue,mouse);
}

Engine::~Engine()
{
	delete map;
	al_destroy_event_queue(event_queue);
	al_destroy_display(this->display);
}

bool Engine::npc_is_drawn(int id)
{
	int x_mod=0;
	int y_mod=0;

	int tile_size=this->map->get_tile_size();

	if(this->center_character>-1)
	{
		x_mod=(this->npc.at(this->center_character).c->get_x()+screen_w/2+tile_size);
		y_mod=(this->npc.at(this->center_character).c->get_y()+screen_h/2-(4*tile_size));
	}


	int npc_x=this->npc.at(id).c->get_x()+x_mod;
	int npc_y=this->npc.at(id).c->get_y()+y_mod;
	if(npc_x>(this->screen_w+((this->screen_h/60)*tile_size)) && this->center_character!=id) return false;
	if(npc_y>(this->screen_h+((this->screen_w/112.5)*tile_size)) && this->center_character!=id) return false;
	if(npc_x<-((this->screen_w/150)*tile_size) && this->center_character!=id) return false;
	if(npc_y<-((this->screen_h/200)*tile_size) && this->center_character!=id) return false;

	return true;
}

void Engine::draw_map()
{
	if(this->map==NULL)
		return;

	int x_mod=0;
	int y_mod=0;
	int tile_size=this->map->get_tile_size();
	
	if(this->center_character>-1)
	{
		x_mod=(this->npc.at(this->center_character).c->get_x()+screen_w/2+tile_size);
		y_mod=(this->npc.at(this->center_character).c->get_y()+screen_h/2-(4*tile_size));
	}

	int x=0;
	int y=0;

	//Clear screen
	al_flip_display();
	al_clear_to_color(al_map_rgb(this->default_bg_red, this->default_bg_green, this->default_bg_blue));


	for(int i=0; i<this->map->get_num_tiles(); i++)
	{
		x=this->map->get_tile_x(i)+x_mod;
		y=this->map->get_tile_y(i)+y_mod;
		if(x<-((this->screen_h/200)*this->map->get_tile_size())) continue;
		if(y<-((this->screen_w/150)*this->map->get_tile_size())) continue;
		if(x>(this->screen_w+(10*this->map->get_tile_size()))) continue;
		if(y>(this->screen_h+(4*this->map->get_tile_size()))) continue;
		switch(this->map->get_tile_type(i))
		{
			case 0: //Draw nothing
				break;
			case 1: //Draw a white square
				al_draw_filled_rectangle(x, y, x+tile_size, y+tile_size, al_map_rgb(0,0,0));
				break;
			case 2: //Draw a black square
				al_draw_filled_rectangle(x, y, x+tile_size, y+tile_size, al_map_rgb(255,255,255));
				break;
			default: //Draw a picture
				int this_tile_type=0;
				if(this->tile_types.size()==0) break;
				al_draw_bitmap(this->tile_types.at(this->map->get_tile_type(i)-3).image, x, y,0);
				break;
		}
	}

	//Draw the characters
	for(int i=0; i<this->npc.size(); i++)
	{
		int npc_x=this->npc.at(i).c->get_x()+x_mod;
		int npc_y=this->npc.at(i).c->get_y()+y_mod;

		if(!this->npc_is_drawn(i) || this->npc_is_ignored(i)) continue;

		//Calculate where the frame is on the spritesheet
		int frame_x=this->npc.at(i).current_frame*this->npc.at(i).width_anim;
		int frame_y=0; //I'm going to just assume that the spritesheet only has one row, since it's way easier to calculate where to draw from

		if(this->center_character!=i)
			al_draw_bitmap_region(this->npc.at(i).image, frame_x, frame_y, this->npc.at(i).width_anim, this->npc.at(i).height_anim, this->npc.at(i).c->get_x()+x_mod, this->npc.at(i).c->get_y()+y_mod,0);
		else
			al_draw_bitmap_region(this->npc.at(i).image, frame_x, frame_y, this->npc.at(i).width_anim, this->npc.at(i).height_anim, (this->screen_w/2)+(tile_size), (this->screen_h/2)-(4*tile_size),0);
	}
	
	al_flip_display();
}

void Engine::add_tile(std::string name, std::string img_path)
{
	ALLEGRO_PATH *path=al_get_standard_path(ALLEGRO_RESOURCES_PATH);

	al_set_path_filename(path, img_path.c_str());


	tile_struct t;
	t.name=name;
	t.image=al_load_bitmap(al_path_cstr(path, '/'));
	this->tile_types.push_back(t);
}

void Engine::print_map()
{
	if(this->map==NULL)
		return;

	int id=0;
	
	for(int i=0; i<this->map->get_num_tiles_y(); i++)
	{
		for(int j=0; j<this->map->get_num_tiles_x(); j++)
		{
			std::cout<<this->map->get_tile_type(id)<<" ";
			if((id+1)<this->map->get_num_tiles()) id++;
		}
		std::cout<<"\n";
	}
}

bool Engine::tile_is_drawn(int i)
{
	int x_mod=0;
	int y_mod=0;

	if(this->center_character>-1)
	{
		x_mod=(this->npc.at(this->center_character).c->get_x()+screen_w/2+this->map->get_tile_size());
		y_mod=(this->npc.at(this->center_character).c->get_y()+screen_h/2-(4*this->map->get_tile_size()));
	}


	int x=this->map->get_tile_x(i)+x_mod;
	int y=this->map->get_tile_y(i)+y_mod;
	if(x<-((this->screen_h/200)*this->map->get_tile_size())) return false;
	if(y<-((this->screen_w/150)*this->map->get_tile_size())) return false;
	if(x>(this->screen_w+(10*this->map->get_tile_size()))) return false;
	if(y>(this->screen_h+(4*this->map->get_tile_size()))) return false;

	return true;
}

int Engine::add_character(Character* c, std::string img_path, int height, int width, int num_anim, int len_anim)
{
	ALLEGRO_BITMAP *sprite;
	ALLEGRO_PATH *path=al_get_standard_path(ALLEGRO_RESOURCES_PATH);

	al_set_path_filename(path, img_path.c_str());
	//std::cout<<"Loading sprite at "<<img_path<<"\npath="<<al_path_cstr(path, '/')<<"\n";

	sprite=al_load_bitmap(al_path_cstr(path, '/'));

	if(!sprite)
	{
		std::cout<<"Not loaded\n";
		return NULL;
	}

	npc_struct d;
	d.c=c;
	d.image=sprite;
	d.num_anim=num_anim;
	d.len_anim=len_anim;
	d.height_anim=height;
	d.width_anim=width;

	if(map!=NULL)
		d.hitbox=this->map->get_tile_size();
	else
		d.hitbox=0;
	d.hitbox_char=d.hitbox;
	
	this->npc.push_back(d);
	this->npc.at(this->npc.size()-1).c->set_engine_id(this->npc.size()-1);
	return npc.size()-1; //The index at which to find the new character
}

void Engine::load_character_spawns(int hitbox, int hitbox_char)
{
	if(hitbox_char==-1) hitbox_char=hitbox;

	std::vector<Character*> chars=this->map->get_characters();
	std::vector<std::string> img_path=this->map->get_character_sprites();
	std::vector<std::array<int,2>> dimension=this->map->get_character_dim();
	std::vector<std::array<int,2>> animation=this->map->get_character_anim();

	ALLEGRO_PATH *path=al_get_standard_path(ALLEGRO_RESOURCES_PATH);

	npc_struct c;
	c.hitbox=hitbox;
	c.hitbox_char=hitbox_char;

	for(int i=0; i<chars.size(); i++)
	{
		ALLEGRO_BITMAP *sprite;

		//std::cout<<"Loading sprite at "<<img_path.at(i)<<"\n";

		al_set_path_filename(path, img_path.at(i).c_str());
		sprite=al_load_bitmap(al_path_cstr(path, '/'));

		if(!sprite)
		{
			std::cout<<"Not loaded!\n";
			return;
		}

		c.c=chars.at(i);
		c.image=sprite;
		c.num_anim=animation.at(i)[0];
		c.len_anim=animation.at(i)[1];
		c.height_anim=dimension.at(i)[0];
		c.width_anim=dimension.at(i)[1];
		this->npc.push_back(c);

		c.c->set_engine_id(this->npc.size()-1);
	}
}

bool Engine::check_collision_wall(int id_c)
{
	int x=this->npc.at(id_c).c->get_x();
	int y=this->npc.at(id_c).c->get_y();
	int hitbox=this->npc.at(id_c).hitbox;
	int tile_size=this->map->get_tile_size();

	if(this->center_character==id_c)
	{
		x=-x;
		y=-y;
	}

	if(hitbox==0)
		return false;

	//Make sure the character is actually in the map
	if((x-hitbox+tile_size)<0 ||
		(x+hitbox)>(this->map->get_num_tiles_x()*tile_size) ||
		(y-hitbox+tile_size)<0 ||
		(y+hitbox)>(this->map->get_num_tiles_y()*tile_size)
	  )
		return true;

	//std::cout<<"x_left: "<<x<<"\ty_top: "<<y<<"\tx_right: "<<x+hitbox<<"\ty_bot: "<<y+hitbox<<"\n";

	//Check to see if the character is trying to walk on an unwalkable block
	for(int i=0; i<this->map->get_num_tiles(); i++)
	{
		for(int j=0; j<this->map->get_unwalkable_types().size(); j++)
		{
			if(this->map->get_tile_type(i)==this->map->get_unwalkable_types().at(j)) //Check to see if the tile is unwalkable
			{
				int tile_x=this->map->get_tile_x(i);
				int tile_y=this->map->get_tile_y(i);

				//Check for collision
				if( ((x-hitbox+tile_size) >= tile_x && (x-hitbox+tile_size) <= (tile_x + tile_size)) ||
						((x + hitbox) >= tile_x && (x + hitbox) <= (tile_x + tile_size)) )
				{
					if( ((y-hitbox+tile_size) >= tile_y && (y-hitbox+tile_size) <= (tile_y + tile_size)) ||
							((y + hitbox) >= tile_y && (y + hitbox) <= (tile_y + tile_size)) )
						return true;
				}
			}
		}
	}

	return false;
}

bool Engine::check_collision_char(int id_c1, int id_c2)
{
	int x1=this->npc.at(id_c1).c->get_x();
	int y1=this->npc.at(id_c1).c->get_y();
	int hitbox1=this->npc.at(id_c1).hitbox;

	int x2=this->npc.at(id_c2).c->get_x();
	int y2=this->npc.at(id_c2).c->get_y();
	int hitbox2=this->npc.at(id_c2).hitbox;

	int tile_size=this->map->get_tile_size();

	if(hitbox1==0 || hitbox2==0)
		return false;


	if(this->center_character==id_c1)
	{
		x1=-x1;
		y1=-y1;
		hitbox2=this->npc.at(id_c2).hitbox_char;
	}
	else if(this->center_character==id_c2)
	{
		x2=-x2;
		y2=-y2;
		hitbox1=this->npc.at(id_c1).hitbox_char;
	}
	
	//std::cout<<"Hitbox1="<<hitbox1<<"\thitbox2="<<hitbox2<<"\n";
	
	//Check for collision
	if( (abs(x1-hitbox1+tile_size) >= abs(x2-hitbox2) && abs(x1-hitbox1+tile_size) <= (x2 + abs(tile_size-hitbox2))) ||
			((x1 + hitbox1) >= (x2+hitbox2) && (x1 + hitbox1) <= (x2 +hitbox2+ tile_size)) )
	{
		if( (abs(y1-hitbox1+tile_size) >= abs(y2-hitbox2) && abs(y1-hitbox1+tile_size) <= (y2+abs(tile_size-hitbox2))) ||
				((y1 + hitbox1) >= (y2+hitbox2) && (y1 + hitbox1) <= (y2+hitbox2 + tile_size)) )
			return true;
	}


	return false;
}

bool Engine::check_collision_misc_tile(int id_c, int tile)
{
	int x=this->npc.at(id_c).c->get_x();
	int y=this->npc.at(id_c).c->get_y();
	int hitbox=this->npc.at(id_c).hitbox;
	int tile_size=this->map->get_tile_size();

	if(this->center_character==id_c)
	{
		x=-x;
		y=-y;
	}

	if(hitbox==0)
		return false;

	//Make sure the character is actually in the map
	if((x-hitbox+tile_size)<0 ||
		(x+hitbox)>(this->map->get_num_tiles_x()*tile_size) ||
		(y-hitbox+tile_size)<0 ||
		(y+hitbox)>(this->map->get_num_tiles_y()*tile_size)
	  )
		return true;

	//Check to see if the character is trying to walk on an unwalkable block
	for(int i=0; i<this->map->get_num_tiles(); i++)
	{
		if(this->map->get_tile_type(i)==tile) //Check to see if the tile is unwalkable
		{
			int tile_x=this->map->get_tile_x(i);
			int tile_y=this->map->get_tile_y(i);

			//Check for collision
			if( ((x-hitbox+tile_size) >= tile_x && (x-hitbox+tile_size) <= (tile_x + tile_size)) ||
					((x + hitbox) >= tile_x && (x + hitbox) <= (tile_x + tile_size)) )
			{
				if( ((y-hitbox+tile_size) >= tile_y && (y-hitbox+tile_size) <= (tile_y + tile_size)) ||
						((y + hitbox) >= tile_y && (y + hitbox) <= (tile_y + tile_size)) )
					return true;
			}
		}
	}

	return false;

}

bool Engine::move_char(int id, int units, face_t dir, bool check_collision, bool animate)
{
	if(this->map==NULL)
		return false;

	if(this->npc_is_ignored(id))
		return false;

	int movement;
	int orig;

	face_t dir_adjusted;

	if(this->center_character==id)
	{
		if(dir==right_dir)
			dir_adjusted=left_dir;
		else if(dir==left_dir)
			dir_adjusted=right_dir;
		else dir_adjusted=dir;
	}
	else dir_adjusted=dir;

	switch(dir_adjusted)
	{
		case forward_dir:
			movement=this->get_char_y(id)+units;
			break;
		case right_dir:
			movement=this->get_char_x(id)+units;
			break;
		case backward_dir:
			movement=this->get_char_y(id)-units;
			break;
		case left_dir:
			movement=this->get_char_x(id)-units;
			break;
	}

	if(dir==forward_dir || dir==backward_dir)
	{
		orig=this->get_char_y(id);
		this->set_char_y(id,movement);
		if(this->check_collision_wall(id) && check_collision)
		{
			this->set_char_y(id,orig);
			return true;
		}
	}
	else if(dir==left_dir || dir==right_dir)
	{
		orig=this->get_char_x(id);
		this->set_char_x(id,movement);
		if(this->check_collision_wall(id) && check_collision)
		{
			this->set_char_x(id,orig);
			return true;
		}
	}
	
	if(animate) this->animate_char(id);	

	this->npc.at(id).c->on_move();

	return false;
}

void Engine::animate_char(int id)
{

	if(this->npc.at(id).locked) return;

	face_t dir=this->npc.at(id).c->get_face();
	face_t dir_adjusted;

	if(this->center_character==id)
	{
		if(dir==backward_dir)
			dir_adjusted=forward_dir;
		else if(dir==forward_dir)
			dir_adjusted=backward_dir;
		else dir_adjusted=dir;
	}
	else dir_adjusted=dir;

	//We assume that the sprite sheet has only one row and the first frame starts at x=0
	//    Walk forward
	//    walk left
	//    walk backward
	//    walk right
	//    death (optional)
	int start_frame=0; //The starting frame for the animation to walk in whatever direction the npc is currently facing
	int end_frame=0; //The last frame of the animation

	//std::cout<<"Guard id="<<id<<"\t";
	switch(dir_adjusted)
	{
		case forward_dir:
			start_frame=0;
			end_frame=this->npc.at(id).len_anim-2;
			//std::cout<<"Forward faceing\tstart_frame="<<start_frame<<"\tend_frame="<<end_frame<<"\n";
			break;
		case left_dir:
			start_frame=this->npc.at(id).len_anim;
			end_frame=(this->npc.at(id).len_anim*2)-2;
			//std::cout<<"left faceing\tstart_frame="<<start_frame<<"\tend_frame="<<end_frame<<"\n";
			break;
		case backward_dir:
			start_frame=(this->npc.at(id).len_anim*2);
			end_frame=(this->npc.at(id).len_anim*3)-2;
			//std::cout<<"back faceing\tstart_frame="<<start_frame<<"\tend_frame="<<end_frame<<"\n";
			break;
		case right_dir:
			start_frame=(this->npc.at(id).len_anim*3);
			end_frame=(this->npc.at(id).len_anim*4)-2;
			//std::cout<<"rignt faceing\tstart_frame="<<start_frame<<"\tend_frame="<<end_frame<<"\n";
			break;
	}
	if(this->npc.at(id).current_frame>end_frame)
		this->npc.at(id).current_frame=start_frame;
	else if(this->npc.at(id).current_frame<start_frame)
		this->npc.at(id).current_frame=start_frame;
	else
		this->npc.at(id).current_frame++;
}

bool Engine::animate_char(int id, int anim)
{

	if(anim>this->npc.at(id).num_anim) 
	{
		std::cerr<<"Animation "<<anim<<" does not exsist for character with ID="<<id<<"\n";
		return true; //Don't run animations that don't exsist
	}
	this->npc.at(id).locked=true; //Prevent any other animation from playing
	
	int start_frame=anim*this->npc.at(id).len_anim; //The starting frame for the animation to walk in whatever direction the npc is currently facing
	int end_frame=((anim+1)*this->npc.at(id).len_anim)-2; //The last frame of the animation

	if(this->npc.at(id).current_frame>end_frame || this->npc.at(id).current_frame<start_frame)
		this->npc.at(id).current_frame=start_frame;
	else if(this->npc.at(id).current_frame>=end_frame)
	{
		this->npc.at(id).locked=false;
		return true;
	}
	else
		this->npc.at(id).current_frame++;

	return false;
}

int Engine::add_screen(std::string img)
{
	ALLEGRO_BITMAP *sprite;
	ALLEGRO_PATH *path=al_get_standard_path(ALLEGRO_RESOURCES_PATH);

	al_set_path_filename(path, img.c_str());
	//std::cout<<"Loading sprite at "<<img_path<<"\npath="<<al_path_cstr(path, '/')<<"\n";

	sprite=al_load_bitmap(al_path_cstr(path, '/'));

	if(!sprite)
	{
		std::cout<<"Not loaded\n";
		return NULL;
	}

	this->menus.push_back(sprite);
	return menus.size()-1; //The index at which to find the new character
}

void Engine::draw_screen(int i)
{
	al_clear_to_color(al_map_rgb(this->default_bg_red, this->default_bg_green, this->default_bg_blue));
	al_draw_bitmap(this->menus.at(i),0,0,0);
	al_flip_display();
}
