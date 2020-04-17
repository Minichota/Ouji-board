#include "setting-editor.hpp"

#include "system.hpp"

SettingEditor::SettingEditor(Ivec pos, Ivec size, short border_size,
							 SDL_Color border_color) :
Instance(pos, size, border_size, border_color),
animation(Ivec(pos.x + border_size, pos.y + border_size), 1000,
		  Resources::create_text("SAVED", Resources::MONO,
								 SDL_Color{ 255, 255, 255, 255 }),
		  SINGULAR)
{
	this->setting_data = Settings::get_all_settings();
	this->changed = true;
	this->selected_setting = 0;
	SDL_StartTextInput();
}

SettingEditor::~SettingEditor()
{
	SDL_DestroyTexture(render_texture);
}

void SettingEditor::update()
{
}

void SettingEditor::render()
{
	Instance::render();
	handle_resize();
	if(changed)
	{
		Ivec glyph_size;
		TTF_SizeText(Resources::get_font(Resources::MONO), " ", &glyph_size.x,
					 &glyph_size.y);
		if(render_texture != nullptr)
		{
			SDL_DestroyTexture(render_texture);
		}
		SDL_Texture* render_complete = SDL_CreateTexture(
			SDL::renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
			(size.x - border_size * 2), (size.y - border_size * 2));
		SDL_SetRenderTarget(SDL::renderer, render_complete);

		int max_width = 0;

		for(size_t i = 0; i < setting_data.size(); i++)
		{
			SDL_Texture* texture = Resources::create_text(
				setting_data[i].first + ":", Resources::MONO,
				i == selected_setting ? SDL_Color{ 0, 0, 255, 255 }
									  : SDL_Color{ 255, 255, 255, 255 });

			SDL_Rect render_rect = { 0, (int)i * glyph_size.y };

			SDL_QueryTexture(texture, nullptr, nullptr, &render_rect.w,
							 &render_rect.h);
			if(render_rect.w > max_width)
			{
				max_width = render_rect.w;
			}
			SDL_RenderCopy(SDL::renderer, texture, nullptr, &render_rect);

			SDL_DestroyTexture(texture);
		}
		for(size_t i = 0; i < setting_data.size(); i++)
		{
			if(setting_data[i].second.size() > 0)
			{
				SDL_Texture* texture = Resources::create_text(
					setting_data[i].second, Resources::MONO,
					SDL_Color{ 255, 255, 255, 255 });

				SDL_Rect render_rect = { max_width + glyph_size.x,
										 (int)i * glyph_size.y };

				SDL_QueryTexture(texture, nullptr, nullptr, &render_rect.w,
								 &render_rect.h);
				SDL_RenderCopy(SDL::renderer, texture, nullptr, &render_rect);

				SDL_DestroyTexture(texture);
			}
		}

		SDL_SetRenderTarget(SDL::renderer, NULL);
		this->render_texture = render_complete;
		this->changed = false;
	}
	if(render_texture != nullptr)
	{
		SDL_Rect render_rect = { pos.x + border_size, pos.y + border_size,
								 size.x - (border_size * 2),
								 size.y - (border_size * 2) };
		SDL_RenderCopy(SDL::renderer, render_texture, nullptr, &render_rect);
	}
}

void SettingEditor::process_event(const SDL_Event& event)
{
	switch(Instance::state)
	{
		case NORMAL:
		{
			const uint8_t* keys = SDL_GetKeyboardState(NULL);
			switch(event.type)
			{
				case SDL_TEXTINPUT:
				{
					this->setting_data[selected_setting].second.append(
						event.text.text);
					this->changed = true;
				}
				break;
				case SDL_KEYDOWN:
				{
					SDL_StartTextInput();
					switch(event.key.keysym.sym)
					{
						case SDLK_j:
						{
							if(keys[SDL_SCANCODE_LCTRL])
							{
								this->selected_setting++;
								if(selected_setting > setting_data.size() - 1)
								{
									this->selected_setting = 0;
								}
								this->changed = true;
							}
						}
						break;
						case SDLK_k:
						{
							if(keys[SDL_SCANCODE_LCTRL])
							{
								if(selected_setting == 0)
								{
									this->selected_setting =
										setting_data.size() - 1;
								}
								else
								{
									this->selected_setting--;
								}
								this->changed = true;
							}
						}
						break;
						case SDLK_BACKSPACE:
						{
							if(setting_data[selected_setting].second.size() > 0)
							{
								this->setting_data[selected_setting]
									.second.pop_back();
								this->changed = true;
							}
						}
						break;
						case SDLK_RETURN:
						{
							Settings::save_settings(setting_data);
							Settings::update_settings();
							animation.activate();
						}
						break;
					}
				}
				break;
			}
		}
		break;
		case COMMAND:
		{
			switch(event.type)
			{
				case SDL_KEYDOWN:
				{
					switch(event.key.keysym.sym)
					{
						case SDLK_r:
						{
							this->setting_data = Settings::get_all_settings();
							this->changed = true;
							Instance::state = NORMAL;
						}
						break;
					}
				}
				break;
			}
			SDL_StopTextInput();
		}
		break;
	}
}

void SettingEditor::handle_resize()
{
	if(render_texture != nullptr)
	{
		Ivec texture_size;
		SDL_QueryTexture(render_texture, nullptr, nullptr, &texture_size.x,
						 &texture_size.y);
		if(Ivec(size.x - border_size * 2, size.y - border_size * 2) !=
		   texture_size)
		{
			this->changed = true;
		}
	}
	else
	{
		animation.set_pos(pos + border_size);
	}
}
