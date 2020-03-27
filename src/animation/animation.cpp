#include <algorithm>

#include "animation.hpp"

std::vector<Animation*> Animation::animations = {};

Animation::Animation(Ivec pos, long long duration, SDL_Texture* texture)
{
	this->pos = pos;
	this->duration = duration;
	this->texture = texture;
	animations.push_back(this);
}

Animation::~Animation()
{
	animations.erase(std::remove(animations.begin(), animations.end(), this),
					 animations.end());
}

void Animation::global_render()
{
	for(Animation* animation : animations)
	{
		animation->render();
	}
}
