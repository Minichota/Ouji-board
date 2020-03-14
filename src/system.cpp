#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
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

SDL_Texture* create_text(std::string text, font_type font)
{
	SDL_Surface* surface =
		TTF_RenderText_Blended(Resources::get_font(font), text.c_str(),
							   SDL_Color{ 255, 255, 255, 255 });
	SDL_Texture* texture = SDL_CreateTextureFromSurface(SDL::renderer, surface);
	SDL_FreeSurface(surface);
	return texture;
}

void load_res()
{
	for(std::string font_path : font_paths)
	{
		fonts.push_back(TTF_OpenFont(font_path.c_str(), 12));
	}
}
void clear_res()
{
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
