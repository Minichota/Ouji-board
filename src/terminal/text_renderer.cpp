#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <algorithm>

#include "text_renderer.hpp"
#include "SDL_FontCache.h"
#include "sizeable.hpp"
#include "errors/error.hpp"
#include "math/vectors.hpp"
Text_Renderer::Text_Renderer(SDL_Renderer* const renderer, int x, int y, short w, short h, const char* font_path) :
Renderable(renderer),
Sizeable(x, y, w, h)
{
	font = FC_CreateFont();
	FC_LoadFont(font, renderer, "data/fonts/test.ttf", 32, FC_MakeColor(255,255,255,255), TTF_STYLE_NORMAL);
}

Text_Renderer::~Text_Renderer()
{
	FC_FreeFont(font);
}

void Text_Renderer::render()
{
	std::string string_buff = std::string(text.begin(), text.end());
	const char* string_repr = string_buff.c_str();
	FC_Draw(font, renderer, pos.x, pos.y, string_repr);
}

void Text_Renderer::push_text(const char* text)
{
	this->text.append(text);
}
void Text_Renderer::set_text(const char* text)
{
	this->text = text;
}
