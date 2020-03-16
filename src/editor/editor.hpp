#ifndef EDITOR_HPP
#define EDITOR_HPP

#include <SDL2/SDL_ttf.h>
#include <vector>

#include "instance.hpp"

class Editor : public Instance
{
	public:
	Editor(Ivec pos, Ivec size, short border_size, SDL_Color border_color,
		   SDL_Color font_color);
	~Editor();

	void update() override;
	void render() override;

	void process_event(const SDL_Event& event) override;

	void read(std::string path);
	void save(std::string path);

	private:
	std::vector<std::string> text;
	size_t col, row;
	TTF_Font* font;
	SDL_Color font_color;
	SDL_Texture* render_texture;

	Ivec scroll_chars;
	Ivec glyph_size;
	Ivec num_cells;

	bool changed;
};

#endif
