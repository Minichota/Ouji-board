#include "system.hpp"
#include "traces.hpp"

#include <iomanip>
#include <type_traits>

static Ivec trace_win_pos = {100,100};
std::vector<std::variant<Trace<float>*, Trace<int>*>> traces = {};

void render_traces()
{
	for(size_t i = 0; i < traces.size(); i++)
	{
		SDL_Texture* render_texture;
		std::visit([&i,&render_texture](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, Trace<int>*>)
			{
				if(*arg->value != arg->prev)
				{
					if(arg->texture != nullptr)
						SDL_DestroyTexture(arg->texture);
					arg->texture = Resources::create_text(std::to_string(*arg->value), Resources::MONO, SDL_Color{0,255,255,255});
				}
				render_texture = arg->texture;
			}
            else if constexpr (std::is_same_v<T, Trace<float>*>)
			{
				if(*arg->value != arg->prev)
				{
					if(arg->texture != nullptr)
						SDL_DestroyTexture(arg->texture);
					// create new if it hasn't changed
					std::string repr = std::to_string(*arg->value);
					// remove extra precision
					for(int i = 0; i < 4; i++)
					{
						repr.pop_back();
					}
					arg->texture = Resources::create_text(repr, Resources::MONO, SDL_Color{0,255,255,255});
				}
				render_texture = arg->texture;
			}
        }, traces[i]);
		SDL_Rect rect = {trace_win_pos.x, trace_win_pos.y + (int)i * 20};
		SDL_QueryTexture(render_texture, nullptr, nullptr, &rect.w, &rect.h);
		SDL_RenderCopy(SDL::renderer, render_texture, nullptr, &rect);
	}
}


void remove_trace(Trace<float>* trace)
{
}

void remove_trace(Trace<int>* trace)
{
}
