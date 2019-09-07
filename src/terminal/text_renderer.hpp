#ifndef TEXT_RENDERER_HPP
#define TEXT_RENDERER_HPP
#include <string>
#include <SDL2/SDL_ttf.h>
#include "window.hpp"
#include "SDL_FontCache.h"
#include "renderable.hpp"
#include "sizeable.hpp"
class Text_Renderer : public Renderable, public Sizeable {
	public:
	Text_Renderer(SDL_Renderer* const renderer, int x, int y, short w, short h, const char* font_path);
	~Text_Renderer();

	void update();
	void render();

	void push_text(const char* text);
	void set_text (const char* text);

	FC_Font* get_font();
	std::string get_text();

	private:
	std::string text;
	bool active = false;

	FC_Font* font;
	unsigned short font_size;

	// represents the text
	SDL_Texture* texture;
};
#endif
