#include "log.hpp"

static std::vector<Log> logs;

SDL_Rect render_logs(Ivec window_pos)
{
	static int title_cache = Resources::cache_text(Resources::create_text("LOGS", Resources::MONO, SDL_Color{ 255, 255, 255, 255 }));
	SDL_Rect title_rect { window_pos.x, window_pos.y };
	SDL_QueryTexture(Resources::load_cache_text(title_cache), nullptr, nullptr, &title_rect.w, &title_rect.h);

	constexpr int BORDER_OFFSET = 2;
	SDL_Rect border_rect {
		window_pos.x - BORDER_OFFSET,
		window_pos.y - BORDER_OFFSET,
		title_rect.w,
		title_rect.h
	};

	long long time = SDL_GetTicks();
	for(size_t i = 0; i < logs.size(); i++)
	{
		if(time - logs[i].spawn_time < 15000)
		{
			// render log
			SDL_Rect rect { window_pos.x, window_pos.y + title_rect.h + (int)(logs.size() - 1 - i) * 20 };
			SDL_QueryTexture(logs[i].log, nullptr, nullptr, &rect.w, &rect.h);
			SDL_RenderCopy(SDL::renderer, logs[i].log, nullptr, &rect);
			border_rect.h += 20;
			border_rect.w = std::max(border_rect.w, rect.w);
		}
		else
		{
			// remove log after 15 seconds
			SDL_DestroyTexture(logs[i].log);
			logs.erase(logs.begin() + i);
		}
	}
	border_rect.w += BORDER_OFFSET*2;
	border_rect.h += BORDER_OFFSET*2;
	SDL_RenderDrawRect(SDL::renderer, &border_rect);
	// render title
	SDL_RenderCopy(SDL::renderer, Resources::load_cache_text(title_cache), nullptr, &title_rect);

	return border_rect;
}

void push_log(std::string log)
{
	logs.push_back(Log{
		Resources::create_text(log, Resources::MONO, SDL_Color{ 255, 0, 0, 255 }),
		SDL_GetTicks()
	});
	// limit log length
	if(logs.size() > 100)
		logs.erase(logs.begin());
}
