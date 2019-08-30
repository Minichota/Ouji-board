#ifndef ERROR_HPP
#define ERROR_HPP
#include <stdlib.h>
#include <vector>
#define LOAD_IMAGE_FAILURE "Failure to load image: "
#define USER_CLOSED        "User closed program with: "
#define LOAD_FONT_FAILURE  "Failure to load font: "
class Error
{
	public:
	Error();
	~Error();

	void push_reason(const char* type, const char* reason);
	const char* get_error_code();

	private:
	char code[256];
};
extern std::vector<Error*> errors;
#endif
