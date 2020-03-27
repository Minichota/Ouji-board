#include "instance-control.hpp"

#include "system.hpp"
#include "terminal.hpp"
#include "text-buffer.hpp"
#include "tty.hpp"

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
						if(current_instance == 0)
						{
							current_instance = instances.size() - 1;
						}
						else
						{
							current_instance--;
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
				case SDLK_q:
				{
					if(Instance::state == COMMAND)
					{
						remove_instance(current_instance);
						Instance::state = NORMAL;
					}
				}
				break;
				case SDLK_c:
				{
					if(Instance::state == COMMAND)
					{
						Ivec& last_pos = instances.back()->get_pos();
						Ivec& last_size = instances.back()->get_size();
						set_command(Settings::get_setting("compile"));
						std::string out = wait_command();
						TextBuffer* compile_buffer = new TextBuffer(
							Ivec(last_pos.x + last_size.x / 2, last_pos.y),
							Ivec(last_size.x / 2, last_size.y), 5, "",
							SDL_Color{ 255, 255, 255, 255 },
							SDL_Color{ 255, 0, 255, 255 });
						compile_buffer->set_text(out);
						push_instance(compile_buffer);
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
				case SDLK_ESCAPE:
				{
					Instance::state = NORMAL;
				}
				break;
			}
		}
	}
}

void push_instance(Instance* instance)
{
	if(instances.size() > 0)
	{
		Ivec& last_pos = instances.back()->get_pos();
		Ivec& last_size = instances.back()->get_size();
		last_size = Ivec(last_size.x / 2, last_size.y);
	}
	instances.push_back(instance);
}

void remove_instance(size_t index)
{
	Ivec old_size = instances[index]->get_size();
	instances.erase(instances.begin() + index);
	Ivec& last_size = instances.back()->get_size();
	instances.back()->active = true;
	current_instance = instances.size() - 1;
	last_size = Ivec(last_size.x + old_size.x, last_size.y);
}
