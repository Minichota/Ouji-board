#include "instance-control.hpp"

#include <algorithm>

#include "file-selector.hpp"
#include "setting-editor.hpp"
#include "system.hpp"
#include "terminal.hpp"
#include "text-buffer.hpp"
#include "tty.hpp"

static tty_instance* tty = create_tty();
static TextBuffer* compile_address = 0;
static bool compile_open = false;

void handle_events(const SDL_Event& event)
{
	const uint8_t* keys = SDL_GetKeyboardState(NULL);
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
					else if(keys[SDL_SCANCODE_LCTRL])
					{
						SDL_Quit();
						TTF_Quit();
						exit(0);
					}
				}
				break;
				case SDLK_c: //////// COMPILE
				{
					if(Instance::state == COMMAND)
					{
						// make sure task is stopped
						if(tty->active)
							stop_tty(tty);
						start_tty(tty);

						set_command(tty, Settings::get_setting("compile").value);
						int previous = current_instance;
						if(!compile_open)
						{
							compile_address = new TextBuffer(
								Ivec(0, 0), SDL::window_size, 5, tty->out,
								SDL_Color{ 255, 255, 255, 255 },
								SDL_Color{ 255, 0, 255, 255 });
							push_instance(compile_address);
						}
						// assures that compile isn't selected after cmd+c
						current_instance = find_instance(instances[previous]);
						instances[previous]->handle_resize();
						instances[previous]->active = true;
						compile_address->active = false;
						// clear to remove potential previous text
						compile_address->clear();
						compile_open = true;
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
						// TODO: Not sure about this one
						if(Instance::state != DEBUG)
						{
							Instance::state = COMMAND;
						}
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
	if(tty->active)
	{
		stop_tty(tty);
	}
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
			delete instances[i];
			instances[i] = next;
			next->set_pos(last_pos);
			next->set_size(last_size);
			next->active = true;
			next->handle_resize();
			current_instance = i;
		}
	}
}

void push_instance(Instance* instance)
{
	if(!instances.empty())
	{
		Ivec& last_pos = instances.back()->get_pos();
		Ivec& last_size = instances.back()->get_size();
		instances.back()->set_size(Ivec(last_size.x / 2, last_size.y));
		instances[current_instance]->active = false;
		instance->set_size(last_size);
		instance->set_pos(Ivec(last_pos.x + last_size.x, last_pos.y));
		instances.back()->handle_resize();
	}
	else
	{
		instance->set_size(SDL::window_size);
	}
	instance->handle_resize();
	instances.push_back(instance);
	instance->active = true;
	current_instance = instances.size() - 1;
}

void remove_instance(size_t index)
{
	if(instances.empty())
	{
		SDL_Quit();
		TTF_Quit();
		exit(0);
	}
	// copy pos and size
	Ivec removed_pos = instances[index]->get_pos();
	Ivec removed_size = instances[index]->get_size();

	// removal
	if(instances[index] != compile_address)
	{
		delete instances[index];
	}
	else
	{
		if(tty->active)
		{
			stop_tty(tty);
		}
		compile_open = false;
	}
	instances.erase(instances.begin() + index);
	if(instances.empty())
		return;

	// size adjust
	if(index != 0)
	{
		index--;
	}
	current_instance = index;
	Ivec& last_pos = instances[index]->get_pos();
	Ivec& last_size = instances[index]->get_size();
	instances[index]->active = true;
	instances[index]->set_size(Ivec(last_size.x + removed_size.x, last_size.y));
	if(last_pos.x > removed_pos.x)
	{
		// when removed instance is left of growing instance
		instances[index]->set_pos(removed_pos);
	}
	instances[index]->handle_resize();
}
