/*
 * =====================================================================================
 *
 *       Filename:  characters.cpp
 *
 *    Description:  A base class for characters, meant to be extended
 *
 *        Version:  1.0
 *        Created:  04/03/17 02:48:40 PM
 *       Revision:  none
 *       Compiler:  clang4
 *
 *         Author:  Jacque McGoats (MrMcGoats), 31115470+MrMcGoats@users.noreply.github.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef __CHARACTERS_HPP
#define __CHARACTERS_HPP

enum face_t
{
	forward_dir, right_dir, backward_dir, left_dir
};

class Character
{
	int loc_x;
	int loc_y;
	int health;
	int max_health;
	int dmg;
	int id_from_engine;
	face_t facing;
	bool ignore_var;
	public:
		Character(int,int,int,int,int);
		int heal(int h) { this->health+=h;
			if(this->health>max_health)
				this->health=max_health;
			return this->health;
		}
		void harm(int h) { this->health-=h; }
		bool is_alive() { return (this->health>0); }
		bool deal_dmg_to(Character* c) { c->harm(this->dmg);
			return c->is_alive(); }
		int get_x() { return this->loc_x; }
		int get_y() { return this->loc_y; }
		void set_x(int x) { this->loc_x=x; }
		void set_y(int y) { this->loc_y=y; }
		void set_face(face_t dir) { this->facing=dir; }
		face_t get_face() { return this->facing; }
		void set_engine_id(int i) { this->id_from_engine=i; }
		int get_id() { return this->id_from_engine; }
		bool ignore(bool=false);

		virtual void on_move() {} //Gets called by the engine whenever the character moves. Meant to be overriden
		
};

#endif //__CHARACTERS_HPP
