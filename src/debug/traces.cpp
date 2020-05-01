#include "system.hpp"
#include "traces.hpp"
#include "instance.hpp"

static Ivec trace_win_pos = {100,100};

static int selected_trace = 0;
static int prev_trace = 0;

std::vector<std::variant<Trace<float>*, Trace<int>*>> traces = {};

void render_traces()
{
	if(Instance::state == DEBUG)
	{
		for(size_t i = 0; i < traces.size(); i++)
		{
			const SDL_Color draw_color = (int)i == selected_trace ?
			SDL_Color { 0,255,255,255 } :
			SDL_Color { 0,0,  255,255 };

			SDL_Texture* render_texture;
			std::visit([&i,&render_texture,draw_color](auto&& arg) {
				if(*arg->value != arg->prev || selected_trace != prev_trace)
				{
					if(arg->texture != nullptr)
						SDL_DestroyTexture(arg->texture);
					std::string repr = std::string(arg->name)+": "+std::to_string(*arg->value);
					// remove extra precision from floats
					using T = std::decay_t<decltype(arg)>;
					if constexpr (std::is_same_v<T, Trace<float>*>)
					{
						for(int i = 0; i < 4; i++)
						{
							repr.pop_back();
						}
					}
					arg->texture = Resources::create_text(repr, Resources::MONO, draw_color);
				}
				render_texture = arg->texture;
				arg->prev = *arg->value;
			}, traces[i]);
			SDL_Rect rect = {trace_win_pos.x, trace_win_pos.y + (int)i * 20};
			SDL_QueryTexture(render_texture, nullptr, nullptr, &rect.w, &rect.h);
			SDL_RenderCopy(SDL::renderer, render_texture, nullptr, &rect);
		}
	}
}

bool handle_trace_event(const SDL_Event& event)
{
	switch(Instance::state)
	{
		case NORMAL:
		{
		}
		break;
		case COMMAND:
		{
			switch(event.type)
			{
				case SDL_KEYDOWN:
				{
					switch(event.key.keysym.sym)
					{
						case SDLK_d:
						{
							Instance::state = DEBUG;
						};
					}
				}
				break;
			}
		}
		break;
		case DEBUG:
		{
			switch(event.type)
			{
				case SDL_KEYDOWN:
				{
					switch(event.key.keysym.sym)
					{
						case SDLK_j:
						{
							prev_trace = selected_trace;
							selected_trace++;
							if(selected_trace > (int)traces.size() - 1)
							{
								selected_trace = 0;
							}
						}
						break;
						case SDLK_k:
						{
							prev_trace = selected_trace;
							selected_trace--;
							if(selected_trace < 0)
							{
								selected_trace = (int)traces.size() - 1;
							}
						}
						break;
						case SDLK_d:
						{
							Instance::state = NORMAL;
						}
						break;
						case SDLK_0:
						case SDLK_1:
						case SDLK_2:
						case SDLK_3:
						case SDLK_4:
						case SDLK_5:
						case SDLK_6:
						case SDLK_7:
						case SDLK_8:
						case SDLK_9:
						{
							std::visit([event](auto&& arg) {
								if(arg->editable)
								{
									std::string value = std::to_string(*arg->value);
									value.push_back(event.key.keysym.sym);
									*arg->value = std::stoi(value);
								}
							}, traces[selected_trace]);
						}
						break;
						case SDLK_BACKSPACE:
						{
							std::visit([event](auto&& arg) {
								if(arg->editable)
								{
									std::string value = std::to_string(*arg->value);
									value.pop_back();
									if(!value.empty())
										*arg->value = std::stoi(value);
									else
										*arg->value = 0;
								}
							}, traces[selected_trace]);
						}
						break;
					}
				}
			}
		}
	}
	return Instance::state == DEBUG;
}

void remove_trace(const char* name)
{
	for(size_t i = 0; i < traces.size(); i++)
	{
		std::visit([&i, name](auto&& arg) {
			if(arg->name == name)
				traces.erase(traces.begin() + i);
			SDL_DestroyTexture(arg->texture);
		}, traces[i]);
	}
}
