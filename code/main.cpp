#include <SDL.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>

#define WINDOW_HEIGHT 480  
#define WINDOW_WIDTH 640 
#define BLOCK_SIZE (WINDOW_WIDTH/20)
#define ARR_ROWS (WINDOW_HEIGHT/BLOCK_SIZE)
#define ARR_COLUMNS (WINDOW_WIDTH/BLOCK_SIZE)
#define DEFAULT_X_START (ARR_COLUMNS/2)
#define DEFAULT_Y_START (ARR_ROWS/2)
#define DRAW_BLOCK_INCREMENT 2
#define X_BLOCK_DRAW ((x*BLOCK_SIZE)+DRAW_BLOCK_INCREMENT)
#define Y_BLOCK_DRAW ((y*BLOCK_SIZE)+DRAW_BLOCK_INCREMENT)
#define MAX_WIDTH_BLOCKS (ARR_COLUMNS-1)
#define MAX_HEIGHT_BLOCKS (ARR_ROWS-1)
//Each block is 32 pixels by 32 pixels (640 / 20)
//Default size is 20 blocks by 15 blocks (Width x Height)
#define TEXT_BLOCK_HEIGHT 5
#define TEXT_BLOCK_WIDTH (TEXT_BLOCK_HEIGHT+1) 
//plus 1 for spacing between letters

#define NUM_OF_LETTERS 38
//plus one for a 'space' character
#define TIME_INTERVAL 250

enum Direction {// forces any Direction type variables to UP,DOWN,L...
	UP,
	DOWN,
	LEFT,
	RIGHT
};

struct ColorStruct {
	int r;
	int b;
	int g;
	
};

struct SnakeBlock {
	int col;
	int row;
	Direction direction;
	
};
struct TextBlock{
	int x; //x & y positions
	int y;
	int font;
	char* message;
};

inline void top_left_eye(SnakeBlock snakeArr, SDL_Surface* WindowSurface, Uint32 BlackPixel, int drawBlockIncrement)
{
	SDL_Rect Temp = {(snakeArr.col*BLOCK_SIZE)+2*drawBlockIncrement,(snakeArr.row*BLOCK_SIZE)+2*drawBlockIncrement, 5, 5};
	SDL_FillRect(WindowSurface, &Temp, BlackPixel);
}
inline void top_right_eye(SnakeBlock snakeArr, SDL_Surface* WindowSurface, Uint32 BlackPixel, int drawBlockIncrement, int DrawBlockSize)
{//inline makes it so that when the function is called it just copies and paste code directly in (since calling would take more time), only used for small functions
	SDL_Rect Temp2 = {(snakeArr.col*BLOCK_SIZE)+(DrawBlockSize - 2.5*drawBlockIncrement),(snakeArr.row*BLOCK_SIZE)+2*drawBlockIncrement, 5, 5};
	SDL_FillRect(WindowSurface, &Temp2, BlackPixel);
}
inline void bot_left_eye(SnakeBlock snakeArr, SDL_Surface* WindowSurface, Uint32 BlackPixel, int drawBlockIncrement, int DrawBlockSize)
{
	SDL_Rect Temp3 = {(snakeArr.col*BLOCK_SIZE)+2*drawBlockIncrement,(snakeArr.row*BLOCK_SIZE)+(DrawBlockSize - 2.5*drawBlockIncrement), 5, 5};
	SDL_FillRect(WindowSurface, &Temp3, BlackPixel);
}
inline void bot_right_eye(SnakeBlock snakeArr, SDL_Surface* WindowSurface, Uint32 BlackPixel, int drawBlockIncrement, int DrawBlockSize)
{
	SDL_Rect Temp4 = {(snakeArr.col*BLOCK_SIZE)+(DrawBlockSize - 2.5*drawBlockIncrement),(snakeArr.row*BLOCK_SIZE)+(DrawBlockSize - 2.5*drawBlockIncrement), 5, 5};
	SDL_FillRect(WindowSurface, &Temp4, BlackPixel);
}

void intializeText(SDL_Surface* surface);
void drawText(TextBlock msg, SDL_Surface* WindowSurface,SDL_Surface* alphabetSurface, ColorStruct color);

void centerText(TextBlock* msg){
	int numOfLetters = sizeof(msg->message);
	msg->x -= (numOfLetters * (msg->font) * TEXT_BLOCK_WIDTH)/2;
	
}
 
int wmain(int argc,char* argv[])
{
	assert(SDL_Init(SDL_INIT_VIDEO) == 0);//makes sure SDL is properly initialized

	int DrawBlockSize = BLOCK_SIZE - (2 * DRAW_BLOCK_INCREMENT);//Draws it a little smaller so that program would have outline
	int gridArr[ARR_COLUMNS][ARR_ROWS] = {};//sets to 0s
	int prevArr[ARR_COLUMNS][ARR_ROWS] = {};
	int currentCol = DEFAULT_X_START;
	int currentRow = DEFAULT_Y_START;
	int snakeX = DEFAULT_X_START;
	int snakeY = DEFAULT_Y_START;
	int mouse_x;
	int mouse_y;
	int score = 1;
	int size = 1;
	int prevRow = 0;
	int prevCol = 0;
	int prevCol2, prevRow2;
	int prevState = 0;
	bool counting = false;
	bool changes = false;
	bool keyPressed = false;
	bool menu = true;
	int count = 0;
	int tailCol, tailRow;
	
	
	srand(time(NULL));//time(NULL) is current time;
	int randX = rand() % (ARR_COLUMNS);
	int randY = rand() % (ARR_ROWS);
	
	SnakeBlock snakeArr[ARR_COLUMNS * ARR_ROWS] = {};
	snakeArr[0] = {DEFAULT_X_START,DEFAULT_Y_START,Direction::UP};//Where snake's head initially starts
	
	gridArr[randX][randY] = 2;//Initial separate block

	SDL_Window* Window = SDL_CreateWindow("Snake horribly made by dpak:P",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
	SDL_Surface* WindowSurface = SDL_GetWindowSurface(Window);
	
	SDL_Surface* alphabetSurface = SDL_CreateRGBSurface(0,TEXT_BLOCK_WIDTH*NUM_OF_LETTERS,TEXT_BLOCK_HEIGHT,32,0,0,0,0);//plus 1 for spacing between letters
	
	intializeText(alphabetSurface);
	Uint32 currentTime = SDL_GetTicks();
	Uint32 pastTime = currentTime;
	
	Uint32 WhitePixel = SDL_MapRGB(WindowSurface->format, 255, 255, 255);
	Uint32 BlackPixel = SDL_MapRGB(WindowSurface->format, 0, 0, 0);
	Uint32 BluePixel = SDL_MapRGB(WindowSurface->format, 0, 0, 255);
	Uint32 RedPixel = SDL_MapRGB(WindowSurface->format, 255, 0, 0);
	
	bool Running = true;
	while(Running)
	{
		SDL_Event Event;
		keyPressed = false;
		
		while (SDL_PollEvent (&Event))
		{
			if (Event.type == SDL_QUIT)
			{
				Running = false;
			}
			else if (Event.type == SDL_KEYDOWN)
			{
				if(Event.key.keysym.sym == SDLK_ESCAPE)
					{
						Running = false;
					}
				if(!menu){
					prevCol = currentCol;
					prevRow = currentRow;
					keyPressed = true;
					
					if(Event.key.keysym.sym == SDLK_RIGHT && (snakeArr[0].direction != RIGHT))
					{
						if (currentCol<(ARR_COLUMNS-1) && ((size == 1) || (snakeArr[1].col != (currentCol + 1)))) //Since its an array index it would be -1
						{
							currentCol++;
							changes = true;
							snakeArr[0].direction = RIGHT;
						}
					}
					else if(Event.key.keysym.sym == SDLK_LEFT && (snakeArr[0].direction != LEFT))
					{
						if(currentCol > 0 && ((size == 1) || (snakeArr[1].col != (currentCol - 1))))
						{
							currentCol--;
							changes = true;
							snakeArr[0].direction = LEFT;
						}
					}
					else if(Event.key.keysym.sym == SDLK_UP && (snakeArr[0].direction != UP))
					{
						if(currentRow > 0 && ((size == 1) || (snakeArr[1].row != (currentRow - 1))))
						{
							currentRow--;
							changes = true;
							snakeArr[0].direction = UP;
						}
					}
					else if(Event.key.keysym.sym == SDLK_DOWN && (snakeArr[0].direction != DOWN))
					{
						if((currentRow < (ARR_ROWS-1)) && ((size == 1) || (snakeArr[1].row != (currentRow + 1))))
						{
							currentRow++;
							changes = true;
							snakeArr[0].direction = DOWN;
						}					
					}
				}
			}
		}
		
		if(menu){

			SDL_GetMouseState(&mouse_x,&mouse_y);
			if((3*WINDOW_WIDTH/16 <= (mouse_x)  && (mouse_x) <= ((3*WINDOW_WIDTH/16) + (5*WINDOW_WIDTH/8))) && (5*WINDOW_HEIGHT/14 <= (mouse_y) && (mouse_y) <= ((5*WINDOW_HEIGHT/14) + (WINDOW_HEIGHT/8)))){
				menu = false;
			}
			
			ColorStruct menuColor = {255,0,0};
			TextBlock menuSign = {WINDOW_WIDTH/8,WINDOW_HEIGHT/12, 20, "SNAKE"};//x,y,,FontSize,String
			drawText(menuSign, WindowSurface, alphabetSurface, menuColor);
			
			SDL_Rect Temp = {3*WINDOW_WIDTH/16, 5*WINDOW_HEIGHT/14, 5*WINDOW_WIDTH/8, WINDOW_HEIGHT/8};
			SDL_FillRect(WindowSurface, &Temp, BluePixel);
			
			ColorStruct playColor = {255,0,255};
			TextBlock playSign = {WINDOW_WIDTH/2, (21*WINDOW_HEIGHT/56), 10, "PLAY"};
			centerText(&playSign);
			drawText(playSign, WindowSurface, alphabetSurface, playColor);
			
			
		}
		else{
			currentTime = SDL_GetTicks();
			
			if(((currentTime - pastTime) >= TIME_INTERVAL) && (keyPressed== false)){//if 1 second has passed move snake head in direction
				if((snakeArr[0].direction == UP) && (snakeArr[0].row != 0)){
					// pastTime = currentTime;
					prevRow = currentRow;
					prevCol = currentCol;//is needed for first time run(if there is no keydown event)
					currentRow--;
					changes = true;
				}
				else if((snakeArr[0].direction == RIGHT) && (snakeArr[0].col != MAX_WIDTH_BLOCKS)){
					// pastTime = currentTime;
					prevRow = currentRow;
					prevCol = currentCol;
					currentCol++;
					changes = true;
				}
				else if((snakeArr[0].direction == LEFT) && (snakeArr[0].col != 0)){
					// pastTime = currentTime;
					prevRow = currentRow;
					prevCol = currentCol;
					currentCol--;
					changes = true;
				}
				else if((snakeArr[0].direction == DOWN) && (snakeArr[0].row != MAX_HEIGHT_BLOCKS)){
					// pastTime = currentTime;
					prevRow = currentRow;
					prevCol = currentCol;
					currentRow++;
					changes = true;
				}
			}
			
			if((counting) && (changes))//counting the number of blocks that has passed the separate block
			{
				
				if(count == size)//if the snake head has surpassed the size the tail will now be joined at the end
				{
					snakeArr[size] = {tailCol,tailRow,Direction::UP};//direction does not matter because it is not the snake head
					size++;
					counting = false;
				}
				else if(count == 0) score++;
				count++;
			}

			if(gridArr[currentCol][currentRow] == 2)//if snake goes over separate block
			{
				count = 0; //used to know when to add the separate block to the end of the snake
				counting = true;
				tailCol = currentCol;//snake's new tail will be at this block
				tailRow = currentRow;
				gridArr[currentCol][currentRow] = 0;
				randX = rand() % (ARR_COLUMNS);
				randY = rand() % (ARR_ROWS);
				gridArr[randX][randY] = 2;
			}
			
			if(changes){//if there are any changes, change col and row accordingly
				for (int i = size; i>0; i--)//newer blocks row and col become previous blocks rows/col
				{
					snakeArr[i].col = snakeArr[(i-1)].col;
					snakeArr[i].row = snakeArr[(i-1)].row;
				}
				snakeArr[0].col += (currentCol - prevCol);
				snakeArr[0].row += (currentRow - prevRow);
				pastTime = currentTime;
				changes = false;
			}
			
			SDL_FillRect(WindowSurface, 0, BlackPixel); //Background
			
			for(int x = 0; x < ARR_COLUMNS; x++)//draws the random box
			{
				for(int y = 0; y <ARR_ROWS; y++)
				{
					if (gridArr[x][y]==2)
					{
						SDL_Rect Temp = {X_BLOCK_DRAW, Y_BLOCK_DRAW, DrawBlockSize, DrawBlockSize};
						SDL_FillRect(WindowSurface, &Temp, BluePixel);
					}
				}
			}
			
			for(int i = 0; i < size; i++) // draws all of the snake blocks
			{
				if((i!=0) && (snakeArr[0].col==snakeArr[i].col) && (snakeArr[0].row==snakeArr[i].row))
				{
					//Running = false;
					printf("poop");
				}

				SDL_Rect Temp = {(snakeArr[i].col*BLOCK_SIZE)+DRAW_BLOCK_INCREMENT,(snakeArr[i].row*BLOCK_SIZE)+DRAW_BLOCK_INCREMENT, DrawBlockSize, DrawBlockSize};
				SDL_FillRect(WindowSurface, &Temp, WhitePixel);
				
				if(i==0)
				{
					if(snakeArr[0].direction==UP)
					{
						top_left_eye(snakeArr[i], WindowSurface, BlackPixel, DRAW_BLOCK_INCREMENT);
						top_right_eye(snakeArr[i], WindowSurface, BlackPixel, DRAW_BLOCK_INCREMENT, DrawBlockSize);
					}
					else if(snakeArr[0].direction==DOWN)
					{
						bot_left_eye(snakeArr[i], WindowSurface, BlackPixel, DRAW_BLOCK_INCREMENT, DrawBlockSize);
						bot_right_eye(snakeArr[i], WindowSurface, BlackPixel, DRAW_BLOCK_INCREMENT, DrawBlockSize);
					}
					else if(snakeArr[0].direction==LEFT)
					{
						top_left_eye(snakeArr[i], WindowSurface, BlackPixel, DRAW_BLOCK_INCREMENT);
						bot_left_eye(snakeArr[i], WindowSurface, BlackPixel, DRAW_BLOCK_INCREMENT, DrawBlockSize);
					}
					else if(snakeArr[0].direction==RIGHT)
					{
						top_right_eye(snakeArr[i], WindowSurface, BlackPixel, DRAW_BLOCK_INCREMENT, DrawBlockSize);
						bot_right_eye(snakeArr[i], WindowSurface, BlackPixel, DRAW_BLOCK_INCREMENT, DrawBlockSize);
					}
				}
			}	
			
			char scoreMsg[20];
			
			sprintf(scoreMsg,"Score:%d", score);//takes score Integer and puts it into scoreMsg then scoreMsg equals to "Score: (score)"
			
			TextBlock scoreMessage = {(2*WINDOW_WIDTH/5), 0, WINDOW_WIDTH/200, scoreMsg};//x,y,font size, message
			ColorStruct scoreColor = {255,0,0};
			
			drawText(scoreMessage, WindowSurface, alphabetSurface, scoreColor);
		}			

		SDL_UpdateWindowSurface(Window);
		
	}
	return 0;
}

void drawText(TextBlock msg, SDL_Surface* WindowSurface,SDL_Surface* alphabetSurface, ColorStruct color){
	
	char currentChar = *msg.message;
	int index, count = 0;
	SDL_SetSurfaceColorMod(alphabetSurface, color.r , color.g ,color.b);
	
	while(currentChar != '\0'){
		if((currentChar == 'A') || (currentChar == 'a')){
			index = 0;
		}
		else if((currentChar == 'B') || (currentChar == 'b')){
			index = 1;
		}
		else if((currentChar == 'C') || (currentChar == 'c')){
			index = 2;
		}
		else if((currentChar == 'D') || (currentChar == 'd')){
			index = 3;
		}
		else if((currentChar == 'E') || (currentChar == 'e')){
			index = 4;
		}
		else if((currentChar == 'F') || (currentChar == 'f')){
			index = 5;
		}
		else if((currentChar == 'G') || (currentChar == 'g')){
			index = 6;
		}
		else if((currentChar == 'H') || (currentChar == 'h')){
			index = 7;
		}
		else if((currentChar == 'I') || (currentChar == 'i')){
			index = 8;
		}
		else if((currentChar == 'J') || (currentChar == 'j')){
			index = 9;
		}
		else if((currentChar == 'K') || (currentChar == 'k')){
			index = 10;
		}
		else if((currentChar == 'L') || (currentChar == 'l')){
			index = 11;
		}
		else if((currentChar == 'M') || (currentChar == 'm')){
			index = 12;
		}
		else if((currentChar == 'N') || (currentChar == 'n')){
			index = 13;
		}
		else if((currentChar == 'O') || (currentChar == 'o')){
			index = 14;
		}
		else if((currentChar == 'P') || (currentChar == 'p')){
			index = 15;
		}
		else if((currentChar == 'Q') || (currentChar == 'q')){
			index = 16;
		}
		else if((currentChar == 'R') || (currentChar == 'r')){
			index = 17;
		}
		else if((currentChar == 'S') || (currentChar == 's')){
			index = 18;
		}
		else if((currentChar == 'T') || (currentChar == 't')){
			index = 19;
		}
		else if((currentChar == 'U') || (currentChar == 'u')){
			index = 20;
		}
		else if((currentChar == 'V') || (currentChar == 'v')){
			index = 21;
		}
		else if((currentChar == 'W') || (currentChar == 'w')){
			index = 22;
		}
		else if((currentChar == 'X') || (currentChar == 'x')){
			index = 23;
		}
		else if((currentChar == 'Y') || (currentChar == 'y')){
			index = 24;
		}
		else if((currentChar == 'Z') || (currentChar == 'z')){
			index = 25;
		}
		else if((currentChar == '1')){
			index = 26;
		}
		else if((currentChar == '2')){
			index = 27;
		}
		else if((currentChar == '3')){
			index = 28;
		}
		else if((currentChar == '4')){
			index = 29;
		}
		else if((currentChar == '5')){
			index = 30;
		}
		else if((currentChar == '6')){
			index = 31;
		}
		else if((currentChar == '7')){
			index = 32;
		}
		else if((currentChar == '8')){
			index = 33;
		}
		else if((currentChar == '9')){
			index = 34;
		}
		else if((currentChar == '0')){
			index = 35;
		}
		else if((currentChar == ':')){
			index = 36;
		}
		else{//for ' '
			index = 37;
		}
		
		int currentChar_x = (TEXT_BLOCK_WIDTH * count * msg.font) + msg.x;
		int currentChar_y = msg.y;
		
		SDL_Rect alphaRect = {index*TEXT_BLOCK_WIDTH,0,(TEXT_BLOCK_WIDTH),TEXT_BLOCK_HEIGHT};//x,y,w,h
		SDL_Rect surfaceRect = {currentChar_x, currentChar_y, (msg.font * TEXT_BLOCK_WIDTH), (msg.font * TEXT_BLOCK_HEIGHT)};
		SDL_BlitScaled(alphabetSurface, &alphaRect, WindowSurface, &surfaceRect);//Blitting Letters to WindowSurface
		
		count++;
		currentChar = *(msg.message + count);// address of msg (move one char forward) + count
	}
}


//I know i could have used a font file but I was bored and wanted to create everything on my own
void intializeText(SDL_Surface* surface)
{//create on surface with each letter block, intialize once b4 main loop, and point back when needed
	int surfaceArr[(TEXT_BLOCK_WIDTH*NUM_OF_LETTERS)][(TEXT_BLOCK_HEIGHT*NUM_OF_LETTERS)] = {};
	
	Uint32 WhitePixel = SDL_MapRGB(surface->format, 255, 255, 255); 
	Uint32 TransparentPixel = SDL_MapRGB(surface->format, 0, 255, 255);//pixel is cyan
	SDL_SetColorKey(surface, SDL_TRUE,TransparentPixel);//sets cyan to be the transparentPixel

	//  |
	// | |
	//|   |
	//|||||
	//|   |
	surfaceArr[2][0] = 1;																		  
	surfaceArr[1][1] = 1;surfaceArr[3][1] = 1;													  
	surfaceArr[0][2] = 1;surfaceArr[4][2] = 1;
	surfaceArr[0][3] = 1;surfaceArr[1][3] = 1;surfaceArr[2][3] = 1;surfaceArr[3][3] = 1;surfaceArr[4][3] = 1;												  	
	surfaceArr[0][4] = 1;surfaceArr[4][4] = 1;													  
	

	//||||
	//|   |
	//||||
	//|   |
	//||||
	surfaceArr[(TEXT_BLOCK_WIDTH*1)][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*1)+1][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*1)+2][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*1)+3][0] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*1)][1] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*1)+4][1] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*1)][2] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*1)+1][2] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*1)+2][2] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*1)+3][2] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*1)][3] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*1)+4][3] = 1;								       
	surfaceArr[(TEXT_BLOCK_WIDTH*1)][4] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*1)+1][4] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*1)+2][4] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*1)+3][4] = 1;

	// ||||
	//|
	//|
	//|
	// ||||
	surfaceArr[(TEXT_BLOCK_WIDTH*2)+1][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*2)+2][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*2)+3][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*2)+4][0] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*2)][1] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*2)][2] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*2)][3] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*2)+1][4] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*2)+2][4] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*2)+3][4] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*2)+4][4] = 1;
	
	//||||
	//|   |
	//|   |
	//|   |
	//||||
	surfaceArr[(TEXT_BLOCK_WIDTH*3)][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*3)+1][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*3)+2][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*3)+3][0] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*3)][1] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*3)+4][1] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*3)][2] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*3)+4][2] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*3)][3] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*3)+4][3] = 1;								       
	surfaceArr[(TEXT_BLOCK_WIDTH*3)][4] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*3)+1][4] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*3)+2][4] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*3)+3][4] = 1;

	//|||||
	//|
	//||||
	//|
	//|||||
	surfaceArr[(TEXT_BLOCK_WIDTH*4)][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*4)+1][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*4)+2][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*4)+3][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*4)+4][0] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*4)][1] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*4)][2] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*4)+1][2] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*4)+2][2] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*4)+3][2] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*4)][3] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*4)][4] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*4)+1][4] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*4)+2][4] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*4)+3][4] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*4)+4][4] = 1;
	
	//|||||
	//|
	//||||
	//|
	//|
	surfaceArr[(TEXT_BLOCK_WIDTH*5)][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*5)+1][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*5)+2][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*5)+3][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*5)+4][0] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*5)][1] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*5)][2] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*5)+1][2] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*5)+2][2] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*5)+3][2] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*5)][3] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*5)][4] = 1;

	// ||||
	//|
	//| |||
	//|   |
	// |||
	surfaceArr[(TEXT_BLOCK_WIDTH*6)+1][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*6)+2][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*6)+3][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*6)+4][0] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*6)][1] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*6)][2] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*6)+2][2] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*6)+3][2] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*6)+4][2] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*6)][3] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*6)+4][3] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*6)+1][4] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*6)+2][4] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*6)+3][4] = 1;
	
	//|   |
	//|   |
	//|||||
	//|   |
	//|   |
	surfaceArr[(TEXT_BLOCK_WIDTH*7)][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*7)+4][0] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*7)][1] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*7)+4][1] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*7)][2] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*7)+1][2] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*7)+2][2] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*7)+3][2] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*7)+4][2] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*7)][3] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*7)+4][3] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*7)][4] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*7)+4][4] = 1;
	
	//|||||
	//  |
	//  |
	//  |
	//|||||
	surfaceArr[(TEXT_BLOCK_WIDTH*8)][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*8)+1][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*8)+2][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*8)+3][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*8)+4][0] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*8)+2][1] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*8)+2][2] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*8)+2][3] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*8)][4] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*8)+1][4] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*8)+2][4] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*8)+3][4] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*8)+4][4] = 1;
	
	//|||||
	//   |
	//   |
	//|  |
	// ||
	surfaceArr[(TEXT_BLOCK_WIDTH*9)][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*9)+1][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*9)+2][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*9)+3][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*9)+4][0] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*9)+3][1] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*9)+3][2] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*9)][3] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*9)+3][3] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*9)+2][4] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*9)+2][4] = 1;
	
	//|   |
	//|  |
	//|||
	//|  |
	//|   |
	surfaceArr[(TEXT_BLOCK_WIDTH*10)][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*10)+4][0] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*10)][1] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*10)+3][1] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*10)][2] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*10)+1][2] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*10)+2][2] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*10)][3] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*10)+3][3] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*10)][4] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*10)+4][4] = 1;
	
	//|
	//|
	//|
	//|
	//|||||
	surfaceArr[(TEXT_BLOCK_WIDTH*11)][0] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*11)][1] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*11)][2] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*11)][3] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*11)][4] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*11)+1][4] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*11)+2][4] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*11)+3][4] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*11)+4][4] = 1;
	
	//|   |
	//|| ||
	//| | |
	//| | |
	//| | |
	surfaceArr[(TEXT_BLOCK_WIDTH*12)][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*12)+4][0] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*12)][1] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*12)+1][1] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*12)+3][1] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*12)+4][1] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*12)][2] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*12)+2][2] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*12)+4][2] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*12)][3] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*12)+2][3] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*12)+4][3] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*12)][4] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*12)+2][4] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*12)+4][4] = 1;
	
	//|   |
	//||  |
	//| | |
	//|  ||
	//|   |
	surfaceArr[(TEXT_BLOCK_WIDTH*13)][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*13)+4][0] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*13)][1] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*13)+1][1] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*13)+4][1] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*13)][2] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*13)+2][2] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*13)+4][2] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*13)][3] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*13)+3][3] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*13)+4][3] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*13)][4] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*13)+4][4] = 1;
	
	//|||||
	//|   |
	//|   |
	//|   |
	//|   |
	//|||||
	surfaceArr[(TEXT_BLOCK_WIDTH*14)][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*14)+1][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*14)+2][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*14)+3][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*14)+4][0] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*14)][1] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*14)+4][1] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*14)][2] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*14)+4][2] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*14)][3] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*14)+4][3] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*14)][4] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*14)+1][4] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*14)+2][4] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*14)+3][4] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*14)+4][4] = 1;
	
	//||||
	//|   |
	//||||
	//|
	//|
	surfaceArr[(TEXT_BLOCK_WIDTH*15)][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*15)+1][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*15)+2][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*15)+3][0] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*15)][1] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*15)+4][1] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*15)][2] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*15)+1][2] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*15)+2][2] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*15)+3][2] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*15)][3] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*15)][4] = 1;
	
	//|||||
	//|   |
	//| | |
	//|  |
	//||| |
	surfaceArr[(TEXT_BLOCK_WIDTH*16)][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*16)+1][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*16)+2][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*16)+3][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*16)+4][0] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*16)][1] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*16)+4][1] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*16)][2] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*16)+2][2] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*16)+4][2] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*16)][3] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*16)+3][3] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*16)][4] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*16)+1][4] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*16)+2][4] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*16)+4][4] = 1;
	
	//||||
	//|   |
	//||||
	//|  |
	//|   |
	surfaceArr[(TEXT_BLOCK_WIDTH*17)][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*17)+1][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*17)+2][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*17)+3][0] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*17)][1] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*17)+4][1] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*17)][2] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*17)+1][2] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*17)+2][2] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*17)+3][2] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*17)][3] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*17)+3][3] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*17)][4] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*17)+4][4] = 1;
	
	//|||||
	//|
	//|||||
	//    |
	//|||||
	surfaceArr[(TEXT_BLOCK_WIDTH*18)][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*18)+1][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*18)+2][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*18)+3][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*18)+4][0] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*18)][1] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*18)][2] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*18)+1][2] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*18)+2][2] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*18)+3][2] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*18)+4][2] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*18)+4][3] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*18)][4] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*18)+1][4] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*18)+2][4] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*18)+3][4] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*18)+4][4] = 1;
	
	//|||||
	//  |
	//  |
	//  |
	//  |
	surfaceArr[(TEXT_BLOCK_WIDTH*19)][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*19)+1][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*19)+2][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*19)+3][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*19)+4][0] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*19)+2][1] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*19)+2][2] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*19)+2][3] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*19)+2][4] = 1;

	//|   |
	//|   |
	//|   |
	//|   |
	// |||
	surfaceArr[(TEXT_BLOCK_WIDTH*20)][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*20)+4][0] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*20)][1] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*20)+4][1] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*20)][2] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*20)+4][2] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*20)][3] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*20)+4][3] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*20)+1][4] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*20)+2][4] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*20)+3][4] = 1;
	
	//|   |
	//|   |
	//|   |
	// | |
	//  |
	surfaceArr[(TEXT_BLOCK_WIDTH*21)][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*21)+4][0] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*21)][1] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*21)+4][1] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*21)][2] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*21)+4][2] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*21)+1][3] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*21)+3][3] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*21)+2][4] = 1;
	
	//| | |
	//| | |
	//| | |
	//| | |
	// | |
	surfaceArr[(TEXT_BLOCK_WIDTH*22)][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*22)+2][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*22)+4][0] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*22)][1] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*22)+2][1] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*22)+4][1] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*22)][2] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*22)+2][2] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*22)+4][2] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*22)][3] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*22)+2][3] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*22)+4][3] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*22)+1][4] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*22)+3][4] = 1;
	
	//|   |
	// | |
	//  |
	// | |
	//|   |
	surfaceArr[(TEXT_BLOCK_WIDTH*23)][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*23)+4][0] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*23)+1][1] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*23)+3][1] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*23)+2][2] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*23)+1][3] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*23)+3][3] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*23)][4] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*23)+4][4] = 1;

	
	//|   |
	// | |
	//  |
	//  |
	//  |
	surfaceArr[(TEXT_BLOCK_WIDTH*24)][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*24)+4][0] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*24)+1][1] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*24)+3][1] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*24)+2][2] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*24)+2][3] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*24)+2][4] = 1;
	
	//|||||
	//   |
	//  |
	// |
	//|||||
	surfaceArr[(TEXT_BLOCK_WIDTH*25)][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*25)+1][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*25)+2][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*25)+3][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*25)+4][0] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*25)+3][1] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*25)+2][2] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*25)+1][3] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*25)][4] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*25)+1][4] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*25)+2][4] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*25)+3][4] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*25)+4][4] = 1;
	
	//|||
	//  |
	//  |
	//  |
	//|||||
	surfaceArr[(TEXT_BLOCK_WIDTH*26)][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*26)+1][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*26)+2][0] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*26)+2][1] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*26)+2][2] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*26)+2][3] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*26)][4] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*26)+1][4] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*26)+2][4] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*26)+3][4] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*26)+4][4] = 1;
	
	//|||||
	//    |
	//|||||
	//|
	//|||||
	surfaceArr[(TEXT_BLOCK_WIDTH*27)][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*27)+1][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*27)+2][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*27)+3][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*27)+4][0] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*27)+4][1] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*27)][2] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*27)+1][2] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*27)+2][2] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*27)+3][2] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*27)+4][2] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*27)][3] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*27)][4] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*27)+1][4] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*27)+2][4] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*27)+3][4] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*27)+4][4] = 1;
	
	//|||||
	//    |
	//|||||
	//    |
	//|||||
	surfaceArr[(TEXT_BLOCK_WIDTH*28)][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*28)+1][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*28)+2][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*28)+3][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*28)+4][0] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*28)+4][1] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*28)][2] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*28)+1][2] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*28)+2][2] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*28)+3][2] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*28)+4][2] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*28)+4][3] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*28)][4] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*28)+1][4] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*28)+2][4] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*28)+3][4] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*28)+4][4] = 1;
	
	//|   |
	//|   |
	//|||||
	//    |
	//    |
	surfaceArr[(TEXT_BLOCK_WIDTH*29)][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*29)+4][0] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*29)][1] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*29)+4][1] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*29)][2] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*29)+1][2] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*29)+2][2] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*29)+3][2] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*29)+4][2] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*29)+4][3] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*29)+4][4] = 1;
	
	//|||||
	//|    
	//||||
	//    |
	//||||
	surfaceArr[(TEXT_BLOCK_WIDTH*30)][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*30)+1][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*30)+2][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*30)+3][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*30)+4][0] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*30)][1] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*30)][2] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*30)+1][2] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*30)+2][2] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*30)+3][2] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*30)+4][3] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*30)][4] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*30)+1][4] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*30)+2][4] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*30)+3][4] = 1;
	
	//|||||
	//|    
	//|||||
	//|   |
	//|||||
	surfaceArr[(TEXT_BLOCK_WIDTH*31)][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*31)+1][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*31)+2][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*31)+3][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*31)+4][0] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*31)][1] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*31)][2] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*31)+1][2] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*31)+2][2] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*31)+3][2] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*31)+4][2] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*31)][3] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*31)+4][3] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*31)][4] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*31)+1][4] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*31)+2][4] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*31)+3][4] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*31)+4][4] = 1;
	
	//|||||
	//    |
	//    |
	//    |
	//    |
	surfaceArr[(TEXT_BLOCK_WIDTH*32)][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*32)+1][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*32)+2][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*32)+3][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*32)+4][0] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*32)+4][1] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*32)+4][2] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*32)+4][3] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*32)+4][4] = 1;
	
	// ||| 
	//|   |
	// |||
	//|   |
	// |||
	surfaceArr[(TEXT_BLOCK_WIDTH*33)+1][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*33)+2][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*33)+3][0] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*33)][1] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*33)+4][1] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*33)+1][2] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*33)+2][2] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*33)+3][2] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*33)][3] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*33)+4][3] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*33)+1][4] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*33)+2][4] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*33)+3][4] = 1;
	
	//|||||
	//|   |
	//|||||
	//    |
	//    |
	surfaceArr[(TEXT_BLOCK_WIDTH*34)][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*34)+1][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*34)+2][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*34)+3][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*34)+4][0] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*34)][1] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*34)+4][1] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*34)][2] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*34)+1][2] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*34)+2][2] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*34)+3][2] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*34)+4][2] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*34)+4][3] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*34)+4][4] = 1;
	
	// |||
	//||  |
	//| | |
	//|  ||
	// |||
	surfaceArr[(TEXT_BLOCK_WIDTH*35)+1][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*35)+2][0] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*35)+3][0] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*35)][1] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*35)+1][1] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*35)+4][1] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*35)][2] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*35)+2][2] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*35)+4][2] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*35)][3] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*35)+3][3] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*35)+4][3] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*35)+1][4] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*35)+2][4] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*35)+3][4] = 1;
	
	//
	//||
	//
	//||
	//
	surfaceArr[(TEXT_BLOCK_WIDTH*36)][1] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*36)+1][1] = 1;
	surfaceArr[(TEXT_BLOCK_WIDTH*36)][3] = 1;surfaceArr[(TEXT_BLOCK_WIDTH*36)+1][3] = 1;
	
	for(int x = 0; x<(TEXT_BLOCK_WIDTH*NUM_OF_LETTERS); x++)//draws each coloured or transparent block
	{
		for(int y = 0; y<(TEXT_BLOCK_HEIGHT*NUM_OF_LETTERS); y++)
		{
			if(surfaceArr[x][y]==1){
				SDL_Rect Temp = {x,y,1,1};
				SDL_FillRect(surface, &Temp, WhitePixel);
			}
			else if(surfaceArr[x][y]==0){
				SDL_Rect Temp = {x,y,1,1};
				SDL_FillRect(surface, &Temp, TransparentPixel);
			}
		}
	}
	

}