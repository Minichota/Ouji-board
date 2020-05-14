#ifndef DEBUG_MANAGER_CPP
#define DEBUG_MANAGER_CPP

#include "debug-manager.hpp"
#include "system.hpp"

DebugManager::DebugManager() :
visible { false, false, false },
debug_positions { Ivec(100,100), Ivec(250,100), Ivec(400,100), Ivec(550,100) }
{
	this->active = TRACE;
	this->moving = SELECTION_COUNT;
}

DebugManager::~DebugManager()
{
}

void DebugManager::render()
{
	if(Instance::state == DEBUG)
	{
		if(visible[TRACE])
			SDL_SetRenderDrawColor(SDL::renderer, 0, 255, 0, 255);
		else
			SDL_SetRenderDrawColor(SDL::renderer, active == TRACE ? 0 : 255, 255, 255, 255);
		debug_sizes[TRACE] = render_traces(debug_positions[TRACE]);

		if(visible[TEMP])
			SDL_SetRenderDrawColor(SDL::renderer, 0, 255, 0, 255);
		else
			SDL_SetRenderDrawColor(SDL::renderer, active == TEMP ? 0 : 255, 255, 255, 255);
		debug_sizes[TEMP] = render_temps(debug_positions[TEMP]);

		if(visible[VIEW])
			SDL_SetRenderDrawColor(SDL::renderer, 0, 255, 0, 255);
		else
			SDL_SetRenderDrawColor(SDL::renderer, active == VIEW ? 0 : 255, 255, 255, 255);
		debug_sizes[VIEW] = render_views(debug_positions[VIEW]);

		if(visible[LOG])
			SDL_SetRenderDrawColor(SDL::renderer, 0, 255, 0, 255);
		else
			SDL_SetRenderDrawColor(SDL::renderer, active == LOG ? 0 : 255, 255, 255, 255);
		debug_sizes[LOG] = render_logs(debug_positions[LOG]);
	}
	else
	{
		SDL_SetRenderDrawColor(SDL::renderer, 255, 255, 255, 255);
		if(visible[TRACE]) render_traces(debug_positions[TRACE]);
		if(visible[TEMP])   render_temps(debug_positions[TEMP]);
		if(visible[VIEW])   render_views(debug_positions[VIEW]);
		if(visible[LOG])   render_logs(debug_positions[LOG]);
	}
}

void DebugManager::handle_event(const SDL_Event& event)
{
	switch(event.type)
	{
		case SDL_MOUSEBUTTONDOWN:
		{
			for(size_t i = 0; i < SELECTION_COUNT; i++)
			{
				SDL_Point mouse = { event.button.x, event.button.y };
				if(SDL_PointInRect(&mouse, &debug_sizes[i]))
				{
					moving = static_cast<Selection>(i);
					active = moving;
				}
			}
		}
		break;
		case SDL_MOUSEBUTTONUP:
		{
			moving = SELECTION_COUNT;
		}
		break;
		case SDL_MOUSEMOTION:
		{
			if(moving != SELECTION_COUNT)
				debug_positions[moving] += Ivec(event.motion.xrel, event.motion.yrel);
		}
		break;
		case SDL_KEYDOWN:
		{
			switch(event.key.keysym.sym)
			{
				case SDLK_KP_1:
				{
					active = TRACE;
				}
				break;
				case SDLK_KP_2:
				{
					active = TEMP;
				}
				break;
				case SDLK_KP_3:
				{
					active = VIEW;
				}
				break;
				case SDLK_KP_4:
				{
					active = LOG;
				}
				break;
				case SDLK_d:
				{
					if(Instance::state == COMMAND)
						Instance::state = DEBUG;
				}
				break;
				case SDLK_SPACE:
				{
					if(Instance::state == DEBUG)
					{
						switch(active)
						{
							case TRACE:
							{
								visible[TRACE] = !visible[TRACE];
							}
							break;
							case TEMP:
							{
								visible[TEMP] = !visible[TEMP];
							}
							break;
							case VIEW:
							{
								visible[VIEW] = !visible[VIEW];
							}
							break;
							case LOG:
							{
								visible[LOG] = !visible[LOG];
							}
							break;
							case SELECTION_COUNT:
							break;
						}
					}
				}
				break;
			}
		}
	}
	switch(active)
	{
		case TRACE:
		{
			handle_trace_event(event);
		}
		break;
		case TEMP:
		{
			handle_temp_event(event);
		}
		break;
		case VIEW:
		{
			handle_view_event(event);
		}
		break;
		case LOG:
		case SELECTION_COUNT:
		break;
	}
}

#endif
