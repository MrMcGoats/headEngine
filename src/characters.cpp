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
#include <stdlib.h>
#include "characters.hpp"

Character::Character(int x, int y, int h, int max_h, int d):
	loc_x(x), loc_y(y), health(h), max_health(max_h), dmg(d), ignore_var(false)
{}

bool Character::ignore(bool i)
{
	if(i)
		this->ignore_var=true;

	return ignore_var;
}
