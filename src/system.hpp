#ifndef SYSTEM_HPP
#define SYSTEM_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <vector>

#include "instance.hpp"
#include "vectors.hpp"

extern std::vector<Instance*> instances;
extern size_t current_instance;

namespace SDL
{
extern SDL_Renderer* renderer;
extern SDL_Window* window;
extern Ivec mouse_pos;
const static Ivec window_size = { 1920, 1080 };
void load_sdl();
void clear_sdl();
};
namespace Resources
{
extern std::vector<TTF_Font*> fonts;
extern std::vector<std::string> font_paths;
extern std::vector<SDL_Texture*> text_cache;
size_t cache_text(SDL_Texture* text);
SDL_Texture* load_cache_text(size_t pos);
void load_res();
void clear_res();
enum font_type
{
	MONO = 0
};
SDL_Texture* create_text(std::string text, font_type font);
TTF_Font* get_font(font_type type);
};

#endif
