#ifndef SETTING_EDITOR_HPP
#define SETTING_EDITOR_HPP

#include <SDL2/SDL.h>
#include <string>
#include <vector>

#include "instance.hpp"

class SettingEditor : public Instance
{
	public:
	SettingEditor(Ivec pos, Ivec size, short border_size,
				  SDL_Color border_color);
	~SettingEditor();

	void update() override;
	void render() override;

	void process_event(const SDL_Event& event) override;

	private:
	SDL_Texture* render_texture;
	std::vector<std::pair<std::string, std::string>> setting_data;

	size_t selected_setting;

	bool changed;
};

#endif
