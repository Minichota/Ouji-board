#include "temp.hpp"
#include "system.hpp"

static std::vector<Temp> temporaries = {};
static Ivec temp_win_pos = { 1000, 10 };
static int current_id = 0;
static bool moving = false;

void render_temps()
{
	SDL_SetRenderDrawColor(SDL::renderer, 255, 255, 255, 255);
	SDL_Rect rect;
	if(Instance::state == DEBUG)
	{
		static size_t title_cache = Resources::cache_text(Resources::create_text("TEMPS",
									Resources::MONO, SDL_Color{255,255,255,255}));
		SDL_Rect title_rect = { temp_win_pos.x, temp_win_pos.y };
		SDL_QueryTexture(Resources::load_cache_text(title_cache), nullptr, nullptr, &title_rect.w, &title_rect.h);
		// render title
		SDL_RenderCopy(SDL::renderer, Resources::load_cache_text(title_cache), nullptr, &title_rect);

		constexpr int BORDER_OFFSET = 2;
		SDL_Rect border_rect = { temp_win_pos.x - BORDER_OFFSET, temp_win_pos.y - BORDER_OFFSET, 0, title_rect.h };
		for(size_t i = 0; i < temporaries.size(); i++)
		{
			rect = { temp_win_pos.x, temp_win_pos.y + (int)i * 20 + title_rect.h };
			SDL_QueryTexture(temporaries[i].texture, nullptr, nullptr, &rect.w, &rect.h);
			SDL_RenderCopy(SDL::renderer, temporaries[i].texture, nullptr, &rect);
			border_rect.w = std::max(border_rect.w, rect.w);
			border_rect.h += 20;
		}
		SDL_Rect anchor_rect = { temp_win_pos.x, temp_win_pos.y, 8, 8 };
		SDL_RenderDrawRect(SDL::renderer, &anchor_rect);
		border_rect.w += 2 * BORDER_OFFSET;
		border_rect.h += 2 * BORDER_OFFSET;
		// render border
		SDL_RenderDrawRect(SDL::renderer, &border_rect);
	}
	current_id = 0;

}

void push_temp(std::string temp)
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

void handle_temp_event(const SDL_Event& event)
{
	if(Instance::state == DEBUG)
	{
		switch(event.type)
		{
			case SDL_MOUSEBUTTONDOWN:
			{
				SDL_Rect anchor_rect = { temp_win_pos.x, temp_win_pos.y, 8, 8 };
				SDL_Point click_point = { event.button.x, event.button.y };
				moving = SDL_PointInRect(&click_point, &anchor_rect);
			}
			break;
			case SDL_MOUSEBUTTONUP:
			{
				moving = false;
			}
			break;
			case SDL_MOUSEMOTION:
			{
				if(moving)
				{
					temp_win_pos += Ivec(event.motion.xrel, event.motion.yrel);
				}
			}
			break;
		}
	}
}
