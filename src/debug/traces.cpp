#include "system.hpp"
#include "traces.hpp"
#include "instance.hpp"

static Ivec trace_win_pos = {100,100};

static std::string path = "";

static int selected_trace = 0;
static int prev_trace = 0;

std::vector<std::variant<Trace<float>*, Trace<int>*>> traces = {};
std::vector<std::variant<Trace<float>*, Trace<int>*>> rendered_traces = {};
std::vector<std::string> rendered_strings = {};

void render_traces()
{
	if(Instance::state == DEBUG)
	{
		rendered_strings.clear();
		rendered_traces.clear();

		// building rendered_traces
		for(size_t i = 0; i < traces.size(); i++)
		{
			std::visit([&](auto&& arg) {
				std::vector<std::string> path_split = Util::split_string(path, '/');
				std::vector<std::string> trace_split = Util::split_string(arg->path, '/');

				for(size_t i = 0; i < path_split.size(); i++)
				{
					if(trace_split[i] != path_split[i])
					{
						return;
					}
				}
				rendered_traces.push_back(arg);

			}, traces[i]);
		}
		for(size_t i = 0; i < rendered_traces.size(); i++)
		{
			SDL_Color draw_color = (int)rendered_strings.size() == selected_trace ?
			SDL_Color { 255,255,255,255 } :
			SDL_Color { 255,255,255,100 };

			SDL_Texture* render_texture;
			std::string repr = "";
			std::visit([&](auto&& arg) {
				if(arg->texture != nullptr)
					SDL_DestroyTexture(arg->texture);
				std::vector<std::string> path_split  = Util::split_string(path, '/');
				std::vector<std::string> trace_split = Util::split_string(arg->path, '/');
				repr.append(trace_split[path_split.size()]);

				// only add to this if it's not a directory //

				if(trace_split.size() - 1 == path_split.size())
				{
					repr.append(": "+std::to_string(*arg->value));
					// remove extra precision from floats
					using T = std::decay_t<decltype(arg)>;
					if constexpr (std::is_same_v<T, Trace<float>*>)
					{
						for(int i = 0; i < 4; i++)
						{
							repr.pop_back();
						}
					}
				}
				arg->texture = Resources::create_text(repr, Resources::MONO, draw_color);
				render_texture = arg->texture;
			}, rendered_traces[i]);

			if(std::find(rendered_strings.begin(), rendered_strings.end(), repr) == rendered_strings.end() ||
			   std::find(repr.begin(), repr.end(), ':') != repr.end())
			{
				SDL_Rect rect = { trace_win_pos.x, trace_win_pos.y + (int)rendered_strings.size() * 20 };
				SDL_QueryTexture(render_texture, nullptr, nullptr, &rect.w, &rect.h);
				SDL_RenderCopy(SDL::renderer, render_texture, nullptr, &rect);
				rendered_strings.push_back(repr);
			}
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
						case SDLK_RETURN:
						{
							std::string selected_string = rendered_strings[selected_trace];
							// change path if it's a directory
							if(std::find(selected_string.begin(),
										 selected_string.end(), ':') == selected_string.end())
							{
								path.append("/" + rendered_strings[selected_trace]);
								selected_trace = 0;
							}
						}
						break;
						case SDLK_j:
						{
							prev_trace = selected_trace;
							selected_trace++;
							if(selected_trace > (int)rendered_strings.size() - 1)
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
								selected_trace = (int)rendered_strings.size() - 1;
							}
						}
						break;
						case SDLK_d:
						{
							Instance::state = NORMAL;
						}
						break;
						case SDLK_h:
						{
							// pop back directory
							path = path.substr(0, path.find_last_of("\\/"));
							selected_trace = 0;
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
							// push the number
							std::visit([event](auto&& arg) {
								std::string value = std::to_string(*arg->value);
								value.push_back(event.key.keysym.sym);
								*arg->value = std::stoi(value);
							}, rendered_traces[selected_trace]);
						}
						break;
						case SDLK_BACKSPACE:
						{
							// pop back number
							std::visit([event](auto&& arg) {
								std::string value = std::to_string(*arg->value);
								value.pop_back();
								if(!value.empty())
									*arg->value = std::stoi(value);
								else
									*arg->value = 0;
							}, rendered_traces[selected_trace]);
						}
						break;
					}
				}
			}
		}
	}
	return Instance::state == DEBUG;
}

void remove_trace(void* address)
{
	// removes trace based off of address supplied.
	// used after object is destructed and shouldn't
	// be used on permanent lifetime variables
	for(size_t i = 0; i < traces.size(); i++)
	{
		std::visit([&i, address](auto&& arg) {
			if(arg->address == address)
			{
				traces.erase(traces.begin() + i--);
				SDL_DestroyTexture(arg->texture);
			}
		}, traces[i]);
	}
}
