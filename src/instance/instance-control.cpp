#include "instance-control.hpp"

#include "system.hpp"

void handle_events(const SDL_Event& event)
{
	switch(event.type)
	{
		case SDL_MOUSEBUTTONDOWN:
		{
			for(size_t i = 0; i < instances.size(); i++)
			{
				if(instances[i]->collides(Ivec(event.motion.x, event.motion.y)))
				{
					instances[current_instance]->active = false;
					current_instance = i;
					instances[current_instance]->active = true;
				}
			}
		}
		break;
		case SDL_KEYDOWN:
		{
			switch(event.key.keysym.sym)
			{
				case SDLK_j:
				{
					if(Instance::state == COMMAND)
					{
						instances[current_instance]->active = false;
						current_instance--;
						if(current_instance == (size_t)-1)
						{
							current_instance = instances.size() - 1;
						}
						instances[current_instance]->active = true;
						Instance::state = NORMAL;
					}
				}
				break;
				case SDLK_k:
				{
					if(Instance::state == COMMAND)
					{
						instances[current_instance]->active = false;
						current_instance++;
						if(current_instance == instances.size())
						{
							current_instance = 0;
						}
						instances[current_instance]->active = true;
						Instance::state = NORMAL;
					}
				}
				break;
				case SDLK_SPACE:
				{
					const uint8_t* keys = SDL_GetKeyboardState(NULL);
					if(keys[SDL_SCANCODE_LCTRL])
					{
						Instance::state = COMMAND;
					}
				}
				break;
			}
		}
	}
}
