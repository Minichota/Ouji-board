#include "instance-control.hpp"

#include "file-selector.hpp"
#include "setting-editor.hpp"
#include "system.hpp"
#include "terminal.hpp"
#include "text-buffer.hpp"
#include "tty.hpp"

static tty_instance* tty = create_tty();

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
				case SDLK_c: //////// COMPILE
				{
					if(Instance::state == COMMAND)
					{
						set_command(tty, Settings::get_setting("compile"));
						TextBuffer* compile_buffer = new TextBuffer(
							Ivec(0, 0), SDL::window_size, 5, tty->out,
							SDL_Color{ 255, 255, 255, 255 },
							SDL_Color{ 255, 0, 255, 255 });
						push_instance(compile_buffer);
						Instance::state = NORMAL;
					}
				}
				break;
				case SDLK_f: //////// FILES
				{
					if(Instance::state == COMMAND)
					{
						FileSelector* file_selector =
							new FileSelector(Ivec(0, 0), SDL::window_size, 5,
											 SDL_Color{ 255, 255, 255, 255 });
						push_instance(file_selector);
						Instance::state = NORMAL;
					}
				}
				break;
				case SDLK_t: //////// TERMINAL
				{
					if(Instance::state == COMMAND)
					{
						Terminal* terminal =
							new Terminal(Ivec(0, 0), SDL::window_size, 5,
										 SDL_Color{ 255, 255, 255, 255 },
										 SDL_Color{ 255, 255, 255, 255 });
						push_instance(terminal);
						Instance::state = NORMAL;
					}
				}
				break;
				case SDLK_0: //////// SETTINGS
				{
					if(Instance::state == COMMAND)
					{
						SettingEditor* setting_editor =
							new SettingEditor(Ivec(0, 0), SDL::window_size, 5,
											  SDL_Color{ 255, 255, 255, 255 });
						push_instance(setting_editor);
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

void close_all_instances()
{
	for(Instance* instance : instances)
	{
		delete instance;
	}
	instances.clear();
	stop_tty(tty);
}

int find_instance(Instance* instance)
{
	for(size_t i = 0; i < instances.size(); i++)
	{
		if(instance == instances[i])
		{
			return i;
		}
	}
	return -1;
}

void switch_instance(Instance* prev, Instance* next)
{
	for(size_t i = 0; i < instances.size(); i++)
	{
		if(instances[i] == prev)
		{
			Ivec last_pos = instances[i]->get_pos();
			Ivec last_size = instances[i]->get_size();
			instances.erase(instances.begin() + i);
			instances.push_back(next);
			Ivec& pos = next->get_pos();
			Ivec& size = next->get_size();
			pos = last_pos;
			size = last_size;
			next->active = true;
		}
	}
}

void push_instance(Instance* instance)
{
	if(!instances.empty())
	{
		Ivec& last_pos = instances.back()->get_pos();
		Ivec& last_size = instances.back()->get_size();
		last_size = Ivec(last_size.x / 2, last_size.y);
		instances.back()->active = false;
		instance->get_size() = last_size;
		instance->get_pos() = Ivec(last_pos.x + last_size.x, last_pos.y);
		instance->handle_resize();
	}
	else
	{
		instance->get_size() = SDL::window_size;
	}
	instances.push_back(instance);
	instance->active = true;
	current_instance = instances.size() - 1;
}

void remove_instance(size_t index)
{
	if(instances.size() == 0)
	{
		SDL_Quit();
		TTF_Quit();
		exit(0);
	}
	// copy size
	Ivec removed_size = instances[index]->get_size();
	// removal
	delete instances[index];
	instances.erase(instances.begin() + index);
	if(instances.size() == 0)
		return;

	std::cout << index << std::endl;
	instances[index - 1]->active = true;
	current_instance = index - 1;
	Ivec& last_size = instances[index - 1]->get_size();
	last_size = Ivec(last_size.x + removed_size.x, last_size.y);
}
