#include <dirent.h>

#include "instance.hpp"

class FileSelector : public Instance
{
	public:
	FileSelector(Ivec pos, Ivec size, short border_size,
				 SDL_Color border_color = SDL_Color{ 255, 255, 255, 255 });

	void update() override;
	void render() override;

	void process_event(const SDL_Event& event) override;

	void handle_resize() override;

	private:
	dirent** dirents;
	std::string current_path;
	TTF_Font* font;

	int sel_file;
	int num_files;
	int scroll_pos;

	bool changed;
};
