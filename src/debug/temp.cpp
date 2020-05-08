#include "temp.hpp"
#include "system.hpp"

static std::vector<Temp> temporaries = {};
static Ivec temp_win_pos = { 1000, 10 };
static int current_id = 0;
static bool moving = false;

void render_temps()
{
	SDL_Rect rect;
	if(Instance::state == DEBUG)
	{
		for(size_t i = 0; i < temporaries.size(); i++)
		{
			rect = { temp_win_pos.x, temp_win_pos.y + (int)i * 20 };
			SDL_QueryTexture(temporaries[i].texture, nullptr, nullptr, &rect.w, &rect.h);
			SDL_RenderCopy(SDL::renderer, temporaries[i].texture, nullptr, &rect);
		}
		SDL_Rect anchor_rect = { temp_win_pos.x, temp_win_pos.y, 8, 8 };
		SDL_RenderDrawRect(SDL::renderer, &anchor_rect);
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
