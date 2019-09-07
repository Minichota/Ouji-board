#include "input_handling.hpp"

Input_Handler::Input_Handler()
{
}
Input_Handler::~Input_Handler()
{
	buffer.clear();
}
void Input_Handler::handle_event(const SDL_Event event)
{
	switch(event.type)
	{
		case SDL_TEXTINPUT:
		{
			buffer.append(event.text.text);
		} break;
		case SDL_KEYDOWN:
		{
			switch(event.key.keysym.sym)
			{
				case SDLK_BACKSPACE:
				{
					if(buffer.size() > 0)
						buffer.pop_back();
				} break;
				case SDLK_RETURN:
				{
					buffer.push_back('\n');
				} break;
			}
		} break;
	}
}
std::string& Input_Handler::get_buffer()
{
	return this->buffer;
}
