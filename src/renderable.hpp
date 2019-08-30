#ifndef RENDERABLE_HPP
#define RENDERABLE_HPP
#include <SDL2/SDL_image.h>
class Renderable {
	public:
	Renderable(SDL_Renderer* renderer);
	virtual ~Renderable();
	virtual void update();
	virtual void render() = 0;

	protected:
	SDL_Renderer* const renderer;
};
#endif
