#ifndef EDITOR_HPP
#define EDITOR_HPP

#include <SDL2/SDL_ttf.h>
#include <vector>

#include "fade-animation.hpp"
#include "instance.hpp"

class Editor : public Instance
{
	public:
	Editor(Ivec pos, Ivec size, short border_size, std::string file,
		   SDL_Color border_color, SDL_Color font_color);

	void update() override;
	void render() override;

	void process_event(const SDL_Event& event) override;

	void handle_resize() override;

	void read(std::string path);
	void save(std::string path);

	bool changed;

	private:
	const static std::vector<std::string> groupings;
	std::string curr_file;
	std::vector<std::string> text;
	size_t col, row;
	size_t prev_col;
	TTF_Font* font;
	SDL_Color font_color;
	FadeAnimation animation;

	Ivec scroll_chars;
	Ivec glyph_size;
	Ivec num_cells;

};

#endif
