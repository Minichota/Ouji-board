#include "temp.hpp"
#include "system.hpp"

static std::vector<Temp> temporaries = {};
static Ivec temp_pos = {1000,10};
static int current_id = 0;

void render_temps()
{
	SDL_Rect rect;
	if(Instance::state == DEBUG)
	{
		for(size_t i = 0; i < temporaries.size(); i++)
		{
			rect = { temp_pos.x, temp_pos.y + (int)i * 20 };
			SDL_QueryTexture(temporaries[i].texture, nullptr, nullptr, &rect.w, &rect.h);
			SDL_RenderCopy(SDL::renderer, temporaries[i].texture, nullptr, &rect);
		}
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
			Temp{current_id, temp, Resources::create_text(temp, Resources::MONO, SDL_Color{255,0,0,255})});
		}
	}
	else
	{
		temporaries.push_back(Temp{current_id, temp, Resources::create_text(temp, Resources::MONO, SDL_Color{255,0,0,255})});
	}
	current_id++;
}
