#ifndef EDITOR_HPP
#define EDITOR_HPP

#include <SDL2/SDL_ttf.h>
#include <vector>

#include "instance.hpp"

class Editor : public Instance
{
	public:
	Editor(Ivec pos, Ivec size, short border_size);
	~Editor();

	void update() override;
	void render() override;

	void process_event(const SDL_Event& event) override;

	private:
	std::vector<std::string> text;
	size_t col, row;
	TTF_Font* font;
	SDL_Texture* render_texture;

	Ivec scroll_chars;
	Ivec glyph_size;
	Ivec num_cells;

	bool changed;
};

#endif
