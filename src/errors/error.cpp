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
	memset(&code[0], '\0', sizeof(code));
}
Error::~Error()
{
	if(!(code[0] == '\0'))
	{
		close_window();
		exit(1);
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
		strcat(code, delimiter);
	}
	strcat(code, type);
	strcat(code, reason);

}
const char* Error::get_error_code()
{
	return this->code;
}

void close_window()
{
	for(size_t i = 0; i < errors.size(); i++)
	{
		std::cout << errors[i]->get_error_code() << std::endl;
	}
	should_close = true;
}

std::vector<Error*> errors = {};
