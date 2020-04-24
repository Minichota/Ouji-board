#ifndef SETTING_EDITOR_HPP
#define SETTING_EDITOR_HPP

#include <SDL2/SDL.h>
#include <string>
#include <vector>

#include "fade-animation.hpp"
#include "instance.hpp"
#include "system.hpp"

class SettingEditor : public Instance
{
	public:
	SettingEditor(Ivec pos, Ivec size, short border_size,
				  SDL_Color border_color);
	~SettingEditor();

	void update() override;
	void render() override;

	void process_event(const SDL_Event& event) override;

	void handle_resize() override;

	private:
	FadeAnimation animation;
	std::vector<std::pair<std::string, Settings::Setting>> data;

	size_t selected_setting;
};

#endif
