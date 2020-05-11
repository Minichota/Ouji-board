#include <string>

#include "instance-view.hpp"
#include "system.hpp"

std::vector<Instance*> views;

static Ivec view_pos = { 500, 500 };

static size_t selected_view = 0;
static bool moving = false;


void push_view(Instance* view)
{
	views.push_back(view);
}

void pop_view(Instance* view)
{
	for(size_t i = 0; i < views.size(); i++)
	{
		if(views[i] == view)
		{
			if(view == views[selected_view])
				views[selected_view] = 0;
			views.erase(views.begin() + i);
			return;
		}
	}
}

void render_views()
{
	SDL_SetRenderDrawColor(SDL::renderer, 255, 255, 255, 255);
	static size_t title_cache = Resources::cache_text(Resources::create_text("INSTANCES", Resources::MONO, SDL_Color{255,255,255,255}));
	static size_t star_cache  = Resources::cache_text(Resources::create_text("*", Resources::MONO, SDL_Color{ 0, 255, 255, 255 }));
	SDL_Rect title_rect = { view_pos.x, view_pos.y };
	SDL_QueryTexture(Resources::load_cache_text(title_cache), nullptr, nullptr, &title_rect.w, &title_rect.h);
	SDL_RenderCopy(SDL::renderer, Resources::load_cache_text(title_cache), nullptr, &title_rect);

	constexpr int OUTLINE_OFFSET = 2;
	SDL_Rect outline_rect = { view_pos.x - OUTLINE_OFFSET, view_pos.y - OUTLINE_OFFSET, 0, title_rect.h };
	// render list of instances
	for(size_t i = 0; i < views.size(); i++)
	{
		SDL_Texture* render_texture = Resources::create_text("at x pos: " + std::to_string(views[i]->get_pos().x),
									  Resources::MONO, SDL_Color{ 255, 0, 0, 255 });
		SDL_Rect render_rect = { view_pos.x, view_pos.y + (int)i * 20 + title_rect.h };
		SDL_QueryTexture(render_texture, nullptr, nullptr, &render_rect.w, &render_rect.h);
		SDL_RenderCopy(SDL::renderer, render_texture, nullptr, &render_rect);
		SDL_DestroyTexture(render_texture);
		if(i == selected_view)
		{
			SDL_Rect star_rect = { render_rect.x - 20, render_rect.y };
			SDL_QueryTexture(Resources::load_cache_text(star_cache), nullptr, nullptr, &star_rect.w, &star_rect.h);
			SDL_RenderCopy(SDL::renderer, Resources::load_cache_text(star_cache), nullptr, &star_rect);
		}
		outline_rect.h += 20;
	}

	int views_height = outline_rect.h;
	outline_rect.h = OUTLINE_OFFSET + title_rect.h;

	// render serialized instance
	if(views.size() > 0)
	{
		std::vector<std::string> render_texts = views[selected_view]->serialize();
		for(size_t i = 0; i < render_texts.size(); i++)
		{
			SDL_Texture* render_texture = Resources::create_text(render_texts[i],
										  Resources::MONO, SDL_Color{ 255, 0, 0, 255 });
			SDL_Rect render_rect = { view_pos.x + 200, view_pos.y + (int)i * 20 + title_rect.h };
			SDL_QueryTexture(render_texture, nullptr, nullptr, &render_rect.w, &render_rect.h);
			SDL_RenderCopy(SDL::renderer, render_texture, nullptr, &render_rect);
			SDL_DestroyTexture(render_texture);
			outline_rect.w = std::max(outline_rect.w, render_rect.w + 200);
			outline_rect.h += 20;
		}
	}
	outline_rect.w += OUTLINE_OFFSET * 2;
	outline_rect.h = std::max(outline_rect.h, views_height);
	SDL_RenderDrawRect(SDL::renderer, &outline_rect);
	if(Instance::state == DEBUG)
	{
		// render move box
		SDL_Rect anchor_rect = { view_pos.x, view_pos.y, 8, 8 };
		SDL_RenderDrawRect(SDL::renderer, &anchor_rect);
	}
}

void handle_view_event(const SDL_Event& event)
{
	if(Instance::state == DEBUG)
	{
		switch(event.type)
		{
			case SDL_MOUSEBUTTONDOWN:
			{
				SDL_Rect anchor_rect = { view_pos.x, view_pos.y, 8, 8 };
				SDL_Point mouse_click = { event.button.x, event.button.y };
				moving = SDL_PointInRect(&mouse_click, &anchor_rect);
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
					view_pos += Ivec(event.motion.xrel, event.motion.yrel);
			}
			break;
			case SDL_KEYDOWN:
			{
				switch(event.key.keysym.sym)
				{
					case SDLK_j:
					{
						selected_view++;
						if((int)selected_view > (int)views.size() - 1)
						{
							selected_view = 0;
						}
					}
					break;
					case SDLK_k:
					{
						selected_view--;
						if((int)selected_view < 0)
						{
							selected_view = views.size() - 1;
						}
					}
					break;
				}
			}
			break;
		}
	}
}
