#include <iostream>

#include "editor.hpp"

Editor::Editor(SDL_Renderer* const renderer, int x, int y, unsigned short w, unsigned short h, const char* font_path) :
Window(renderer, x, y, w, h),
text_renderer(renderer, x, y, w, h, font_path),
cell_size(0,0),
cursor_rect{x, y, 0, 0},
cursor_pos(x, y)
{
	cell_size.x = 7;
	cell_size.y = 22;
	cursor_rect.w = cell_size.x;
	cursor_rect.h = cell_size.y;
}

Editor::~Editor()
{
}

void Editor::update()
{
	// clear up newlines
	// TODO create this shit

	std::string out_string;
	for(size_t i = 0; i < cells.size(); i++)
	{
		for(size_t j = 0; j < cells[i].size(); j++)
		{
			out_string.push_back(cells[i][j].c);
		}
	}
	text_renderer.set_text(out_string);
}

template <class T>
size_t find_index(std::vector<std::vector<T>> input, size_t x, size_t y)
{
	size_t index = 0;
	for(size_t i = 0; i < y; i++)
	{
		index += input[i].size();
	}
	index += x;
	return index;
}

void Editor::render()
{
	std::string out = text_renderer.get_text();

	SDL_Rect cursor_draw_pos = FC_GetCharacterOffset(text_renderer.get_font(), find_index<Cell>(cells, cursor_pos.x, cursor_pos.y), size.x, text_renderer.get_text().c_str());
	cursor_rect.x = cursor_draw_pos.x;
	cursor_rect.y = cursor_draw_pos.y;
	SDL_SetRenderDrawColor(renderer, 0, 100, 100, 255);
	SDL_RenderFillRect(renderer, &cursor_rect);
	text_renderer.render();
}

void Editor::handle_event(const SDL_Event event)
{
	switch(event.type)
	{
		case SDL_TEXTINPUT:
		{
			insert_text(event.text.text);
		} break;
		case SDL_KEYDOWN:
		{
			switch(event.key.keysym.sym)
			{
				case SDLK_UP:
				{
					move_cursor(map_direction_to_position(UP));
				} break;
				case SDLK_DOWN:
				{
					move_cursor(map_direction_to_position(DOWN));
				} break;
				case SDLK_RIGHT:
				{
					move_cursor(map_direction_to_position(RIGHT));
				} break;
				case SDLK_LEFT:
				{
					move_cursor(map_direction_to_position(LEFT));
				} break;
				case SDLK_RETURN:
				{
					insert_text("\n");
					move_cursor(map_direction_to_position(DOWN_FRONT));
				} break;
				case SDLK_BACKSPACE:
				{
					pop_text();
				} break;
			}
		} break;
	}
}

STvec Editor::get_end(size_t pos) {
	size_t i;
	size_t i_underflow_check = cells[cursor_pos.y].size();
	for(i = cells[cursor_pos.y].size() - 1; i >= 0; i--)
	{
		if(i_underflow_check < i)
		{
			// empty column
			return STvec(0,pos);
		}
		if(cells[cursor_pos.y][i].c != '\0')
		{
			// loop completed
			break;
		}
		i_underflow_check = i;
	}
	return STvec(i,pos);
}

void Editor::move_cursor(const STvec pos)
{
	this->cursor_pos = pos;
}

STvec Editor::map_direction_to_position(const Direction direction)
{
	STvec out = cursor_pos;
	switch(direction)
	{
		case UP:
		{
		} break;
		case DOWN:
		{
			if(cells.size() - 1 > out.y)
			{
				out = map_direction_to_position(DOWN_FRONT);
			}
		} break;
		case LEFT:
		{
			size_t x = out.x;
			out.x--;
			// because of size_t being unsigned,
			// we have to check for underflow.
			if(x < out.x)
			{
				out = map_direction_to_position(UP_END);
			}
		} break;
		case RIGHT:
		{
			out.x++;
			if(out.x > cells[out.y].size())
			{
				if(out.y < cells.size() - 1)
				{
					// wrap to next line
					out = map_direction_to_position(DOWN_FRONT);
					// increment x to skip over \n
					out.x++;
				}
				else
				{
					// end of last line
					out.x--;
				}
			}
		} break;
		case UP_END:
		{
			if(cursor_pos.y > 0)
			{
				cursor_pos.y--;
				out = get_end(cursor_pos.y);
			}
		} break;
		case DOWN_END:
		{
		} break;;
		case UP_FRONT:
		{
		} break;
		case DOWN_FRONT:
		{
			out.x = 0;
			out.y++;
		} break;
	}
	std::cout << out << std::endl;
	return out;
}

void Editor::push_text(const char* text)
{
	const char* c = &text[0];
	while(*c != '\0')
	{
		if(cells.size() <= cursor_pos.y)
		{
			insert_column(cursor_pos.y);
		}
		push_cell(cursor_pos.x, cursor_pos.y);
		cells[cursor_pos.y].back().c = *c;
		move_cursor(map_direction_to_position(RIGHT));

		c++;
	}
	std::cout << cursor_pos << std::endl;
}
void Editor::insert_text(const char* text)
{
	const char* c = &text[0];
	while(*c != '\0')
	{
		if(cells.size() <= cursor_pos.y)
		{
			insert_column(cursor_pos.y);
		}
		insert_cell(cursor_pos.x);
		cells[cursor_pos.y][cursor_pos.x].c = *c;
		move_cursor(map_direction_to_position(RIGHT));

		c++;
	}
	std::cout << cursor_pos << std::endl;
}

char Editor::pop_text()
{
	return '\0';
}

void Editor::move_cell(STvec pos_1, STvec pos_2)
{
	std::cout << "pos_1:" << pos_1 << std::endl;
	std::cout << "pos_2:" << pos_2 << std::endl;
	std::cout << cells.size() << std::endl;
	std::cout << cells[pos_1.y].size() << std::endl;
	std::cout << "value == " << (cells[pos_1.y][pos_1.x].c == '\n') << std::endl;
	std::move(cells[pos_1.y].begin() + pos_1.x, cells[pos_1.y].begin() + pos_1.x, cells[pos_2.y].begin() + pos_2.x);
}

void Editor::push_cell(size_t x, size_t col)
{
	Cell new_cell = Cell(x, col, cell_size.x, cell_size.y);
	cells[col].push_back(new_cell);
}

void Editor::push_column()
{
	std::vector<Cell> column;
	cells.push_back(column);
}

void Editor::insert_cell(size_t pos)
{
	Cell new_cell = Cell(cursor_pos.x, cursor_pos.y, cell_size.x, cell_size.y);
	cells[cursor_pos.y].insert(cells[cursor_pos.y].begin() + pos, new_cell);
}

void Editor::insert_column(size_t pos)
{
	std::vector<Cell> column;
	cells.insert(cells.begin() + pos, column);
}

void Editor::pop_cell(size_t col)
{
	cells[col].pop_back();
	//cells[col].resize(cells.size() - 1);
}

void Editor::remove_cell(size_t col, size_t pos)
{
	cells[col].erase(cells[col].begin() + pos);
	//cells.resize(cells.size() - 1);
}

void Editor::scroll_up(size_t times)
{
}

void Editor::scroll_down(size_t times)
{
}
