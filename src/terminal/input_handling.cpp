#include <iostream>

#include "input_handling.hpp"

Input_Handler::Input_Handler()
{
}
Input_Handler::~Input_Handler()
{
	text_buffer.clear();
	cmd_buffer.clear();
}
void Input_Handler::handle_event(const SDL_Event event)
{
	switch(event.type)
	{
		case SDL_TEXTINPUT:
		{
			text_buffer.append(event.text.text);
			cmd_buffer.append(event.text.text);
		} break;
		case SDL_KEYDOWN:
		{
			switch(event.key.keysym.sym)
			{
				case SDLK_BACKSPACE:
				{
					if(cmd_buffer.size() > 0)
					{
						text_buffer.pop_back();
						cmd_buffer.pop_back();
					}
				} break;
				case SDLK_RETURN:
				{
					text_buffer.push_back('\n');
					cmd_buffer.push_back('\r');
					cmd_out = true;
				} break;
			}
		} break;
	}
}

bool Input_Handler::handle_ansi_seq(std::string& sequence)
{
	if(sequence.find("\E[2J") != std::string::npos)
	{
		text_buffer.clear();
		return false;
	}
	return true;
}

void Input_Handler::set_out(bool out)
{
	this->cmd_out = out;
}

void Input_Handler::clear_buffers()
{
	this->text_buffer.clear();
	this->cmd_buffer.clear();
}

std::string& Input_Handler::get_text_buffer()
{
	return this->text_buffer;
}

std::string Input_Handler::get_cmd_buffer()
{
	std::string temp = cmd_buffer;
	cmd_buffer.clear();
	return temp;
}

bool Input_Handler::is_out()
{
	return this->cmd_out;
}
