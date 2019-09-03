#include "error.hpp"
#include <cstring>
#include <vector>
#include <iostream>
#include <algorithm>
#include "main.hpp"
static void close_window();
Error::Error()
{
	// clearing error garbage values
	memset(&error_text[0], '\0', sizeof(error_text));
}
Error::~Error()
{
	if(!(error_text[0] == '\0'))
	{
		close_window();
		exit(error_code);
	}
}

void Error::push_reason(const char* type, const char* reason)
{
	const char delimiter[] = ",\n";
	if(std::find(errors.begin(), errors.end(), this) == errors.end())
	{
		errors.push_back(this);
	}
	else
	{
		strcat(error_text, delimiter);
	}
	strcat(error_text, type);
	strcat(error_text, reason);
}

void Error::set_error_code(char code)
{
	this->error_code = code;
}

const char* Error::get_error_text()
{
	return this->error_text;
}

void close_window()
{
	for(size_t i = 0; i < errors.size(); i++)
	{
		std::cout << errors[i]->get_error_text() << std::endl;
	}
	should_close = true;
}

std::vector<Error*> errors = {};
