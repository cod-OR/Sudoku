#include "Sudoku.h"

Sudoku::Sudoku():
	  WINDOW_HEIGHT(880), 
	  WINDOW_WIDTH(720),
	  GRID_HEIGHT(720), 
	  GRID_WIDTH(720),
	  GRID_ROWS(9), 
	  GRID_COLS(9),	
	  window(NULL), 
	  renderer(NULL), 
	  TotalTextures(20), 
	  TextureCache{ NULL },
	  ImageTextures{NULL},
	  font(NULL), 
	  FontSize(GRID_WIDTH/10),
	  TotalCells(81)
{}

Sudoku::~Sudoku()
{
	freeTextures();
	close();
}

int Sudoku::getIndex(int row, int col) const
{
	return row * GRID_ROWS + col;
}

bool Sudoku::init(){
	
	// Initalise SDL video subsystem
	if(SDL_Init(SDL_INIT_VIDEO) !=0){
		std::cout<<"SDL couldn't initialize: "<<SDL_GetError()<<std::endl;
		return 0;
	}

	// Initialise SDL_ttf
	if(TTF_Init()==-1){
		std::cout<<"SDL_ttf couldn't initialize!: "<<SDL_GetError()<<std::endl;
		return 0;
	}

	// Create window
	window = SDL_CreateWindow("Onkar's Sudoku", 
		SDL_WINDOWPOS_CENTERED, 
		SDL_WINDOWPOS_CENTERED, 
		WINDOW_WIDTH, WINDOW_HEIGHT, 
		0
	);
	if (window == NULL){
		std::cout << "SDL could not create window! Error: " << SDL_GetError() << std::endl;
		return 0;
	}

	// Create renderer
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (renderer == NULL){
		std::cout << "SDL could not create renderer! Error: " << SDL_GetError() << std::endl;
		return 0;
	}

	// Load font
	font = TTF_OpenFont("fonts/gooddog.ttf", FontSize);
	if (font == nullptr){
		std::cout << "Failed to load font! Error: " << TTF_GetError() << std::endl;
		return 0;
	}

	SDL_SetRenderDrawColor(renderer, 11,2,59, 255);
	return 1;

}

void Sudoku::loadTexture(SDL_Texture*& texture, const char* text, SDL_Color& fontColor){
	// Create text surface
	SDL_Surface* textSurface = TTF_RenderText_Solid(font, text, fontColor);
	if (textSurface == NULL){
		std::cout << "Could not create SDL_Surface! Error: " <<" \""<<text<<"\""<< TTF_GetError() << std::endl;
	}
	else{
		// Create texture from surface pixels
		texture = SDL_CreateTextureFromSurface(renderer, textSurface);
		if (texture == NULL){
			std::cout << "Could not create texture from surface! Error: " <<" \""<<text<<"\"" << SDL_GetError() << std::endl;
		}
		SDL_FreeSurface(textSurface);
	}
}

void Sudoku::loadImage(SDL_Texture*& texture, const char* image){
	SDL_Surface* ImageSurface= SDL_LoadBMP(image);
	if(ImageSurface == NULL){
		std::cout << "Could not create surface from bmp! " <<" \""<<image<<"\""<<": "<<SDL_GetError() << std::endl;
		return;
	}
	texture = SDL_CreateTextureFromSurface(renderer, ImageSurface);
	if (texture == NULL){
		std::cout << "Could not create texture from surface! Error: " <<" \""<<image<<"\""<< SDL_GetError() << std::endl;
	}
	SDL_FreeSurface(ImageSurface);
	// printf("Image loaded successfully!\n");
}

void Sudoku::preloadTextures(){
	// Choose colour of font
	SDL_Color fontColour = { 255, 255, 255, 255 }; // white

	// Load texture for empty space
	loadTexture(TextureCache[0], " ", fontColour);

	// Load textures for numbers from 1 to 9
	for (int num = 1; num < 10; num++){
		const char temp[] = { '0' + num, '\0' };
		loadTexture(TextureCache[num], temp, fontColour);
	}

	// Load texture for "Check", "Menu", and "New" buttons
	loadTexture(TextureCache[10], "Check", fontColour);
	loadTexture(TextureCache[11], "New", fontColour);
	loadTexture(TextureCache[12], "Menu", fontColour);
	loadTexture(TextureCache[13], "Wrong!", fontColour);
	loadTexture(TextureCache[14], "Right!", fontColour);
	loadTexture(TextureCache[15], "Play", fontColour);
	loadTexture(TextureCache[16], "Instr", fontColour);
	loadTexture(TextureCache[17], "Quit", fontColour);

	// Load textures for instr image
	loadImage(ImageTextures[0], "images/instr3.bmp");
}

void Sudoku::createLayout(){
	// TODO: change 3's and 6's in this function

	const int thinBorder =2;
	const int thickBorder = thinBorder + 6;

	int buttonStartRow=0, buttonWidth=0, buttonHeight=0;
	buttonHeight = (WINDOW_HEIGHT - 6 * thinBorder - 6 * thickBorder) / (GRID_ROWS+2);	

	buttonWidth = (GRID_WIDTH - 6*thinBorder - 4*thickBorder)/GRID_COLS ;
	buttonStartRow = 0;

	for (int row = 0; row < GRID_ROWS; row++){
		// Add to starting row
		if (row == 0) buttonStartRow += thickBorder;
		else if (row % 3 == 0) buttonStartRow += buttonHeight + thickBorder;
		else buttonStartRow += buttonHeight + thinBorder;

		// Reset starting column
		int buttonStartCol = 0;

		for (int col = 0; col < GRID_COLS; col++)
		{
			// Add to starting column
			if (col == 0) buttonStartCol += thickBorder;
			else if (col % 3 == 0) buttonStartCol += buttonWidth + thickBorder;
			else buttonStartCol += buttonWidth + thinBorder;

			// Set button position and dimensions
			SDL_Rect buttonRect = { buttonStartCol, buttonStartRow, buttonWidth, buttonHeight };
			int index = getIndex(row, col);
			grid[index].setButtonRect(buttonRect);
		}
	}

	buttonStartRow += (buttonHeight + thickBorder);

	// About Watch

	SDL_Rect buttonRect = {thickBorder, buttonStartRow, (GRID_WIDTH - 2 * thickBorder), buttonHeight };
	Timer.setButtonRect(buttonRect);


	// Watch ends


	const int numberOfOtherButtons = 3;
	CheckButton.setTexture(TextureCache[10]);
	NewButton.setTexture(TextureCache[11]);
	MenuButton.setTexture(TextureCache[12]);
	Button* otherButtons[numberOfOtherButtons] = { &CheckButton, &NewButton , &MenuButton};

	buttonWidth = (GRID_WIDTH - (numberOfOtherButtons+1) * thickBorder) / numberOfOtherButtons;

	buttonStartRow += (buttonHeight + thickBorder);

	// Reset starting column
	int borderWidthTotal = 0;

	// Set check, menu, and new buttons (last row)
	for (int button = 0; button < numberOfOtherButtons; button++){
		// Add border width to total
		if (button == 0) borderWidthTotal += thickBorder;
		else borderWidthTotal += thickBorder;
		int buttonStartCol = button * buttonWidth + borderWidthTotal;

		// Set button position and dimensions
		SDL_Rect buttonRect = { buttonStartCol, buttonStartRow, buttonWidth, buttonHeight };
		otherButtons[button]->setButtonRect(buttonRect);
		otherButtons[button]->centerTextureRect();
	}

	// printf("Layout Created!\n");

}

void Sudoku::freeTextures(){

	for (int i = 0; i < TotalTextures; i++){
		// Free texture if it exists
		if (TextureCache[i] != nullptr){
			SDL_DestroyTexture(TextureCache[i]);
			TextureCache[i] = nullptr;
		}
	}

	for (int i = 0; i < 2; i++){
		if (ImageTextures[i] != nullptr){
			SDL_DestroyTexture(ImageTextures[i]);
			ImageTextures[i] = NULL;
		}
	}
}

bool Sudoku::showtillesc(int textureid){
	bool showing =1;
	SDL_Event e;
	while(showing){
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, ImageTextures[textureid], NULL, NULL);
		SDL_RenderPresent(renderer);
		while(SDL_PollEvent(&e)){
			if(e.key.keysym.scancode==SDL_SCANCODE_ESCAPE){	
				showing=0;
			}	
			if(e.type==SDL_QUIT){
				showing=0;
				return 0;
			}
		}		
	}
	return 1;
}

void Sudoku::instr(int &running){
	if(!showtillesc(0))running=0;
}

void Sudoku::menu(){
	// Initialise SDL
	if (!init()){
		close();
	}

	// Preload textures for menu, grid, buttons and images
	preloadTextures();

	// int temp=1;
	// play(temp);   // FOR DEBUGGING PURPOSE

	int totalOptions=3;
	int OptionPosition_y[totalOptions], OptionPosition_x=WINDOW_WIDTH/2 - 50, OptionWidth=120, OptionHeight=65;
	SDL_Rect text_position;
	text_position.x=WINDOW_WIDTH/2 - OptionWidth/2;
	text_position.w=OptionWidth;
	text_position.h=OptionHeight;

	// listen to events
	SDL_Event event;
	int menuRunning=1;
	while(menuRunning){
		// play(menuRunning);
		SDL_SetRenderDrawColor(renderer, 11,2,59, 255);
		SDL_RenderClear(renderer);
		int y= WINDOW_HEIGHT/2 - (totalOptions*OptionWidth)/2;
		for(int id=0;id<totalOptions;id++, y+=90){
			text_position.y=y;
			OptionPosition_y[id] = y;
			SDL_RenderCopy(renderer, TextureCache[id+15], NULL, &text_position);
		}
		SDL_RenderPresent(renderer);
		if(SDL_PollEvent(&event)){
			switch(event.type){
				case SDL_QUIT :
				{	menuRunning=0;
					break;
				}
				case SDL_MOUSEBUTTONDOWN:
				{	
					int mouse_x, mouse_y;
					SDL_GetMouseState(&mouse_x, &mouse_y);
					if(mouse_x>OptionPosition_x && mouse_x<(OptionPosition_x + OptionWidth)){
						if(mouse_y>OptionPosition_y[0] && mouse_y<OptionPosition_y[1]){
							play(menuRunning);
							while(menuRunning==2)
								play(menuRunning=1);
						}
						else if(mouse_y>OptionPosition_y[1] && mouse_y<OptionPosition_y[2]){
							instr(menuRunning);
						}
						else if(mouse_y>OptionPosition_y[2] && mouse_y<OptionPosition_y[2]+OptionHeight){
							menuRunning=0;
						}
					}
					break;
				}
				default:
					break;
			}
		}
		
	}

	// Free button textures
	freeTextures();

	// Destroy and quit
	close();	
}

void Sudoku::generateSudoku(){

	// Create empty an empty grid to store generated Sudoku
	std::vector<int> generatedGrid(81);

	// Instantiate a Sudoku generator object and generate Sudoku with the empty grids
	

	P.generator(generatedGrid);
	for (int i = 0; i < 81; i++){
		// Set number and solution
		grid[i].setNumber(generatedGrid[i]);

		// Set editability
		if (generatedGrid[i] == 0){
			// This is a cell that can be editable
			grid[i].setEditable(true);
		}
		else{
			// This cell is fixed and cannot be edited
			grid[i].setEditable(false);
		}

		// Set texture (0 = ' ', 1 to 9 = '1' '2'... '9')
		grid[i].setTexture(TextureCache[generatedGrid[i]]);

		// Center texture onto button
		grid[i].centerTextureRect();
	}
}


void Sudoku::play(int &running){

	// Create interface layout
	createLayout();

	generateSudoku();

	// Enable text input
	SDL_StartTextInput();

	// Set first current cell selected
	Cell* currentCellSelected;
	int selectedCellId=0;
	bool init_selection=0;
	for (int cell = 0; cell < TotalCells; cell++){
		if (!init_selection && grid[cell].isEditable){
			currentCellSelected = &grid[cell];
			currentCellSelected->setSelected(true);
			init_selection=1;
			selectedCellId=cell;
		}
		else{
			grid[cell].setSelected(0);
		}
	}

	// Loop variables
	SDL_Event event;
	bool gameRunning = 1;
	bool checkSolution=0;
	bool completed=0;
	bool CompletionFlag=0;
	time_t startTimer= time(NULL);
	long long checkLightTime=1e10;
	char timer[10];

	while(gameRunning){
		while (SDL_PollEvent(&event) != 0){
			if (event.type == SDL_QUIT){
				gameRunning = 0;
				running=0;
				break;
			}

			// Handle mouse event for "Check" button
			if (CheckButton.getMouseEvent(&event) == ButtonState::BUTTON_MOUSE_DOWN){
				checkSolution = true;
			}

			// Handle mouse event for "New" button
			if (NewButton.getMouseEvent(&event) == ButtonState::BUTTON_MOUSE_DOWN){
				running=2;																	// TODO: Convert this to emum
				return;
			}

			// Handle mouse event for "Menu" button
			if (MenuButton.getMouseEvent(&event) == ButtonState::BUTTON_MOUSE_DOWN){
				return;
			}

			for (int cell = 0; cell < TotalCells; cell++){
				if (grid[cell].isEditable){
					// Set button state and return if mouse pressed on cell
					if (grid[cell].getMouseEvent(&event) == ButtonState::BUTTON_MOUSE_DOWN){
						// Set current cell selected to false
						currentCellSelected->setSelected(false);
						// Set new cell selected to true
						currentCellSelected = &grid[cell];
						currentCellSelected->setSelected(true);
						selectedCellId=cell;
					}
				}
			}
			// Handle keyboard events for current cell selected
			currentCellSelected->handleKeyboardEvent(&event, TextureCache);
			P.setValue(selectedCellId, grid[selectedCellId].getNumber());

		}

		// If "Check" button was clicked
		if (checkSolution){
			// Check if complete
			checkLightTime = min(checkLightTime, (long long)(difftime(time(NULL) , startTimer)+1ll));
			if(P.checker(0,0,1))
				completed = true;
			else{
				// Set texture to "Wrong!"
				CheckButton.setTexture(TextureCache[13]);
			}

		}

		if(completed && CompletionFlag==0){
			CompletionFlag=1;
			std::string temp_timer= "Time Taken: " + std::to_string(SCORE);
            int length= temp_timer.size();
            for(int pos=0;pos<length;pos++){
                timer[pos]=temp_timer[pos];
            }
            timer[length]='\0';			
			for(int cell=0;cell<TotalCells;cell++){
				grid[cell].isEditable=0;
				grid[cell].setSelected(0);
			}
			// Set texture to "Right!"
			CheckButton.setTexture(TextureCache[14]);
		}
		
		if(completed)
			SDL_SetRenderDrawColor(renderer, 0, 125, 19, 255);
		else if(checkSolution)
			SDL_SetRenderDrawColor(renderer,  125, 0, 0, 255);
		else
			SDL_SetRenderDrawColor(renderer, 0,0,0, 255);

		SDL_RenderClear(renderer);
		for (int cell = 0; cell < TotalCells; cell++){
			grid[cell].renderButton(renderer);
			grid[cell].centerTextureRect();
			grid[cell].renderTexture(renderer);
		}

		if(!completed){
			long long seconds =difftime(time(NULL) , startTimer);
			int hr=seconds/3600;
			int mi=(seconds%3600)/60;
			int sec= (seconds%3600)%60;
			SCORE = hr*10000 + mi*100 + sec ;
			std::string temp_timer= std::to_string(hr)+":"+std::to_string(mi)+":"+std::to_string(sec);
			int length= temp_timer.size();
			for(int pos=0;pos<length;pos++){
				timer[pos]=temp_timer[pos];
			}
			timer[length]='\0';
			if(checkSolution){
				
				if(difftime(time(NULL) , startTimer) >= checkLightTime){
					checkLightTime=1e10;
					checkSolution = false;
					CheckButton.setTexture(TextureCache[10]);
				}
			}
		}

		SDL_Texture* timerTexture = NULL;
		SDL_Color fontColour = { 0, 0, 0, SDL_ALPHA_OPAQUE }; // black
		loadTexture(timerTexture, timer, fontColour);
		Timer.setTexture(timerTexture);
		Timer.renderButton(renderer);
		Timer.centerTextureRect();
		Timer.renderTexture(renderer);
		SDL_DestroyTexture(timerTexture);
		timerTexture = NULL;

		CheckButton.renderButton(renderer);
		CheckButton.renderTexture(renderer);

		// Render new button
		NewButton.renderButton(renderer);
		NewButton.renderTexture(renderer);

		// Render menu button
		MenuButton.renderButton(renderer);
		MenuButton.renderTexture(renderer);

		SDL_RenderPresent(renderer);
		
	}

	// Disable text input
	SDL_StopTextInput();

}

void Sudoku::close(){
	// Destroy
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	renderer = nullptr;
	window = nullptr;

	// Free font
	TTF_CloseFont(font);
	font = nullptr;

	// Quit
	SDL_Quit();
	TTF_Quit();
}

