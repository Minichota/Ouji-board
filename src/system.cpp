#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <fstream>
#include <vector>

#include "system.hpp"

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

namespace Resources
{
std::vector<TTF_Font*> fonts;
std::vector<std::string> font_paths = { "res/font/RobotoMono-Regular.ttf" };
std::vector<SDL_Texture*> text_cache;

size_t cache_text(SDL_Texture* text)
{
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

void load_res()
{
	for(std::string font_path : font_paths)
	{
		fonts.push_back(TTF_OpenFont(font_path.c_str(), 14));
	}
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
	// TODO fix hacky solution
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
std::string& get_setting(std::string setting_name)
{
	return setting_values[setting_name];
}
std::vector<std::pair<std::string, std::string>> get_all_settings()
{
	return std::vector<std::pair<std::string, std::string>>(
		setting_values.begin(), setting_values.end());
}
void update_settings()
{
	std::ifstream file(settings_path);
	std::string line;
	while(getline(file, line))
	{
		std::vector<std::string> data = Util::split_string(line, '=');
		std::map<std::string, std::string>::iterator replace =
			setting_values.find(data[0]);
		if(replace != setting_values.end())
		{
			// replace existing setting
			replace->second = data[1];
		}
		else
		{
			// insert new setting
			setting_values.insert(std::pair(data[0], data[1]));
		}
	}
	file.close();
}

void save_settings(
	std::vector<std::pair<std::string, std::string>> new_settings)
{
	std::ofstream file(settings_path);
	std::string of_data;
	for(std::pair<std::string, std::string> line : new_settings)
	{
		of_data.append(line.first + "=" + line.second + "\n");
	}
	file << of_data;
	file.close();
}

};
