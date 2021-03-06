#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <fstream>
#include <vector>

#include "system.hpp"
#include "log.hpp"

std::vector<Instance*> instances;
size_t current_instance = 0;

namespace SDL
{

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
Ivec mouse_pos = Ivec(0, 0);

void load_sdl()
{
	if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		fprintf(stderr, "Video init failed: %s\n", SDL_GetError());
		exit(-1);
	}
	if(TTF_Init() < 0)
	{
		fprintf(stderr, "Video init failed: %s\n", SDL_GetError());
		exit(-1);
	}
	window = SDL_CreateWindow("Ouji board", SDL_WINDOWPOS_CENTERED,
							  SDL_WINDOWPOS_CENTERED, window_size.x,
							  window_size.y, SDL_WINDOW_RESIZABLE);
	renderer = SDL_CreateRenderer(window, -1, 0);
	SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
}

void clear_sdl()
{
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
	TTF_Quit();
}
}

namespace Time
{
long long prev_time;
long long time;
bool paused;
void update_time()
{
	if(!paused)
	{
		time += SDL_GetTicks() - prev_time;
		prev_time = time;
	}
	else
	{
		prev_time = SDL_GetTicks();
	}
}
void toggle_time()
{
	paused = !paused;
}
long long get_time()
{
	return time;
}
long long get_dtime()
{
	return SDL_GetTicks() - time;
}
}

namespace Resources
{
std::vector<TTF_Font*> fonts;
std::vector<std::string> font_paths = { "res/font/RobotoMono-Regular.ttf" };
std::vector<SDL_Texture*> text_cache;

size_t cache_text(SDL_Texture* text)
{
	push_log("created cache");
	text_cache.push_back(text);
	return text_cache.size() - 1;
}

SDL_Texture* load_cache_text(size_t pos)
{
	return text_cache[pos];
}

SDL_Texture* create_text(std::string text, font_type font, SDL_Color color)
{
	SDL_Surface* surface =
		TTF_RenderText_Blended(Resources::get_font(font), text.c_str(), color);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(SDL::renderer, surface);
	SDL_FreeSurface(surface);
	return texture;
}

SDL_Texture* create_shaded_text(std::string text, font_type font,
								SDL_Color fg_color, SDL_Color bg_color)
{
	SDL_Surface* surface = TTF_RenderText_Shaded(
		Resources::get_font(font), text.c_str(), fg_color, bg_color);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(SDL::renderer, surface);
	SDL_FreeSurface(surface);
	return texture;
}

void load_res(int font_size)
{
	// clear out old fonts
	for(TTF_Font* font : fonts)
	{
		TTF_CloseFont(font);
	}
	fonts.clear();
	for(std::string font_path : font_paths)
	{
		TTF_Font* font = TTF_OpenFont(font_path.c_str(), font_size);
		if(!font)
		{
			printf("[TTF] Failed to load font: %s", font_path.c_str());
		}
		fonts.push_back(font);
	}
	push_log("loaded resources");
}
void clear_res()
{
	for(TTF_Font* font : fonts)
	{
		TTF_CloseFont(font);
	}

	for(SDL_Texture* cache : text_cache)
	{
		SDL_DestroyTexture(cache);
	}
}
TTF_Font* get_font(font_type type)
{
	return fonts[type];
}
};
namespace Util
{
std::vector<std::string> split_string(const std::string& str,
									  const char delimiter)
{
	std::vector<std::string> strings;

	std::string::size_type pos = 0;
	std::string::size_type prev = 0;
	while((pos = str.find(delimiter, prev)) != std::string::npos)
	{
		strings.push_back(str.substr(prev, pos - prev));
		prev = pos + 1;
	}

	// To get the last substring (or only, if delimiter is not found)
	strings.push_back(str.substr(prev));

	return strings;
}

};

namespace Settings
{
Setting& get_setting(std::string setting_name)
{
	return setting_values[setting_name];
}
std::vector<std::pair<std::string, Setting>> get_all_settings()
{
	return std::vector<std::pair<std::string, Setting>>(setting_values.begin(), setting_values.end());
}
void update_settings()
{
	std::ifstream file(settings_path);
	std::string line;
	while(getline(file, line))
	{
		std::vector<std::string> data = Util::split_string(line, '=');
		std::map<std::string, Setting>::iterator replace = setting_values.find(data[0]);

		if(replace != setting_values.end())
		{
			// replace existing setting
			replace->second = Setting{data[1], replace->second.callback};
		}
		else
		{
			// insert new setting
			setting_values.insert(std::pair(data[0], Setting{data[1]}));
		}
	}
	file.close();
}

void save_settings(std::vector<std::pair<std::string, Setting>> new_settings)
{
	std::ofstream file(settings_path);
	std::string of_data;
	for(std::pair<std::string, Setting> line : new_settings)
	{
		of_data.append(line.first + "=" + line.second.value + "\n");
	}
	file << of_data;
	file.close();
}

};
