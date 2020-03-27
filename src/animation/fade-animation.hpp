#ifndef FADE_ANIMATION_HPP
#define FADE_ANIMATION_HPP

#include "animation.hpp"

enum Action
{
	GLOWING,
	FADING
};
enum FadeType
{
	SEPERATED,
	SINGULAR
};

class FadeAnimation : public Animation
{
	public:
	FadeAnimation(Ivec pos, long long duration, SDL_Texture* texture,
				  FadeType type);

	void render() override;

	void activate();

	private:
	bool active;
	FadeType type;
	Action action;
	long long point;
};

#endif
