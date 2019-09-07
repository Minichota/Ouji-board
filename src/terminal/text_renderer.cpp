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
Sizeable(x, y, w, h),
font_size(32)
{
	font = FC_CreateFont();
	FC_LoadFont(font, renderer, "data/fonts/test.ttf", font_size, FC_MakeColor(255,255,255,255), TTF_STYLE_NORMAL);
}

Text_Renderer::~Text_Renderer()
{
	FC_FreeFont(font);
}

void Text_Renderer::update()
{
}

void Text_Renderer::render()
{
	FC_DrawColumn(font, renderer, pos.x, pos.y, size.x, this->text.c_str());
}

void Text_Renderer::push_text(const char* text)
{
	this->text.append(text);
}

void Text_Renderer::set_text(const char* text)
{
	this->text = text;
}

std::string Text_Renderer::get_text()
{
	return this->text;
}

FC_Font* Text_Renderer::get_font()
{
	return this->font;
}
