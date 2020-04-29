#ifndef SYSTEM_HPP
#define SYSTEM_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <map>
#include <vector>
#include <functional>

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
namespace Time
{
extern long long prev_time;
extern long long time;
extern bool paused;
void update_time();
void toggle_time();
long long get_time();
long long get_dtime();
};
namespace Resources
{
extern std::vector<TTF_Font*> fonts;
extern std::vector<std::string> font_paths;
extern std::vector<SDL_Texture*> text_cache;
size_t cache_text(SDL_Texture* text);
SDL_Texture* load_cache_text(size_t pos);
void load_res(int font_size);
void clear_res();
enum font_type
{
	MONO = 0
};
SDL_Texture* create_text(std::string text, font_type font, SDL_Color color);
SDL_Texture* create_shaded_text(std::string text, font_type font,
								SDL_Color fg_color, SDL_Color bg_color);
TTF_Font* get_font(font_type type);
};
namespace Util
{
std::vector<std::string> split_string(const std::string& data,
									  const char delimiter);
};
namespace Settings
{
struct Setting
{
	std::string value;
	std::function<void(std::string arg)> callback = nullptr;
};
static std::string settings_path = "res/settings/settings.ou";
static std::map<std::string, Setting> setting_values = { { "volume", {"0"} },
														 { "compile", {"make all"} },
														 { "font-size", {"15"} },
														 { "highlight-line", {"0"} } };
Setting& get_setting(std::string setting_name);
std::vector<std::pair<std::string, Setting>> get_all_settings();
void update_settings();
void save_settings(std::vector<std::pair<std::string, Setting>> new_settings);
};

#endif
