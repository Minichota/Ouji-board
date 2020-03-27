#ifndef SETTING_EDITOR_HPP
#define SETTING_EDITOR_HPP

#include <SDL2/SDL.h>
#include <string>
#include <vector>

#include "fade-animation.hpp"
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

	void handle_resize() override;

	private:
	std::vector<std::pair<std::string, std::string>> setting_data;
	FadeAnimation animation;

	size_t selected_setting;

	bool changed;
};

#endif
