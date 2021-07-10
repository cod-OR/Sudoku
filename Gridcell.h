#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <stdlib.h>

#include "Button.h"

class Cell : public Button
{
	char CharNumber;

public:
	bool isEditable;
	Cell();

	void setNumber(const int number);
	int getNumber() const;

	void setEditable(const bool editable);
	void handleKeyboardEvent(const SDL_Event* event, SDL_Texture* textureCache[]);

};
