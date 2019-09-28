#ifndef EDITOR_HPP
#define EDITOR_HPP

#include "event_handler.hpp"
#include "window.hpp"
#include "vectors.hpp"
#include "sizeable.hpp"
#include "text_renderer.hpp"

#include <SDL_FontCache.h>
#include <SDL2/SDL_image.h>

class Cell : public Sizeable {
	public:
	Cell(int x, int y, unsigned short w, unsigned short h) :
	Sizeable(x, y, w, h),
	c('\0')
	{
	}
	~Cell()
	{
	}
	public:
	char c;
};

const typedef enum
{
	UP,
	UP_END,
	UP_FRONT,
	DOWN,
	DOWN_END,
	DOWN_FRONT,
	LEFT,
	RIGHT
} Direction;

class Editor : public Window {
	public:
	Editor(SDL_Renderer* const renderer, int x, int y, unsigned short w, unsigned short h, const char* font_path);
	~Editor();

	void update();
	void render();

	void handle_event(const SDL_Event event);

	STvec get_end(size_t pos);

	void move_cursor(const STvec pos);
	STvec map_direction_to_position(const Direction direction);

	void push_text(const char* text);
	void insert_text(const char* text);
	char pop_text();

	void scroll_down(size_t times = 1);
	void scroll_up(size_t times = 1);

	private:
	void move_cell(STvec pos_1, STvec pos_2);

	void push_cell(size_t x, size_t col);
	void push_column();

	void insert_cell(size_t pos);
	void insert_column(size_t pos);

	void pop_cell(size_t col);
	void remove_cell(size_t col, size_t pos);

	private:
	Text_Renderer text_renderer;

	std::vector<std::vector<Cell>> cells;
	USvec cell_size;

	SDL_Rect cursor_rect;
	STvec cursor_pos;
};
#endif
