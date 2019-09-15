#ifndef INPUT_HANDLER_HPP
#define INPUT_HANDLER_HPP

#include <string>
#include <SDL2/SDL.h>

#include "text_renderer.hpp"
#include "event_handler.hpp"

class Input_Handler : Event_Handler {
	public:
	Input_Handler();
	~Input_Handler();

	void handle_event(const SDL_Event event);
	bool handle_ansi_seq(std::string& sequence);

	void clear_buffers();

	void set_out(bool out);
	std::string& get_text_buffer();
	std::string get_cmd_buffer();
	bool is_out();

	private:
	std::string text_buffer;
	std::string cmd_buffer;

	bool cmd_out = false;
};
#endif
