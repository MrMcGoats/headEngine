/*
 * =====================================================================================
 *
 *       Filename:  game_loop.cpp
 *
 *    Description:  Handles the game loop
 *
 *        Version:  1.0
 *        Created:  05/03/17 09:54:46 AM
 *       Revision:  none
 *       Compiler:  clang4
 *
 *         Author:  Jacque McGoats (MrMcGoats), 31115470+MrMcGoats@users.noreply.github.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <allegro5/allegro.h>
#include <thread>
#include "engine.hpp"

#include <iostream>

namespace
{

	Engine *engine;
	void (*draw)(Engine*);
	void (*event)(Engine*,ALLEGRO_EVENT*);

	void draw_loop()
	{
		while(engine->is_game_running())
		{
			(draw)(engine);
		}

	}

	void event_loop()
	{

		ALLEGRO_EVENT ev;

		while(engine->is_game_running())
		{
			//std::cout<<"looping events\n";
		 	///al_wait_for_event(event_queue, &ev);

			(event)(engine,&ev);
			//std::cout<<"Events looped\n";
		}

	}

}

void Engine::game_loop(void (*draw_func)(Engine*), void (*event_func)(Engine*,ALLEGRO_EVENT*))
{
	engine=this;
	draw=draw_func;
	event=event_func;
	
	//std::thread draw_thread(draw_loop);
	std::thread event_thread(event_loop);

	draw_loop();
	//while(this->is_running) {} //Wait for the game to stop running

	event_thread.join();
	//draw_thread.join();
}
