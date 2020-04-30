#include <SDL2/SDL.h>

#include "animation.hpp"
#include "editor.hpp"
#include "fade-animation.hpp"
#include "file-selector.hpp"
#include "instance-control.hpp"
#include "setting-editor.hpp"
#include "system.hpp"
#include "terminal.hpp"
#include "tty.hpp"
#include "vectors.hpp"
#include "traces.hpp"

using namespace SDL;
using namespace Resources;

static bool process_events()
{
	SDL_Event event;
	while(SDL_PollEvent(&event))
	{
		switch(event.type)
		{
			case SDL_QUIT:
			{
				return false;
			}
			break;
			case SDL_KEYDOWN:
			{
				switch(event.key.keysym.sym)
				{
					case SDLK_z:
					{
						if(Instance::state == COMMAND)
						{
							Settings::update_settings();
							Instance::state = NORMAL;
						}
					}
					break;
				}
			}
			break;
		}
		handle_events(event);

		if(!instances.empty())
			instances[current_instance]->process_event(event);
	}
	return true;
}

int main()
{
	load_sdl();
	Settings::update_settings();
	load_res(std::stoi(Settings::get_setting("font-size").value));
	Settings::Setting& highlight_setting = Settings::get_setting("highlight-line");
	highlight_setting.callback = [&](std::string args) -> void
	{
		for(Instance* instance : instances)
		{
			Editor* editor = dynamic_cast<Editor*>(instance);
			if(editor != nullptr)
			{
				editor->changed = true;
			}
		}
	};
	Settings::Setting& font_size_setting = Settings::get_setting("font-size");
	font_size_setting.callback = [&](std::string args) -> void
	{
		load_res(std::stoi(args));
	};
	Ivec instance_size = { window_size.x, window_size.y };
	FileSelector* fs =
		new FileSelector(Ivec(0, 0), Ivec(window_size.x, window_size.y), 5,
						 SDL_Color{ 255, 255, 255, 255 });
	push_instance(fs);

	current_instance = 0;
	instances[current_instance]->active = true;

	size_t normal_cache = Resources::cache_text(Resources::create_text(
		"NORMAL MODE", Resources::MONO, SDL_Color{ 255, 255, 255, 255 }));
	size_t command_cache = Resources::cache_text(Resources::create_text(
		"COMMAND MODE", Resources::MONO, SDL_Color{ 255, 255, 255, 255 }));

	push_trace(&mouse_pos.x);
	push_trace(&mouse_pos.y);

	while(true)
	{
		if(!process_events())
		{
			break;
		}

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);
		for(size_t i = 0; i < instances.size(); i++)
		{
			instances[i]->render();
		}

		render_traces();

		/* rendering of current_state to corner of screen */
		SDL_Texture* texture = nullptr;
		switch(Instance::state)
		{
			case NORMAL:
			{
				texture = Resources::load_cache_text(normal_cache);
			}
			break;
			case COMMAND:
			{
				texture = Resources::load_cache_text(command_cache);
			}
			break;
		}
		SDL_Rect dest_rect;
		SDL_QueryTexture(texture, nullptr, nullptr, &dest_rect.w, &dest_rect.h);
		constexpr int CORNER_OFFSET = 5;
		dest_rect.x = CORNER_OFFSET;
		dest_rect.y = window_size.y - dest_rect.h - CORNER_OFFSET;
		SDL_RenderCopy(renderer, texture, nullptr, &dest_rect);

		SDL_GetMouseState(&mouse_pos.x, &mouse_pos.y);
		/* render */
		Animation::global_render();
		SDL_RenderPresent(renderer);

		Time::update_time();
		SDL_Delay(1000.0f / 144.0f);
	}

	close_all_instances();
	clear_res();
	clear_sdl();
	return 0;
}
