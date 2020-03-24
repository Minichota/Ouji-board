#include "fade-animation.hpp"
#include "system.hpp"

FadeAnimation::FadeAnimation(Ivec pos, long long duration, SDL_Texture* texture,
							 FadeType type) :
Animation(pos, duration, texture)
{
	this->active = false;
	this->point = 0;
	this->action = GLOWING;
	this->type = type;
}

void FadeAnimation::render()
{
	if(active)
	{
		this->point += Time::get_dtime();
		if(point > duration)
		{
			this->point = 0;
			switch(action)
			{
				case GLOWING:
				{
					if(type == SEPERATED)
					{
						this->active = false;
					}
					this->action = FADING;
				}
				break;
				case FADING:
				{
					this->action = GLOWING;
					this->active = false;
				}
				break;
			}
		}
		uint8_t alpha = 0;
		switch(action)
		{
			case GLOWING:
			{
				alpha = (float)point / duration * 255;
			}
			break;
			case FADING:
			{
				alpha = 255 - ((float)point / duration * 255);
			}
			break;
		}
		std::cout << (int)alpha << std::endl;
		SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
		SDL_SetTextureAlphaMod(texture, alpha);
		SDL_Rect rect = { pos.x, pos.y };
		SDL_QueryTexture(texture, nullptr, nullptr, &rect.w, &rect.h);
		SDL_RenderCopy(SDL::renderer, texture, nullptr, &rect);
	}
}

void FadeAnimation::activate()
{
	if(!active)
	{
		this->active = true;
	}
}
