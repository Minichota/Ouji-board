#ifndef TEXT_BUFFER_HPP
#define TEXT_BUFFER_HPP

#include <string>
#include <vector>

#include "instance.hpp"

class TextBuffer : public Instance
{
	public:
	TextBuffer(Ivec pos, Ivec size, short border_size, char* data_read,
			   SDL_Color border_color = { 255, 255, 255, 255 },
			   SDL_Color font_color = { 255, 255, 255, 255 });
	~TextBuffer();
	void update() override;
	void render() override;

	void process_event(const SDL_Event& event) override;

	void handle_resize();

	void set_text(std::string data);

	private:
	std::string text;
	char* data_read;
	SDL_Color font_color;
};

#endif
