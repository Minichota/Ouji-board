#ifndef ANIMATION_HPP
#define ANIMATION_HPP

#include <SDL2/SDL.h>
#include <vector>

#include "vectors.hpp"

class Animation
{
	public:
	Animation(Ivec pos, long long duration, SDL_Texture* texture);
	~Animation();

	virtual void render() = 0;

	static void global_render();

	void set_pos(Ivec pos);

	protected:
	static std::vector<Animation*> animations;

	SDL_Texture* texture;
	Ivec pos;
	long long duration;
};

#endif
