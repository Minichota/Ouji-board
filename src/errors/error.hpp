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
	Error(const Error* old_err);
	~Error();

	void push_reason(const char* type, const char* reason);

	const char* get_error_text();
	bool should_finish();

	void set_error_code(char code);
	void set_finish(bool should_finish);

	static void purge_errors();

	private:
	char error_text[256];
	char error_code = 0;

	bool finish = false;
};
extern std::vector<Error*> errors;
extern std::vector<Error*> finishing_errors;
#endif
