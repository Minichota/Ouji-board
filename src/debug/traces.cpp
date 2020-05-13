#include "system.hpp"
#include "traces.hpp"
#include "instance.hpp"

// represents the current window that is selected
static std::string current_path = "";

// used to update the values in the window
static int selected_trace = 0;
static int prev_trace = 0;

// a storage of the traces
std::vector<std::variant<Trace<float>*, Trace<int>*>> traces = {};
// the currently being rendered traces
std::vector<std::variant<Trace<float>*, Trace<int>*>> rendered_traces = {};
// a storage of only the strings being rendered
std::vector<std::string> rendered_strings = {};

SDL_Rect render_traces(Ivec window_pos)
{
	static size_t title_cache = Resources::cache_text(Resources::create_text("TRACES", Resources::MONO, SDL_Color{255,255,255,255}));
	static size_t star_cache  = Resources::cache_text(Resources::create_text("*", Resources::MONO, SDL_Color{0,255,255,255}));
	SDL_Rect title_rect = { window_pos.x, window_pos.y };
	SDL_QueryTexture(Resources::load_cache_text(title_cache), nullptr, nullptr, &title_rect.w, &title_rect.h);

	constexpr int OUTLINE_OFFSET = 2;
	SDL_Rect outline_rect = { window_pos.x - OUTLINE_OFFSET, window_pos.y - OUTLINE_OFFSET, 0, title_rect.h };
	rendered_strings.clear();
	rendered_traces.clear();

	// building rendered_traces
	for(size_t i = 0; i < traces.size(); i++)
	{
		std::visit([&](auto&& arg) {
			auto path_split  = Util::split_string(current_path, '/');
			auto trace_split = Util::split_string(arg->path, '/');

			for(size_t i = 0; i < path_split.size(); i++)
			{
				if(trace_split[i] != path_split[i])
					return;
			}
			rendered_traces.push_back(arg);

		}, traces[i]);
	}
	for(size_t i = 0; i < rendered_traces.size(); i++)
	{
		SDL_Color draw_color = { 255, 0, 0, 255 };

		SDL_Texture* render_texture;
		std::string repr = "";
		std::visit([&](auto&& arg) {
			// remove old texture
			if(arg->texture != nullptr)
				SDL_DestroyTexture(arg->texture);
			// extract string
			auto path_split  = Util::split_string(current_path, '/');
			auto trace_split = Util::split_string(arg->path, '/');
			repr.append(trace_split[path_split.size()]);

			// only append value to this if it's not a directory //

			/* checks if it's a directory by checking how
			   many more splits are left in the path */
			if(trace_split.size() - 1 == path_split.size())
			{
				repr.append(": "+std::to_string(*arg->value));

				// remove extra decimals from floats
				using T = std::decay_t<decltype(arg)>;
				if constexpr (std::is_same_v<T, Trace<float>*>)
					repr.erase(repr.end() - 4, repr.end());
			}
			// recreate texture
			arg->texture = Resources::create_text(repr, Resources::MONO, draw_color);
			render_texture = arg->texture;
		}, rendered_traces[i]);

		/* check if it's in the rendered strings already
		   when it's a directory to avoid duplicates */
		if(std::find(rendered_strings.begin(), rendered_strings.end(), repr) == rendered_strings.end()
		|| std::find(repr.begin(), repr.end(), ':') != repr.end())
		{
			SDL_Rect rect = { window_pos.x, window_pos.y + (int)rendered_strings.size() * 20 + title_rect.h };
			SDL_QueryTexture(render_texture, nullptr, nullptr, &rect.w, &rect.h);
			SDL_RenderCopy(SDL::renderer, render_texture, nullptr, &rect);
			outline_rect.w = std::max(outline_rect.w, rect.w);
			outline_rect.h += 20;
			if((int)selected_trace == (int)rendered_strings.size())
			{
				// render star
				SDL_Rect star_rect = { rect.x - 20, rect.y };
				SDL_QueryTexture(Resources::load_cache_text(star_cache), nullptr, nullptr, &star_rect.w, &star_rect.h);
				SDL_RenderCopy(SDL::renderer, Resources::load_cache_text(star_cache), nullptr, &star_rect);
			}
			rendered_strings.push_back(repr);
		}
	}
	// render outline
	outline_rect.w += OUTLINE_OFFSET * 2;
	outline_rect.h += OUTLINE_OFFSET * 2;
	SDL_RenderDrawRect(SDL::renderer, &outline_rect);

	// render title
	SDL_RenderCopy(SDL::renderer, Resources::load_cache_text(title_cache), nullptr, &title_rect);
	return outline_rect;
}

void handle_trace_event(const SDL_Event& event)
{
	switch(Instance::state)
	{
		case NORMAL:
		{
		}
		break;
		case COMMAND:
		{
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
							// change current_path if it's a directory
							if(std::find(selected_string.begin(),
										 selected_string.end(), ':') == selected_string.end())
							{
								current_path.append("/" + rendered_strings[selected_trace]);
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
						case SDLK_h:
						{
							// pop back directory
							current_path = current_path.substr(0, current_path.find_last_of("\\/"));
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
								int path_counts  = Util::split_string(current_path, '/').size();
								int trace_counts = Util::split_string(arg->path, '/').size();
								// assure that it only takes place on values and not directories
								if(trace_counts - 1 == path_counts)
								{
									std::string value = std::to_string(*arg->value);
									value.push_back(event.key.keysym.sym);
									*arg->value = std::stoi(value);
								}
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
}

void remove_trace(void* owner)
{
	// removes trace based off of owner supplied.
	// used after object is destructed and shouldn't
	// be used on permanent lifetime variables
	for(size_t i = 0; i < traces.size(); i++)
	{
		std::visit([&i, owner](auto&& arg) {
			if(arg->owner == owner)
			{
				traces.erase(traces.begin() + i--);
				SDL_DestroyTexture(arg->texture);
			}
		}, traces[i]);
	}
}
