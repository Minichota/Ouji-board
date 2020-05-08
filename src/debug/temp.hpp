#include <SDL2/SDL.h>
#include <string>
#include <vector>

struct Temp
{
	int id;
	std::string text;
	SDL_Texture* texture;
};

void render_temps();

void push_temp(std::string temp);

void handle_temp_event(const SDL_Event& event);
