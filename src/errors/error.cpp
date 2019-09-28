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

Error::Error(const Error* old_err)
{
	strcpy(this->error_text, old_err->error_text);
}

Error::~Error()
{
	if(!(error_text[0] == '\0') && !finish)
	{
		close_window();
		exit(error_code);
	}
}

void Error::push_reason(const char* type, const char* reason)
{
	if(std::find(errors.begin(), errors.end(), this) == errors.end() && !finish)
	{
		errors.push_back(this);
	}
	strcat(error_text, type);
	strcat(error_text, reason);
	if (finish)
	{
		Error* e = new Error(this);
		finishing_errors.push_back(e);
		memset(&error_text[0], '\0', sizeof(error_text));
	}
}

const char* Error::get_error_text()
{
	return this->error_text;
}

bool Error::should_finish()
{
	return this->finish;
}

void Error::set_error_code(char code)
{
	this->error_code = code;
}

void Error::set_finish(bool finish)
{
	this->finish = finish;
}

void Error::purge_errors()
{
	for(size_t i = 0; i < finishing_errors.size(); i++)
	{
		std::cout << finishing_errors[i]->get_error_text() << std::endl;
		should_close = true;
	}
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
std::vector<Error*> finishing_errors = {};
