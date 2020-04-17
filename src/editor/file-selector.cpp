#include <SDL2/SDL_ttf.h>
#include <algorithm>
#include <sys/stat.h>
#include <sys/types.h>

#include "editor.hpp"
#include "file-selector.hpp"
#include "instance-control.hpp"
#include "system.hpp"

FileSelector::FileSelector(Ivec pos, Ivec size, short border_size,
						   SDL_Color border_color) :
Instance(pos, size, border_size, border_color)
{
	this->current_path = ".";
	this->font = Resources::get_font(Resources::MONO);
	this->sel_file = 0;
	SDL_StartTextInput();
	this->changed = true;
	this->num_files = scandir(current_path.c_str(), &dirents, 0, versionsort);
	this->dirents[num_files] = nullptr;
	this->scroll_pos = 0;
	if(num_files < 0)
		printf("[IO] Failed to open directory");
}

void FileSelector::update()
{
	static Ivec glyph_size;
	if(!glyph_size)
	{
		// only call once
		TTF_SizeText(font, " ", &glyph_size.x, &glyph_size.y);
	}
	while(sel_file - scroll_pos >= size.y / glyph_size.y)
	{
		this->scroll_pos++;
		this->changed = true;
	}
	while(sel_file - scroll_pos < 0)
	{
		this->scroll_pos--;
		this->changed = true;
	}
}

void FileSelector::render()
{
	static Ivec glyph_size;
	TTF_SizeText(font, " ", &glyph_size.x, &glyph_size.y);
	Instance::render();
	if(changed)
	{
		SDL_Texture* render_complete = SDL_CreateTexture(
			SDL::renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
			(size.x - border_size * 2), (size.y - border_size * 2));
		SDL_SetRenderTarget(SDL::renderer, render_complete);

		for(int i = 0; i < num_files; i++)
		{
			SDL_Texture* texture_part = Resources::create_text(
				dirents[i]->d_name, Resources::MONO,
				i == sel_file ? SDL_Color{ 0, 0, 255, 255 }
							  : SDL_Color{ 255, 255, 255, 255 });
			SDL_Rect line_pos;
			line_pos.x = 0;
			line_pos.y = i * glyph_size.y - scroll_pos * glyph_size.y;
			SDL_QueryTexture(texture_part, nullptr, nullptr, &line_pos.w,
							 &line_pos.h);
			if(i == sel_file)
			{
				SDL_SetRenderDrawColor(SDL::renderer, 0, 0, 255, 255);
			}
			else
			{
				SDL_SetRenderDrawColor(SDL::renderer, 255, 255, 255, 255);
			}
			SDL_RenderCopy(SDL::renderer, texture_part, nullptr, &line_pos);

			SDL_DestroyTexture(texture_part);
		}

		this->render_texture = render_complete;
		SDL_SetRenderTarget(SDL::renderer, nullptr);
		this->changed = false;
	}
	if(render_texture != nullptr)
	{
		// rendering all text
		SDL_Rect rect = { pos.x + border_size, pos.y + border_size };
		SDL_QueryTexture(render_texture, nullptr, nullptr, &rect.w, &rect.h);
		SDL_RenderCopy(SDL::renderer, render_texture, nullptr, &rect);
	}
}

void FileSelector::process_event(const SDL_Event& event)
{
	switch(Instance::state)
	{
		case NORMAL:
		{
			switch(event.type)
			{
				case SDL_KEYDOWN:
				{
					switch(event.key.keysym.sym)
					{
						case SDLK_j:
						{
							this->sel_file++;
							if(sel_file > num_files - 1)
							{
								this->sel_file = 0;
							}
							this->changed = true;
						}
						break;
						case SDLK_k:
						{
							this->sel_file--;
							if(sel_file < 0)
							{
								sel_file = num_files - 1;
							}
							this->changed = true;
						}
						break;
						case SDLK_RETURN:
						{
							std::string prev_path = current_path;
							this->current_path.append(
								std::string("/") + dirents[sel_file]->d_name);

							struct stat file_spec;
							if(stat(current_path.c_str(), &file_spec) < 0)
								printf("[IO] Failed to check file description");
							if(!S_ISDIR(file_spec.st_mode))
							{
								// create editor if is file
								Editor* editor =
									new Editor(Ivec(0, 0), SDL::window_size, 5,
											   current_path,
											   SDL_Color{ 255, 255, 255, 255 },
											   SDL_Color{ 255, 255, 255, 255 });
								push_instance(editor);
								this->current_path = prev_path;
							}
							else
							{
								// remove old directories
								for(int i = 0; i < num_files; i++)
								{
									delete dirents[i];
								}
								delete dirents;
								// add new directories
								this->num_files =
									scandir(current_path.c_str(), &dirents, 0,
											versionsort);
								this->dirents[num_files] = nullptr;
								if(num_files < 0)
									printf("[IO] Failed to open directory");
							}
							this->changed = true;
							this->sel_file = 0;
						}
						break;
					}
					SDL_StartTextInput();
				}
				break;
			}
		}
		break;
		case COMMAND:
		{
			SDL_StopTextInput();
		};
		break;
	}
}

void FileSelector::handle_resize()
{
	this->changed = true;
}
