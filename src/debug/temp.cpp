#include "temp.hpp"
#include "system.hpp"

static std::vector<Temp> temporaries = {};
static int current_id = 0;

SDL_Rect render_temps(Ivec window_pos)
{
	SDL_Rect rect;
	static size_t title_cache = Resources::cache_text(Resources::create_text("TEMPS",
								Resources::MONO, SDL_Color{255,255,255,255}));
	SDL_Rect title_rect = { window_pos.x, window_pos.y };
	SDL_QueryTexture(Resources::load_cache_text(title_cache), nullptr, nullptr, &title_rect.w, &title_rect.h);
	// render title
	SDL_RenderCopy(SDL::renderer, Resources::load_cache_text(title_cache), nullptr, &title_rect);

	constexpr int BORDER_OFFSET = 2;
	SDL_Rect border_rect = { window_pos.x - BORDER_OFFSET, window_pos.y - BORDER_OFFSET, 0, title_rect.h };
	for(size_t i = 0; i < temporaries.size(); i++)
	{
		rect = { window_pos.x, window_pos.y + (int)i * 20 + title_rect.h };
		SDL_QueryTexture(temporaries[i].texture, nullptr, nullptr, &rect.w, &rect.h);
		SDL_RenderCopy(SDL::renderer, temporaries[i].texture, nullptr, &rect);
		border_rect.w = std::max(border_rect.w, rect.w);
		border_rect.h += 20;
	}
	// render border
	border_rect.w += 2 * BORDER_OFFSET;
	border_rect.h += 2 * BORDER_OFFSET;
	SDL_RenderDrawRect(SDL::renderer, &border_rect);
	current_id = 0;
	return border_rect;
}

void push_temp(std::string temp)
{
	if(Instance::state == DEBUG)
	{
		if(current_id < (int)temporaries.size())
		{
			if(temporaries[current_id].text != temp)
			{
				SDL_DestroyTexture(temporaries[current_id].texture);
				temporaries.erase(temporaries.begin() + current_id);
				temporaries.insert(temporaries.begin() + current_id,
					Temp {
						current_id, temp,
						Resources::create_text(temp, Resources::MONO, SDL_Color{255,0,0,255})
					});
			}
		}
		else
		{
			temporaries.push_back(
				Temp {
					current_id, temp,
					Resources::create_text(temp, Resources::MONO, SDL_Color{255,0,0,255})
				});
		}
		current_id++;
	}
}

void handle_temp_event(const SDL_Event& event)
{
}
