#include "Gridcell.h"

Cell::Cell(): 
	isEditable(1),
	CharNumber('0')
{}

void Cell::setNumber(const int number){
	
	CharNumber = '0' + number;
	
}

int Cell::getNumber() const{
	return int(CharNumber-'0');
}

void Cell::setEditable(const bool editable){
	isEditable = editable;
	if (isEditable){
		mouseOutColour = { 219, 184, 215, SDL_ALPHA_OPAQUE }; // light purple
		mouseOverMotionColour = { 95, 89, 191, SDL_ALPHA_OPAQUE }; // blue
		mouseDownColour = { 91, 191, 116, SDL_ALPHA_OPAQUE }; // green
		mouseUpColour = { 95, 89, 191, SDL_ALPHA_OPAQUE }; // blue
	}
	else{
		mouseOutColour = { 159, 101, 152, SDL_ALPHA_OPAQUE }; // purple
		mouseOverMotionColour = { 159, 101, 152, SDL_ALPHA_OPAQUE }; // purple
		mouseDownColour = { 159, 101, 152, SDL_ALPHA_OPAQUE }; // purple
		mouseUpColour = { 159, 101, 152, SDL_ALPHA_OPAQUE }; // purple
	}
}


void Cell::handleKeyboardEvent(const SDL_Event* event, SDL_Texture* textureCache[]){
	if (event->key.keysym.sym == SDLK_BACKSPACE && CharNumber != ' '){
		CharNumber = ' ';
		setTexture(textureCache[0]);
	}
	else if (event->type == SDL_TEXTINPUT){
		if (atoi(event->text.text)){
			CharNumber = *(event->text.text);
			setTexture(textureCache[CharNumber-'0']);
			// printf("%c\n", CharNumber);
		}
	}
}
